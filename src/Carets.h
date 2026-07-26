#pragma once
#include <windows.h>
#include "Game.h"

enum CaretType {
	CARET_NULL = 0,
	CARET_STAR,
	CARET_BUBBLE,
	CARET_FEATHER,
	CARET_FEATHER_ALT,
	CARET_SCORE,
	CARET_GRADEUP,
};

struct CaretTable {
	char surf;
	RECT view;
};

int SpawnCaret(char id, int x, int y, int xm, int ym);

void ActCaret();
void PutCaret(int fx, int fy);