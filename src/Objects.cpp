#include "Objects.h"
#include "Doggy.h"
#include "Draw.h"
#include <math.h>

// TODO: move theses in seperate .cpp files
ObjectTable gObjectTable[] = {
	{0,{0,0,0,0},0}, // OBJ_NULL

	{SURFACE_ID_OBSTACLES,{28,28,28,28},{32,32,32,32},isSolid}, // OBJ_ROCK
	{SURFACE_ID_OBSTACLES,{28,28,28,28},{32,32,32,32},isSolid | isHazard}, // OBJ_SPIKE
	{SURFACE_ID_OBSTACLES,{48,48,48,48},{64,64,64,64},isSolid}, // OBJ_BIGROCK

	{SURFACE_ID_ITEMS,{8,8,8,8},{16,16,16,16},isTouchable}, // OBJ_DIAMOND
	{SURFACE_ID_ITEMS,{8,8,8,8},{16,16,16,16},isTouchable}, // OBJ_DIAMONDSUPER

	{SURFACE_ID_ENEMY1,{24,24,24,24},{32,32,32,32},isTouchable | isHazard | isKillable | isFront}, // OBJ_BIRD
	{SURFACE_ID_ENEMY2,{24,24,24,24},{32,32,32,32},isTouchable | isHazard | isKillable | isFront}, // OBJ_BUG
};

void ObjectNull(Object* obj)
{
}
void ObjectPhys(Object* obj)
{
	obj->x += obj->xm;
	obj->y += obj->ym;
}
void ObjectRock(Object* obj)
{
	ObjectPhys(obj);
	obj->rect = { 0, 0, 64, 64 };
}
void ObjectSpike(Object* obj)
{
	ObjectPhys(obj);
	obj->rect = { 64, 0, 128, 64 };
}
void ObjectBigRock(Object* obj)
{
	ObjectPhys(obj);
	obj->rect = { 0, 64, 128, 192 };
}
void ObjectDiamond(Object* obj)
{
	ObjectPhys(obj);
	obj->rect = { 0, 32, 32, 64 };

	if (obj->act_no == 1) {
		obj->xm -= 0x10;
	}
}
void ObjectSuperDiamond(Object* obj)
{
	ObjectPhys(obj);
	obj->rect = { 32, 32, 64, 64 };
}
void ObjectBird(Object* obj)
{
	ObjectPhys(obj);

	RECT rect[] = {
		// Sitting
		{ 0, 0, 64, 64 },
		{64, 0, 128, 64},
		// Flying
		{0, 64, 64, 128},
		{64, 64, 128, 128},
		{128, 64, 192, 128},
		{192, 64, 256, 128},
		{256, 64, 320, 128},
	};

	if (obj->xm > 4 * THAT_FULLPIXEL)
		obj->xm = 4 * THAT_FULLPIXEL;
	if (obj->xm < -4 * THAT_FULLPIXEL)
		obj->xm = -4 * THAT_FULLPIXEL;

	if (obj->ym > 4 * THAT_FULLPIXEL)
		obj->ym = 4 * THAT_FULLPIXEL;
	if (obj->ym < -4 * THAT_FULLPIXEL)
		obj->ym = -4 * THAT_FULLPIXEL;


	if (obj->act_no == 0) {
		// Sit
		if (++obj->ani_wait > 2) {
			obj->ani_wait = 0;
			obj->ani_no++;
			obj->ani_no %= 2;
		}

		if (obj->x - (94 * THAT_FULLPIXEL) < doggy.x && obj->x + (94 * THAT_FULLPIXEL) > doggy.x && obj->y - (94 * THAT_FULLPIXEL) < doggy.y && obj->y + (94 * THAT_FULLPIXEL) > doggy.y) {
			if(obj->y < doggy.y)
				obj->act_no = 1;
			else
				obj->act_no = 2;
			
			obj->xm = 2 * THAT_FULLPIXEL;
		}

	}else {
		// Flying
		if (++obj->ani_wait > 3) {
			obj->ani_wait = 0;
			
			if (obj->ani_no >= 0 && obj->ani_no < 2)
				obj->ani_no = 2;

			if (obj->ani_no == 6)
				obj->ani_no = 2;
			if (obj->ani_no >= 2 && obj->ani_no < 6)
				obj->ani_no++;
		}

		if (obj->act_no == 1) {
			obj->ym -= 0x20;
			obj->xm -= 0x20;
		}
		if (obj->act_no == 2) {
			obj->ym += 0x20;
			obj->xm -= 0x20;
		}
		if (obj->act_no == 3) {

			if (obj->ym < 0)
				obj->ym += 0x10;
			if (obj->ym > 0)
				obj->ym -= 0x10;

			obj->xm -= 0x20;
		}
	}
	
	if (obj->x < map.progress - (320 * THAT_FULLPIXEL))
		obj->id = OBJ_NULL;

	obj->rect = rect[obj->ani_no];
}

