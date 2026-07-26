#pragma once
#include <windows.h>
#include "Game.h"

enum ObjectType {
	OBJ_NULL = 0,
	// Obstacles
	OBJ_ROCK,
	OBJ_SPIKE,
	OBJ_BIGROCK,
	// Diamonds
	OBJ_DIAMOND,
	OBJ_DIAMONDSUPER,
	// Npcs
	OBJ_BIRD,
	OBJ_BUG,
};

enum ObjectFlags {
	isSolid = 1 << 0,
	isTouchable = 1 << 1,
	isKillable = 1 << 2,
	isHazard = 1 << 3,
	isFront = 1 << 4,
};

struct ObjectTable {
	char surf;
	RECT hit;
	RECT view;
	char flags;
};

extern ObjectTable gObjectTable[];

void ActObjects();
void PutObjects(int fx, int fy);
int MapSpawn(char id, int x, int y);