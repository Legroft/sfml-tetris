#include "Game.h"

Game::Game() {
	Window_width = 1350;
	Window_height = 1000;

	imgSetNo = 1;

	//设定窗口属性，窗口禁止缩放
	Uint8 WindowStyle = sf::Style::Close | sf::Style::Titlebar;
	window.create(sf::VideoMode(Window_width, Window_height), L"Tetris by legroft", WindowStyle);
	sf::VideoMode mode = sf::VideoMode::getDesktopMode();//查询桌面的属性
	Vector2i p = { 0,0 };
	p.x = (mode.width - Window_width) / 2;
	window.setPosition(p);
}

Game::~Game() {

}

void Game::gameInitial() {
	window.setFramerateLimit(60);

	LoadMediaData();

	isGameBegin = false;
	isGameHold = false;

	ButtonState_Start = Start_Dark;
	ButtonState_Hold = Hold_Dark;

	player1.role = rolePLAYER1;
	player2.role = rolePLAYER2;
	player1.Initial(&tTiles);
	player2.Initial(&tTiles);
}

void Game::LoadMediaData() {
	stringstream ss;
	ss << "data/images/bg" << imgSetNo << ".jpg";

	if (!tBackground.loadFromFile(ss.str())) {
		cout << "BG image not found" << endl;
	}

	ss.str("");
	ss << "data/images/tiles" << imgSetNo << ".jpg";
	if (!tTiles.loadFromFile(ss.str())) {
		cout << "tiles.jpg not found" << endl;
	}
	if (!tFrame.loadFromFile("data/images/frame.png")) {
		cout << "frame.png not found" << endl;
	}
	if (!tCover.loadFromFile("data/images/cover.png")) {
		cout << "cover.png not found" << endl;
	}
	if (!tGameOver.loadFromFile("data/images/end.png")) {
		cout << "end.png not found" << endl;
	}
	if (!tButtons.loadFromFile("data/images/button.png")) {
		cout << "button.png not found" << endl;
	}
	if (!tSwitcher.loadFromFile("data/images/bgSwitch.png")) {
		cout << "bgSwitch.png not found" << endl;
	}

	sBackground.setTexture(tBackground);
	sFrame.setTexture(tFrame);
	sCover.setTexture(tCover);
	sGameOver.setTexture(tGameOver);
	sButtons.setTexture(tButtons);
	sSwitcher.setTexture(tSwitcher);
	sSwitcher.setOrigin(sSwitcher.getLocalBounds().width / 2.0, sSwitcher.getLocalBounds().height / 2.0);

	if (!font.loadFromFile("data/fonts/方正粗黑宋简体.ttf"))
		cout << "font not found" << endl;
	text.setFont(font);
}

void Game::gameInput() {
	Event event;
	window.setKeyRepeatEnabled(false);
	while (window.pollEvent(event)) {
		if (event.type == Event::Closed) {
			window.close();
			gameQuit = true;
		}
		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::Escape) {
			window.close();
			gameQuit = true;
		}
		if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
			if (ButtonRectStart.contains(event.mouseButton.x, event.mouseButton.y))
				if (isGameBegin == false) {
					isGameBegin = true;
					player1.animationFlag = false;
					player2.animationFlag = false;
					ButtonState_Start = Close_Light;
				} else {
					isGameBegin = false;
					ButtonState_Start = Start_Light;
				}

			if (ButtonRectHold.contains(event.mouseButton.x, event.mouseButton.y))
				if (isGameHold == false) {
					isGameHold = true;
					ButtonState_Hold = Continue_Light;
				} else {
					isGameHold = false;
					ButtonState_Hold = Hold_Light;
				}

			if (ButtonRectLeft.contains(event.mouseButton.x, event.mouseButton.y)) {
				imgSetNo--;
				cout << imgSetNo << endl;
				if (imgSetNo < 1)
					imgSetNo = 4;
				LoadMediaData();
			}
			if (ButtonRectRight.contains(event.mouseButton.x, event.mouseButton.y)) {
				imgSetNo++;
				if (imgSetNo > 4)
					imgSetNo = 1;
				LoadMediaData();
			}
		}

		if (event.type == Event::MouseMoved) {
			if (ButtonRectStart.contains(event.mouseMove.x, event.mouseMove.y))
				if (isGameBegin == false)
					ButtonState_Start = Start_Light;
				else
					ButtonState_Start = Close_Light;
			else
				if (isGameBegin == false)
					ButtonState_Start = Start_Dark;
				else
					ButtonState_Start = Close_Dark;

			if (ButtonRectHold.contains(event.mouseMove.x, event.mouseMove.y))
				if (isGameHold == false)
					ButtonState_Hold = Hold_Light;
				else
					ButtonState_Hold = Continue_Light;
			else
				if (isGameHold == false)
					ButtonState_Hold = Hold_Dark;
				else
					ButtonState_Hold = Continue_Dark;
		}
		player1.Input(event);
		player2.Input(event);
	}
}

void Game::gameLogic() {
	if (isGameHold == true)
		return;

	float time = clock.getElapsedTime().asSeconds();
	clock.restart();
	player1.timer += time;
	player2.timer += time;

	player1.Logic();
	player2.Logic();
}

