#pragma once
#include <windows.h>

#define THAT_FULLPIXEL 0x400
#define MAXOBJECTS 256

enum GAMEMODE
{
	GAMEMODE_TITLE,
	GAMEMODE_LOAD,
	GAMEMODE_GAMEPLAY,
	GAMEMODE_CLEAR,
	GAMEMODE_GAMEOVER,
	GAMEMODE_END,
};

struct Object {
	int x, y;
	int xm, ym;
	int id;
	int act_no;
	int ani_no;
	int ani_wait;
	int angle;
	int tag;
	RECT rect;
};

struct Map {
	int height;
	int progress;
	int nexttrigger;
	int stage;
	Object objects[MAXOBJECTS];
	Object carets[MAXOBJECTS];
};

struct Frame {
	int x, y;
};

struct Fade
{
	char mode;
	int wait;
};
enum FadeMode
{
	FADEMODE_NONE,
	FADEMODE_FADEOUT,
	FADEMODE_FADEIN,
};
void SetFade(char mode);
bool ActFade();

BOOL Game(HWND hWnd);

extern DWORD gKeyTrg;
extern Map map;
extern int gDelayGame;
extern char mode;

int Random(int min, int max);

void ReloadStage(bool OnHurt);

