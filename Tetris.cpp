#include "Tetris.h"

int Tetris::holdcolorNum = 0, Tetris::holdShapeNum = 0;
Vector2i Tetris::holdSquare[4] = { {0,0},{0,0},{0,0},{0,0} };

Tetris::Tetris() {
	dx = 0;
	rotate = false;
	colorNum = 1;
	timer = 0;
	delay = DELAYVALUE;
}

Tetris::~Tetris() {

}

void Tetris::Initial(Texture* tex) {
	tTiles = tex;
	dx = 0;
	rotate = false;
	hold = false;
	hardDrop = false;
	animationFlag = true;
	animationCtrlValue = 1.0;

	colorNum = 1;
	timer = 0;
	delay = DELAYVALUE;
	b7Int = 0;
	score = 0;

	if (role == rolePLAYER1) {
		mCornPoint = { P1_STAGE_CORNER_X,P1_STAGE_CORNER_Y };
		nextSquareCornPoint = { P1_NEXT_CORNER_X,P1_NEXT_CORNER_Y };
	}
	if (role == rolePLAYER2) {
		mCornPoint = { P2_STAGE_CORNER_X,P2_STAGE_CORNER_Y };
		nextSquareCornPoint = { P2_NEXT_CORNER_X,P2_NEXT_CORNER_Y };
	}
	holdSquareCornPoint = { HOLD_CORNER_X,HOLD_CORNER_Y };
	sTiles.setTexture(*tTiles);

	colorNum = 1 + rand() % 7;
	currentShapeNum = rand() % 7;

	nextcolorNum = 1 + rand() % 7;
	nextShapeNum = rand() % 7;

	for (int i = 0; i < 4; i++) {
		currentSquare[i].x = Figures[currentShapeNum][i] % 2 + STAGE_WIDTH / 2;
		currentSquare[i].y = Figures[currentShapeNum][i] / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;

		animationRow[i] = -1;
	}
	holdShapeNum = -1;

	shadow();

	for (int i = 0; i < STAGE_HEIGHT; i++)
		for (int j = 0; j < STAGE_WIDTH; j++)
			Field[i][j] = 0;
}

void Tetris::Input(Event event) {
	if (role == rolePLAYER1) {
		if (event.type == Event::KeyPressed) {
			if (event.key.code == Keyboard::W)
				if (currentShapeNum != shapeO)
					rotate = true;
			if (event.key.code == Keyboard::A)
				dx = -1;
			if (event.key.code == Keyboard::D)
				dx = 1;
			if (event.key.code == Keyboard::S)
				delay = DELAYVALUE / 10;
		}
		if (event.type == Event::KeyReleased) {
			if (event.key.code == Keyboard::LControl)
				hold = true;
			if (event.key.code == Keyboard::Space)
				hardDrop = true;
			if (event.key.code == Keyboard::A || event.key.code == Keyboard::D)
				dx = 0;
			if (event.key.code == Keyboard::S)
				delay = DELAYVALUE;
		}
	}
	if (role == rolePLAYER2) {
		if (event.type == Event::KeyPressed) {
			if (event.key.code == Keyboard::Up)
				if (currentShapeNum != shapeO)
					rotate = true;
			if (event.key.code == Keyboard::Left)
				dx = -1;
			if (event.key.code == Keyboard::Right)
				dx = 1;
			if (event.key.code == Keyboard::Down)
				delay = DELAYVALUE / 10;
		}
		if (event.type == Event::KeyReleased) {
			if (event.key.code == Keyboard::RControl)
				hold = true;
			if (event.key.code == Keyboard::Enter)
				hardDrop = true;
			if (event.key.code == Keyboard::Left || event.key.code == Keyboard::Right)
				dx = 0;
			if (event.key.code == Keyboard::Down)
				delay = DELAYVALUE;
		}
	}
}