void ObjectBug(Object* obj)
{
	RECT rect[] = {
		// Flying
		{0, 64, 64, 128},
		{64, 64, 128, 128},
		{128, 64, 192, 128},
		{192, 64, 256, 128},
	};

	//if (obj->xm > 4 * THAT_FULLPIXEL)
	//	obj->xm = 4 * THAT_FULLPIXEL;
	//if (obj->xm < -4 * THAT_FULLPIXEL)
	//	obj->xm = -4 * THAT_FULLPIXEL;
	//
	//if (obj->ym > 4 * THAT_FULLPIXEL)
	//	obj->ym = 4 * THAT_FULLPIXEL;
	//if (obj->ym < -4 * THAT_FULLPIXEL)
	//	obj->ym = -4 * THAT_FULLPIXEL;

	if (++obj->ani_wait > 3) {
		obj->ani_wait = 0;
		obj->ani_no++;
		obj->ani_no %= 4;
	}


	if (obj->act_no == 1) {
		obj->angle++;
		int tempx = ((320 * cos(((float)obj->angle / 96) - (3.14/2)))) * THAT_FULLPIXEL;
		int tempy = ((320 * sin(((float)obj->angle / 96) - (3.14/2)))) * THAT_FULLPIXEL;
		obj->x += ((map.progress + (32 * THAT_FULLPIXEL) + ((SURFACE_WIDTH / 2) * THAT_FULLPIXEL) + tempx) - obj->x) / 64;
		obj->y += ((((map.height / 2) * THAT_FULLPIXEL) + tempy) - obj->y) / 64;
	}

	if (obj->act_no == 2) {
		obj->angle--;
		int tempx = ((320 * cos(((float)obj->angle / 96) + (3.14 / 2)))) * THAT_FULLPIXEL;
		int tempy = ((320 * sin(((float)obj->angle / 96) + (3.14 / 2)))) * THAT_FULLPIXEL;
		obj->x += ((map.progress + (32 * THAT_FULLPIXEL) + ((SURFACE_WIDTH / 2) * THAT_FULLPIXEL) + tempx) - obj->x) / 64;
		obj->y += ((((map.height / 2) * THAT_FULLPIXEL) + tempy) - obj->y) / 64;
	}

	//ObjectPhys(obj);


	obj->rect = rect[obj->ani_no];
}

typedef void (*ObjectFunc)(Object*);
const ObjectFunc gObjectFunc[] = {
	ObjectNull,

	ObjectRock,
	ObjectSpike,
	ObjectBigRock,

	ObjectDiamond,
	ObjectSuperDiamond,

	ObjectBird,
	ObjectBug,
};

void ActObjects()
{
	for (int i = 0; i < MAXOBJECTS; i++) {
		gObjectFunc[map.objects[i].id](&map.objects[i]);
	}
}

void PutObjects(int fx, int fy)
{
	// Regular Objects
	for (int i = 0; i < MAXOBJECTS; i++) {
		if (map.objects[i].id != OBJ_NULL && gObjectTable[map.objects[i].id].flags & ~isFront) {
			int id = map.objects[i].id;
			PutBitmap3(&grcFull, ((map.objects[i].x - (gObjectTable[id].view.left * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (fx / THAT_FULLPIXEL),
				((map.objects[i].y - (gObjectTable[id].view.top * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (fy / THAT_FULLPIXEL),
				&map.objects[i].rect, gObjectTable[id].surf);
		}
	}
	// Front
	for (int i = 0; i < MAXOBJECTS; i++) {
		if (map.objects[i].id != OBJ_NULL && gObjectTable[map.objects[i].id].flags & isFront) {
			int id = map.objects[i].id;
			PutBitmap3(&grcFull, ((map.objects[i].x - (gObjectTable[id].view.left * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (fx / THAT_FULLPIXEL),
				((map.objects[i].y - (gObjectTable[id].view.top * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (fy / THAT_FULLPIXEL),
				&map.objects[i].rect, gObjectTable[id].surf);
		}
	}
}

int MapSpawn(char id, int x, int y)
{
	int ind = 0;
	while (ind < MAXOBJECTS && map.objects[ind].id != OBJ_NULL)
		ind++;

	if (ind >= MAXOBJECTS)
		return -1;

	memset(&map.objects[ind], 0, sizeof(Object));
	map.objects[ind].id = id;
	map.objects[ind].x = x;
	map.objects[ind].y = y;
	return ind;
}
