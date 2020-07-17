#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <iostream>
#include <sstream>

#define GRIDSIZE 35
#define DELAYVALUE 0.3
#define STAGE_WIDTH 10
#define STAGE_HEIGHT 20
#define P1_STAGE_CORNER_X 156
#define P1_STAGE_CORNER_Y 174
#define P2_STAGE_CORNER_X 844
#define P2_STAGE_CORNER_Y 174
#define P1_NEXT_CORNER_X 587
#define P1_NEXT_CORNER_Y 125
#define P2_NEXT_CORNER_X 702
#define P2_NEXT_CORNER_Y 125
#define HOLD_CORNER_X 660
#define HOLD_CORNER_Y 275

using namespace sf;
using namespace std;

typedef enum PLAYROLE {
	roleNONE,
	rolePLAYER1,
	rolePLAYER2,
};

class Tetris {
public:
	Tetris();
	~Tetris();
	Vector2i mCornPoint, nextSquareCornPoint, holdSquareCornPoint;

	int gridSize;
	int imgBGNo, imgSkinNo;
	int role;
	int colorNum, nextcolorNum, tempcolorNum;
	int currentShapeNum, nextShapeNum, tempShapeNum;
	int b7array[7] = {}, b7Int;
	int dx;
	int score;
	int animationRow[4];
	static int holdcolorNum, holdShapeNum;
	static Vector2i holdSquare[4];
	bool rotate, hold, hardDrop, newShapeFlag, animationFlag, gameOver;
	float timer, delay, animationCtrlValue;
	Texture* tTiles;
	Texture tBackground, tButtons, tNum, tTimer, tCounter, tGameOver;
	Sprite sBackground, sTiles, sButtons, sNum, sTimer, sCounter, sGameOver;

	RenderWindow* window;

	int Field[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };
	Vector2i currentSquare[4], nextSquare[4], tempSquare[4], shadowSquare[4];
	int Figures[7][4] =
	{
		3,5,1,7,//I
		4,5,2,7,//S
		4,5,3,6,//Z
		5,3,4,7,//T
		5,3,2,7,//L
		5,7,3,6,//J
		2,3,4,5,//O
	};

	typedef enum gridShape {
		shapeI,//I
		shapeS,//S
		shapeZ,//Z
		shapeT,//T
		shapeL,//L
		shapeJ,//J
		shapeO,//O
	};


	void Initial(Texture* tex);
	void Input(Event event);
	void traditionLogic();
	void Logic();
	void xMove();
	void yMove();
	void rotateFunc();
	void holdFunc();
	void checkLine();
	void clearLine();
	void slowLoading();
	void newShapeFunc();
	void animationFunc(int i);
	void hardDropFunc();
	void shadow();

	void Draw(RenderWindow* window);
	bool hitTest();
	int Bag7();


	void isWin();
};