void Game::gameDraw() {
	window.clear();
	sBackground.setPosition(0, 0);
	window.draw(sBackground);
	window.draw(sFrame);
	player1.Draw(&window);
	player2.Draw(&window);

	sCover.setPosition(P1_STAGE_CORNER_X, P1_STAGE_CORNER_Y);
	window.draw(sCover);
	sCover.setPosition(P2_STAGE_CORNER_X, P2_STAGE_CORNER_Y);
	window.draw(sCover);

	DrawButton();
	TextOut();
	DrawResults();

	window.display();
}

void Game::DrawButton() {
	int ButtonWidth, ButtonHeight;

	ButtonWidth = 110;
	ButtonHeight = sButtons.getLocalBounds().height;

	//ButtonRectStart
	sButtons.setTextureRect(IntRect(ButtonState_Start * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(B_START_CORNER_X, B_START_CORNER_Y);
	ButtonRectStart.left = B_START_CORNER_X;
	ButtonRectStart.top = B_START_CORNER_Y;
	ButtonRectStart.width = ButtonWidth;
	ButtonRectStart.height = ButtonHeight;
	window.draw(sButtons);

	//ButtonRectHold
	sButtons.setTextureRect(IntRect(ButtonState_Hold * ButtonWidth, 0, ButtonWidth, ButtonHeight));
	sButtons.setPosition(B_HOLD_CORNER_X, B_HOLD_CORNER_Y);
	ButtonRectHold.left = B_HOLD_CORNER_X;
	ButtonRectHold.top = B_HOLD_CORNER_Y;
	ButtonRectHold.width = ButtonWidth;
	ButtonRectHold.height = ButtonHeight;
	window.draw(sButtons);

	//BGSwitch
	ButtonWidth = sSwitcher.getLocalBounds().width;
	ButtonHeight = sSwitcher.getLocalBounds().height;

	//ButtonRectLeft
	sSwitcher.setPosition(B_LEFT_CORNER_X, B_LEFT_CORNER_Y);
	ButtonRectLeft.left = B_LEFT_CORNER_X - ButtonWidth / 2;
	ButtonRectLeft.top = B_LEFT_CORNER_Y - ButtonHeight / 2;
	ButtonRectLeft.width = ButtonWidth;
	ButtonRectLeft.height = ButtonHeight;
	window.draw(sSwitcher);

	//ButtonRectRight
	sSwitcher.setPosition(B_RIGHT_CORNER_X, B_RIGHT_CORNER_Y);
	ButtonRectRight.left = B_RIGHT_CORNER_X - ButtonWidth / 2;
	ButtonRectRight.top = B_RIGHT_CORNER_Y - ButtonHeight / 2;
	ButtonRectRight.width = ButtonWidth;
	ButtonRectRight.height = ButtonHeight;
	sSwitcher.rotate(180);
	window.draw(sSwitcher);
	sSwitcher.rotate(180);
}

void Game::gameRun() {
	do {

		gameInitial();

		while (window.isOpen() && gameOver == false) {
			gameInput();

			if (isGameBegin == true)
				gameLogic();
			else
				gameInitial();

			gameDraw();
		}
	}
	while (!gameQuit);
}
void Game::DrawResults() {
	int ButtonWidth, ButtonHeight;
	ButtonWidth = 250;// sGameOver.getLocalBounds().width / 2;
	ButtonHeight = sGameOver.getLocalBounds().height;

	if (player1.gameOver || player2.gameOver) {
		sGameOver.setTextureRect(IntRect(player1.gameOver * ButtonWidth, 0, ButtonWidth, ButtonHeight));//读取按钮的纹理区域
		sGameOver.setPosition(P1_STAGE_CORNER_X + GRIDSIZE * 1.5, 0); //设置按钮的位置坐
		window.draw(sGameOver);

		sGameOver.setTextureRect(IntRect(player2.gameOver * ButtonWidth, 0, ButtonWidth, ButtonHeight));//读取按钮的纹理区域
		sGameOver.setPosition(P2_STAGE_CORNER_X + GRIDSIZE * 1.5, 0); //设置按钮的位置坐

		window.draw(sGameOver);
	}
}
void Game::TextOut() {
	int initialX, initialY;
	int CharacterSize = 48;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 0, 0, 255));//红色字体
	text.setStyle(Text::Bold); // |Text::Underlined
	text.setPosition(P1_SCORE_CORNER_X, P1_SCORE_CORNER_Y);
	std::stringstream ss;
	//player1.score = 9999;
	ss << player1.score;
	text.setString(ss.str()); window.draw(text);

	text.setPosition(P2_SCORE_CORNER_X - CharacterSize * 3, P2_SCORE_CORNER_Y);
	//player2.score = 6666;
	ss.str("");
	ss << player2.score;
	text.setString(ss.str()); window.draw(text);
	CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Bold); // |Text::Underlined
	initialY = INFO_CORNER_Y;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"玩家 1:"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 方向键：WASD"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 速降： Space 键"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 交换： 左 Ctrl 键"); window.draw(text);
	initialY += CharacterSize * 1.5;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"玩家 2:"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 方向键：↑←↓→"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 速降： Enter 键"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 交换： 右 Ctrl 键"); window.draw(text);
	initialY += 2 * CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"自带底部缓冲功能"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"退出： Esc 键"); window.draw(text);
}