void Tetris::traditionLogic() {
	//hold
	if (hold) {
		holdFunc();
		hold = false;
	}

	//水平
	xMove();

	//旋转
	if (rotate) {
		rotateFunc();
		rotate = false;
	}

	slowLoading();

	//下落
	if (timer > delay) {
		yMove();
		timer = 0;
	}

	shadow();

	if (hardDrop) {
		hardDropFunc();
		hardDrop = false;
	}
}

void Tetris::Logic() {
	if (!animationFlag)
		traditionLogic();
	//如果有发生新方块生成请求，则先判定是否有动画播放，再 new 新的方块
	if (newShapeFlag) {
		if (animationFlag == false) {
			//check lines
			checkLine();
			if (animationFlag == false)
				newShapeFunc();
			isWin();
		} else {
			animationCtrlValue -= 0.1;
			if (animationCtrlValue < 0) {
				animationFlag = false;
				animationCtrlValue = 1.0;

				clearLine();
				newShapeFunc();
			}
		}
	}
}

void Tetris::xMove() {
	for (int i = 0; i < 4; i++) {
		tempSquare[i] = currentSquare[i];
		currentSquare[i].x += dx;
	}
	if (!hitTest())//如果撞上了
		for (int i = 0; i < 4; i++)
			currentSquare[i] = tempSquare[i];//到左右的边界，不能移出边界
}

void Tetris::yMove() {
	for (int i = 0; i < 4; i++) {
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y++;
	}

	if (!hitTest()) {
		for (int i = 0; i < 4; i++)
			Field[tempSquare[i].y][tempSquare[i].x] = colorNum;

		newShapeFlag = true;
	}
}

void Tetris::newShapeFunc() {
	colorNum = nextcolorNum;
	currentShapeNum = nextShapeNum;

	nextcolorNum = 1 + rand() % 7;
	nextShapeNum = Bag7();

	for (int i = 0; i < 4; i++) {
		currentSquare[i] = nextSquare[i];
		currentSquare[i].x += STAGE_WIDTH / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;
	}

	shadow();

	newShapeFlag = false;

	for (int i = 0; i < 4; i++)
		animationRow[i] = -1;
}

void Tetris::rotateFunc() {
	int originalHeight = currentSquare[0].y;
	for (int j = 0; j < 4; j++) {
		Vector2i p = currentSquare[j];//设置旋转中心
		for (int i = 0; i < 4; i++) {
			//顺时针90°
			int x = currentSquare[i].y - p.y;//原Y方向距离中心点的差值，作为新的差值，传递给X方向
			int y = currentSquare[i].x - p.x;//原X方向距离中心点的差值，作为新的差值，传递给Y方向
			currentSquare[i].x = p.x - x;//新坐标X=中心点坐标‐新的X方向差值
			currentSquare[i].y = p.y + y;//新坐标Y=中心点坐标‐新的Y方向差值
		}
		if (!hitTest())
			for (int i = 0; i < 4; i++)
				currentSquare[i] = tempSquare[i];
		else {
			int detaY = 0;
			detaY = currentSquare[0].y - originalHeight;
			if (detaY != 0)
				for (int i = 0; i < 4; i++)
					currentSquare[i].y -= detaY;
			break;
		}
	}
}

