#include "Carets.h"
#include "Draw.h"

CaretTable gCaretTable[] = {
	{0,{0,0,0,0}},
	{SURFACE_ID_STAR,{8,8,8,8}},
	{SURFACE_ID_BUBBLES,{8,8,8,8}},
	{SURFACE_ID_ENEMY1,{8,8,8,8}},
	{SURFACE_ID_ENEMY2,{8,8,8,8}},
	{SURFACE_ID_GAMEINTERFACE,{4,4,4,4}},
	{SURFACE_ID_GAMEINTERFACE,{32,8,32,8}},
};

void CaretNull(Object* caret)
{
}

void CaretStar(Object* caret)
{
	RECT rect[] = {
		{0,0,16,16},
		{16, 0, 32, 16},
		{32, 0, 48, 16},
		{48, 0, 64, 16},
		{64, 0, 80, 16},
		{80, 0, 96, 16},
	};

	caret->x += caret->xm;
	caret->y += caret->ym;

	caret->xm -= THAT_FULLPIXEL / 2;

	if (++caret->ani_wait > 6) {
		caret->ani_wait = 0;
		if (++caret->ani_no > 5)
			caret->id = CARET_NULL;
	}

	caret->rect = rect[caret->ani_no];
}

void CaretBubble(Object* caret)
{
	RECT rect[] = {
		// Big
		{0, 80, 32, 112},
		{32, 80, 64, 112},
		{64, 80, 96, 112},
		{32, 80, 64, 112},
		// Small
		{0, 112, 16, 128},
		{16, 112, 32, 128},
		{32, 112, 48, 128},
		{16, 112, 32, 128},
	};

	caret->x += caret->xm;
	caret->y += caret->ym;

	caret->xm -= THAT_FULLPIXEL / 4;

	if (caret->ym < 0)
		caret->ym += 0x40;
	if (caret->ym > 0)
		caret->ym -= 0x40;

	if (++caret->ani_wait > 6) {
		caret->ani_wait = 0;
		caret->ani_no++;
		caret->ani_no %= 4;
		if (caret->act_no == 1)
			caret->ani_no += 4;
	}

	if (caret->x < map.progress - (320 * THAT_FULLPIXEL))
		caret->id = CARET_NULL;

	caret->rect = rect[caret->ani_no];
}

void CaretFeather(Object* caret)
{
	RECT rect[] = {
		// Big
		{192, 0, 224, 32},
		{224, 0, 256, 32},
		{256, 0, 288, 32},
		{288, 0, 320, 32},
		// Small
		{192, 32, 208, 48},
		{208, 32, 224, 48},
		{224, 32, 240, 48},
		{240, 32, 256, 48},
	};

	caret->x += caret->xm;
	caret->y += caret->ym;

	caret->xm -= THAT_FULLPIXEL / 4;

	if (caret->ym < 0)
		caret->ym += 0x40;
	if (caret->ym > 0)
		caret->ym -= 0x40;

	if (++caret->ani_wait > 6) {
		caret->ani_wait = 0;
		caret->ani_no++;
		caret->ani_no %= 4;
		if (caret->act_no == 1)
			caret->ani_no += 4;
	}

	if (caret->x < map.progress - (320 * THAT_FULLPIXEL))
		caret->id = CARET_NULL;

	caret->rect = rect[caret->ani_no];
}

void CaretScore(Object* caret)
{
	RECT rect = { 0, 16, 8, 24 };

	if (++caret->ani_wait > 66) {
		caret->id = CARET_NULL;
	}

	if (caret->ani_wait > 50)
		caret->ym++;

	rect.left += caret->ani_no * 8;
	rect.right += caret->ani_no * 8;

	//rect.top = 16 - caret->ym;
	rect.bottom = 24 - caret->ym;

	caret->x += THAT_FULLPIXEL;
	caret->y -= THAT_FULLPIXEL;

	caret->rect = rect;
}

void CaretGradeUp(Object* caret)
{
	RECT rect = { 0, 56, 64, 72 };

	if (++caret->ani_wait > 132) {
		caret->id = CARET_NULL;
	}

	if (caret->ani_wait > 116)
		caret->ym++;

	rect.bottom = 72 - caret->ym;

	caret->x += THAT_FULLPIXEL;
	caret->y -= THAT_FULLPIXEL;

	caret->rect = rect;
}

typedef void (*CaretFunc)(Object*);
const CaretFunc gCaretFunc[] = {
	CaretNull,
	CaretStar,
	CaretBubble,
	CaretFeather,
	CaretFeather,
	CaretScore,
	CaretGradeUp,
};

void ActCaret()
{
	for (int i = 0; i < MAXOBJECTS; i++) {
		gCaretFunc[map.carets[i].id](&map.carets[i]);
	}
}

void PutCaret(int fx, int fy)
{
	for (int i = 0; i < MAXOBJECTS; i++) {
		if (map.carets[i].id != CARET_NULL) {
			int id = map.carets[i].id;
			PutBitmap3(&grcFull, ((map.carets[i].x - (gCaretTable[id].view.left * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (fx / THAT_FULLPIXEL),
				((map.carets[i].y - (gCaretTable[id].view.top * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (fy / THAT_FULLPIXEL),
				&map.carets[i].rect, gCaretTable[id].surf);
		}
	}
}

int SpawnCaret(char id, int x, int y, int xm, int ym)
{
	int ind = 0;
	while (ind < MAXOBJECTS && map.carets[ind].id != CARET_NULL)
		ind++;

	if (ind >= MAXOBJECTS)
		return -1;

	memset(&map.carets[ind], 0, sizeof(Object));
	map.carets[ind].id = id;
	map.carets[ind].x = x;
	map.carets[ind].y = y;
	map.carets[ind].xm = xm;
	map.carets[ind].ym = ym;

	return ind;
}