void Tetris::holdFunc() {
	Vector2i backUpSquare[4];
	tempcolorNum = holdcolorNum;
	tempShapeNum = holdShapeNum;

	holdcolorNum = colorNum;
	holdShapeNum = currentShapeNum;

	for (int i = 0; i < 4; i++) {
		holdSquare[i].x = Figures[holdShapeNum][i] % 2;
		holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		tempSquare[i].x = Figures[tempShapeNum][i] % 2;
		tempSquare[i].y = Figures[tempShapeNum][i] / 2;
		backUpSquare[i] = currentSquare[i];
	}

	if (tempShapeNum == -1)//如果原 hold 区为空，则当前图形从Next取
	{
		//取下个方块图形
		colorNum = nextcolorNum;
		currentShapeNum = nextShapeNum;

		//更新下个方块图形
		nextcolorNum = 1 + rand() % 7;
		nextShapeNum = Bag7();

		for (int i = 0; i < 4; i++) {
			currentSquare[i] = nextSquare[i];//当前块更新
			currentSquare[i].x = currentSquare[i].x + STAGE_WIDTH / 2;
			nextSquare[i].x = Figures[nextShapeNum][i] % 2;
			nextSquare[i].y = Figures[nextShapeNum][i] / 2;
		}
	} else//当前图形取原来 hold 图形的值，发生交换
	{
		colorNum = tempcolorNum;
		currentShapeNum = tempShapeNum;
		//从 hold 区置换过来的方块图形，在舞台上坐标的计算
		int minCurrentX = currentSquare[0].x;
		int minCurrentY = currentSquare[0].y;
		int minTempX = tempSquare[0].x;
		int minTempY = tempSquare[0].y;
		int dx, dy;
		for (int i = 1; i < 4; i++) {
			if (currentSquare[i].x < minCurrentX)
				minCurrentX = currentSquare[i].x;
			if (currentSquare[i].y < minCurrentY)
				minCurrentY = currentSquare[i].y;
			if (tempSquare[i].x < minTempX)
				minTempX = tempSquare[i].x;
			if (tempSquare[i].y < minTempY)
				minTempY = tempSquare[i].y;
		}
		dx = minCurrentX - minTempX;
		dy = minCurrentY - minTempY;
		for (int i = 0; i < 4; i++) {
			currentSquare[i].x = tempSquare[i].x + dx;
			currentSquare[i].y = tempSquare[i].y + dy;
			holdSquare[i].x = Figures[holdShapeNum][i] % 2;
			holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		}
	}

	if (!hitTest()) {
		holdcolorNum = tempcolorNum;
		holdShapeNum = tempShapeNum;

		for (int i = 0; i < 4; i++) {
			currentSquare[i] = backUpSquare[i];
		}
	}
}

void Tetris::clearLine() {
	int k = STAGE_HEIGHT - 1;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--) {
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++) {
			if (Field[i][j])
				xCount++;
			Field[k][j] = Field[i][j];
		}
		if (xCount < STAGE_WIDTH)
			k--;
	}
}

void Tetris::checkLine() {
	int k = STAGE_HEIGHT - 1;
	int yCount = 0;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--) {
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++) {
			if (Field[i][j])
				xCount++;
			//Field[k][j] = Field[i][j];
		}
		if (xCount < STAGE_WIDTH)
			k--;
		else {
			animationRow[yCount] = i;
			yCount++;
			animationFlag = true;
		}
	}
	switch (yCount) {
		case 1:
			score += 10;
			break;
		case 2:
			score += 30;
			break;
		case 3:
			score += 60;
			break;
		case 4:
			score += 100;
			break;
	}
}

void Tetris::hardDropFunc() {
	for (int i = 0; i < 4; i++) {
		currentSquare[i] = shadowSquare[i];
	}
}

void Tetris::slowLoading() {
	for (int i = 0; i < 4; i++) {
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y++;
	}

	if (!hitTest()) {
		delay = DELAYVALUE * 2;
	}
	for (int i = 0; i < 4; i++)
		currentSquare[i] = tempSquare[i];
}

void Tetris::shadow() {
	for (int i = 0; i < 4; i++) {
		shadowSquare[i] = currentSquare[i];
	}
	for (int j = 0; j < STAGE_HEIGHT; j++) {
		for (int i = 0; i < 4; i++) {
			currentSquare[i].y++;
		}
		if (!hitTest()) {
			for (int i = 0; i < 4; i++) {
				currentSquare[i].y--;
			}
			break;
		}
	}
	for (int i = 0; i < 4; i++) {
		tempSquare[i] = currentSquare[i];
		currentSquare[i] = shadowSquare[i];
		shadowSquare[i] = tempSquare[i];
	}
}

void Tetris::Draw(RenderWindow* w) {
	window = w;
	if (animationFlag == false) {
		//shadow
		for (int i = 0; i < 4; i++) {
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(shadowSquare[i].x * GRIDSIZE, shadowSquare[i].y * GRIDSIZE);
			sTiles.setColor(Color(50, 50, 50, 255));
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
			sTiles.setColor(Color(255, 255, 255, 255));
		}

		//活动方块
		for (int i = 0; i < 4; i++) {
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(currentSquare[i].x * GRIDSIZE, currentSquare[i].y * GRIDSIZE);
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
		}
	}
	//不活动方块
	for (int i = 0; i < STAGE_HEIGHT; i++)
		for (int j = 0; j < STAGE_WIDTH; j++) {
			if (Field[i][j] == 0)
				continue;
			sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
		}
	for (int i = 0; i < STAGE_HEIGHT; i++)
		if (i == animationRow[0] || i == animationRow[1] || i == animationRow[2] || i == animationRow[3])
			animationFunc(i);
		else
			for (int j = 0; j < STAGE_WIDTH; j++) {
				if (Field[i][j] == 0)
					continue;
				sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
				sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
				sTiles.move(mCornPoint.x, mCornPoint.y);

				window->draw(sTiles);
			}
	//Next区
	for (int i = 0; i < 4; i++) {
		sTiles.setTextureRect(IntRect(nextcolorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(nextSquare[i].x * GRIDSIZE, nextSquare[i].y * GRIDSIZE);
		sTiles.move(nextSquareCornPoint.x, nextSquareCornPoint.y);
		window->draw(sTiles);
	}

	//Hold区
	if (holdShapeNum > -1)
		for (int i = 0; i < 4; i++) {
			sTiles.setTextureRect(IntRect(holdcolorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(holdSquare[i].x * GRIDSIZE, holdSquare[i].y * GRIDSIZE);
			sTiles.move(holdSquareCornPoint.x, holdSquareCornPoint.y);
			window->draw(sTiles);
		}
}

bool Tetris::hitTest() {
	for (int i = 0; i < 4; i++)
		if (currentSquare[i].x < 0 || currentSquare[i].x >= STAGE_WIDTH || currentSquare[i].y >= STAGE_HEIGHT)
			return false;
		else if (Field[currentSquare[i].y][currentSquare[i].x])
			return false;
	return true;
}

int Tetris::Bag7() {
	int num;
	srand(time(NULL));
	num = rand() % 7;
	for (int i = 0; i < b7Int; i++) {
		if (b7array[i] == num) {
			i = -1;
			num = rand() % 7;
		}
	}
	b7array[b7Int] = num;

	b7Int++;
	if (b7Int == 7) {
		b7Int = 0;
		for (int i = 0; i < 7; i++)
			b7array[i] = 0;
	}
	return num;
}

void Tetris::animationFunc(int i) {
	Vector2f p;
	sTiles.scale(animationCtrlValue, animationCtrlValue);
	p = sTiles.getOrigin();
	sTiles.setOrigin(GRIDSIZE / 2, GRIDSIZE / 2);
	sTiles.rotate(360 * animationCtrlValue);
	for (int j = 0; j < STAGE_WIDTH; j++) {
		sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
		sTiles.move(mCornPoint.x + GRIDSIZE / 2, mCornPoint.y + GRIDSIZE / 2);

		window->draw(sTiles);
	}
	sTiles.scale(1.0 / animationCtrlValue, 1.0 / animationCtrlValue);
	sTiles.rotate(-360 * animationCtrlValue);
	sTiles.setOrigin(p);
}
void Tetris::isWin() {
	//原本应该用碰撞检测来做结束判断，但此处，两方块位置有一不为零，游戏即结束
	//if(!hitTest())
	if (Field[2][5] || Field[2][6])
		gameOver = true;
}