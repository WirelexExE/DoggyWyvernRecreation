#include "Game.h"
#include "Generic.h"
#include "Draw.h"
#include "Doggy.h"
#include "Carets.h"
#include "Objects.h"
#include "Sound.h"
#include <math.h>
#include <stdio.h>

DWORD gKeyTrg;

char mode = GAMEMODE_TITLE;
Frame frame;
Map map;

int Random(int min, int max)
{
	const int range = max - min + 1;
	return (rand() % range) + min;
}

void GetTrg()
{
	static DWORD key_prev;

	//Get pressed key
	gKeyTrg = (key_prev ^ gKey) & gKey;
	key_prev = gKey;
}

DWORD CountFramePerSecound()
{
	unsigned long current_tick;
	static BOOL first = TRUE;
	static unsigned long max_count;
	static unsigned long count;
	static unsigned long wait;

	if (first)
	{
		wait = GetTickCount();
		first = FALSE;
	}

	current_tick = GetTickCount();
	++count;

	if (wait + 1000 <= current_tick)
	{
		wait += 1000;
		max_count = count;
		count = 0;
	}

	return max_count;
}

void GameLoading()
{
	CortBox(&grcFull, 0x000000);
	RECT rcLoading = { 0, 32, 240, 64 };
	PutBitmap3(&grcFull, 200, 200, &rcLoading, SURFACE_ID_GAMESTATUS);
}

void GameTitle()
{
	CortBox(&grcFull, 0x040404);
	RECT rcTitle = { 0, 0, 525, 120};
	PutBitmap3(&grcFull, 58, 120, &rcTitle, SURFACE_ID_TITLE);

	if (gKeyTrg & KEY_SPACE | gKeyTrg & KEY_Z) {
		GameLoading();
		PlaySoundObject(SOUND_ID_MUS_TITLE, SOUND_MODE_STOP);
		mode = GAMEMODE_LOAD;
	}
		
}

struct stagetxt {
	int wait;
	int pos;
	char mode;
};
stagetxt stagetxts[14];
void StageTxtReset()
{
	for (int i = 0; i < 14; i++) {
		stagetxts[i].wait = i * -8;
		stagetxts[i].mode = 0;
	}
}

void GameLoad()
{
	GameLoading();

	//TEMP
	if (map.stage == 3)
		map.stage = 1;

	switch (map.stage)
	{
	case 1:
		// Stage 1
		ReloadSurface_File("Back1_1.bmp", SURFACE_ID_BACK0);
		ReloadSurface_File("Back1_2.bmp", SURFACE_ID_BACK1);
		ReloadSurface_File("Back1_3.bmp", SURFACE_ID_BACK2);
		ReloadSurface_File("Back1_4.bmp", SURFACE_ID_BACK3);
		ReloadSurface_File("Back1_5.bmp", SURFACE_ID_BACK4);
		ReloadSurface_File("Back1_6.bmp", SURFACE_ID_BACK5);
		ReloadSurface_File("Back1_7.bmp", SURFACE_ID_BACK6);
		ReloadSurface_File("Back1_8.bmp", SURFACE_ID_BACK7);
		ReloadSurface_File("Back1_9.bmp", SURFACE_ID_BACK8);
		ReloadSurface_File("Back1_10.bmp", SURFACE_ID_BACK9);
		ReloadSurface_File("Obstacles.bmp", SURFACE_ID_OBSTACLES);
		ReloadSurface_File("Enemy2.bmp", SURFACE_ID_ENEMY2);
		LoadSoundObject("MusStage1A.wav", SOUND_ID_MUS_STAGEA);
		LoadSoundObject("MusStage1B.wav", SOUND_ID_MUS_STAGEB);
		break;
	case 2:
		// Stage 2
		ReloadSurface_File("NBack1_1.bmp", SURFACE_ID_BACK0);
		ReloadSurface_File("NBack1_2.bmp", SURFACE_ID_BACK1);
		ReloadSurface_File("NBack1_3.bmp", SURFACE_ID_BACK2);
		ReloadSurface_File("NBack1_4.bmp", SURFACE_ID_BACK3);
		ReloadSurface_File("NBack1_5.bmp", SURFACE_ID_BACK4);
		ReloadSurface_File("NBack1_6.bmp", SURFACE_ID_BACK5);
		ReloadSurface_File("NBack1_7.bmp", SURFACE_ID_BACK6);
		ReloadSurface_File("NBack1_8.bmp", SURFACE_ID_BACK7);
		ReloadSurface_File("NBack1_9.bmp", SURFACE_ID_BACK8);
		ReloadSurface_File("NBack1_10.bmp", SURFACE_ID_BACK9);
		ReloadSurface_File("NObstacles.bmp", SURFACE_ID_OBSTACLES);
		ReloadSurface_File("NEnemy2.bmp", SURFACE_ID_ENEMY2);
		LoadSoundObject("MusStage2A.wav", SOUND_ID_MUS_STAGEA);
		LoadSoundObject("MusStage2B.wav", SOUND_ID_MUS_STAGEB);
		break;
	}
	StageTxtReset();
	PlaySoundObject(SOUND_ID_MUS_STAGEA, SOUND_MODE_PLAY);
	mode = GAMEMODE_GAMEPLAY;
}

void GameClear()
{
	CortBox(&grcFull, 0x000000);
	RECT rcClear = { 0, 96, 96, 128 };
	PutBitmap3(&grcFull, 270, 200, &rcClear, SURFACE_ID_GAMESTATUS);

	// Music Fade
	static int vol = 300;
	if (vol = vol - 1, vol < 0) {
		vol = 0;
		if (IsSoundObjectPlaying(SOUND_ID_MUS_STAGEB)) {
			ChangeSoundVolume(SOUND_ID_MUS_STAGEB, 300);
			PlaySoundObject(SOUND_ID_MUS_STAGEB, SOUND_MODE_STOP);
		}
	}
	else {
		ChangeSoundVolume(SOUND_ID_MUS_STAGEB, vol);
	}

	//452 frames
	static int wait = 452;
	if (--wait < 0) {
		map.stage++;
		ReloadStage(false);
		RespawnDoggy();
		SetFade(FADEMODE_FADEOUT);
		wait = 452;
		vol = 300;
		mode = GAMEMODE_LOAD;
		
	}
}

void GameGameOver()
{
	CortBox(&grcFull, 0x000000);
	RECT rcLoading = { 0, 64, 240, 96 };
	PutBitmap3(&grcFull, 200, 200, &rcLoading, SURFACE_ID_GAMESTATUS);
}

void PutHud()
{
	// Draw Hud
	RECT rcHud[] = {
		{ 0, 24, 16, 40}, // Diamond
		{ 0, 40, 16, 56}, // Feathers
		{16, 24, 64, 40}, // Score
	};

	PutBitmap3(&grcFull, 24, 8, &rcHud[0], SURFACE_ID_GAMEINTERFACE);
	PutBitmap3(&grcFull, 320, 8, &rcHud[1], SURFACE_ID_GAMEINTERFACE);
	PutBitmap3(&grcFull, 8, 24, &rcHud[2], SURFACE_ID_GAMEINTERFACE);

	PutNumber(56, 8, doggy.diamonds); // Diamonds
	PutNumber(56, 24, doggy.score); // Score

	// Fish
	RECT rcFish[] = {
		{80,  0,112, 32}, // Fish (Empty)
		{112, 0,144, 32}, // Fish 
		{144, 0,176, 32}, // Fish (Use)
	};
	for (int i = 0; i < 5; i++)
	{
		int ind = 0;
		if (i < doggy.maxfish) {
			ind = 1;
			if (doggy.holdingfish) {
				if(i == 0)
					ind = 2;
				if (doggy.fishpower / THAT_FULLPIXEL > i)
					ind = 2;
			}
		}
		PutBitmap3(&grcFull, 128 + (32 * i), 8, &rcFish[ind], SURFACE_ID_GAMEINTERFACE);
	}

	RECT rcFlightMeter = { 16,40,144,56 };
	rcFlightMeter.right -= 128 - (doggy.flightbar / THAT_FULLPIXEL);
	PutBitmap3(&grcFull, 344, 10, &rcFlightMeter, SURFACE_ID_GAMEINTERFACE);
}

void PutBackground(int fx, int fy)
{
	// Draw Background
	if(map.stage == 1)
		CortBox(&grcFull, 0x4872A9);
	else
		CortBox(&grcFull, 0x080B42);

	//RECT rcBack[] = {
	//	{ 0, 0, 640, 140},
	//};
	//PutBitmap3(&grcFull, 0, 198 - ((fy / 32) / THAT_FULLPIXEL), &rcBack[0], SURFACE_ID_BACK0);

	RECT rcLayers[] = {
		{ 0, 0, 640, 140},
		{ 0, 0, 640, 32},
		{ 0, 0, 640, 48},
		{ 0, 0, 640, 48},
		{ 0, 0, 640, 48},
		{ 0, 0, 640, 64},
		{ 0, 0, 640, 80},
		{ 0, 0, 640, 80},
		{ 0, 0, 640, 96},
		{ 0, 0, 640, 160},
	};

	int layerofs[]{
		200 - 24,
		308 - 24,
		308 - 16,
		308 - 12,
		308 - 10,
		308 - 8,
		308 - 6,
		308 - 4,
		308 + 2,
		308,
	};

	int layerspeed[]{
		1,
		THAT_FULLPIXEL / 50, // behind
		THAT_FULLPIXEL / 25, // behind (26)
		THAT_FULLPIXEL / 10, // behind
		THAT_FULLPIXEL / 4, // behind

		THAT_FULLPIXEL, // ahead
		2 * THAT_FULLPIXEL, // 2 to 3 frames ahead
		4 * THAT_FULLPIXEL,
		7 * THAT_FULLPIXEL,
		10 * THAT_FULLPIXEL,
	};

	//PutBitmap3(&grcFull, 0, 636 - (fy / THAT_FULLPIXEL), &rcLayers[0], SURFACE_ID_DEBUGFLOOR); //642

	static int scrollx[] = {
		0,0,0,0,0,0,0,0,0,0
	};

	for (int i = 0; i < 10; i++) {
		scrollx[i] = ((map.progress / THAT_FULLPIXEL) * layerspeed[i]) / THAT_FULLPIXEL;
		if (i == 0)
			scrollx[i] = 0;
		scrollx[i] %= 640;
		PutBitmap3(&grcFull, 640 - scrollx[i], layerofs[i] - ((((-layerofs[i] * THAT_FULLPIXEL) + fy) / THAT_FULLPIXEL) / (10 - i)), &rcLayers[i], SURFACE_ID_BACK0 + i);
		rcLayers[i].left = scrollx[i];
		PutBitmap3(&grcFull, 0, layerofs[i] - ((((-layerofs[i] * THAT_FULLPIXEL) + fy) / THAT_FULLPIXEL) / (10 - i)), &rcLayers[i], SURFACE_ID_BACK0 + i);
	}
}

void PutForeground(int fx, int fy)
{
	RECT rcLayer = { 0, 0, 640, 160 };

	int layerofs = 308;

	int scrollx = (map.progress / THAT_FULLPIXEL) * 10;
	scrollx %= 640;
	PutBitmap3(&grcFull, 640 - scrollx, layerofs - ((((-layerofs * THAT_FULLPIXEL) + fy) / THAT_FULLPIXEL)), &rcLayer, SURFACE_ID_BACK9);
	rcLayer.left = scrollx;
	PutBitmap3(&grcFull, 0, layerofs - ((((-layerofs * THAT_FULLPIXEL) + fy) / THAT_FULLPIXEL)), &rcLayer, SURFACE_ID_BACK9);
	
}

void MoveFrame(Frame* frame, Map* mp)
{
	int tx, ty;
	tx = doggy.x;
	ty = doggy.y;

	//(SURFACE_HEIGHT << 9) -> ((SURFACE_HEIGHT / 2) * THAT_FULLPIXEL)

	if ((frame->y + ((SURFACE_HEIGHT / 2) * THAT_FULLPIXEL) - (8 * THAT_FULLPIXEL)) < ty)
		frame->y += (ty - (frame->y + ((SURFACE_HEIGHT / 2) * THAT_FULLPIXEL) - (8 * THAT_FULLPIXEL))) / 48;
	if ((frame->y + ((SURFACE_HEIGHT / 2) * THAT_FULLPIXEL) - (8 * THAT_FULLPIXEL)) > ty)
		frame->y += (ty - (frame->y + ((SURFACE_HEIGHT / 2) * THAT_FULLPIXEL) - (8 * THAT_FULLPIXEL))) / 48;

	if (frame->y < 0)
		frame->y = 0;
	if (frame->y > (mp->height - SURFACE_HEIGHT) * THAT_FULLPIXEL)
		frame->y = (mp->height - SURFACE_HEIGHT) * THAT_FULLPIXEL;

	frame->x = map.progress;
}

int gDelayGame = 0;
Fade fade;

void InitFade()
{
	fade.mode = 0;
	fade.wait = 0;
}

void SetFade(char mode)
{
	fade.mode = mode;
}

bool ActFade()
{
	switch (fade.mode)
	{
	case FADEMODE_NONE:
		fade.wait = 0;
		break;
	case FADEMODE_FADEOUT:
		//these should last 80 frames
		if (fade.wait < SURFACE_WIDTH) {
			fade.wait += 8;
		} else {
			fade.mode = FADEMODE_NONE;
			fade.wait = 0;
			return true;
		}
		break;
	case FADEMODE_FADEIN:
		if (fade.wait < SURFACE_WIDTH) {
			fade.wait += 8;
		}
		else {
			fade.mode = FADEMODE_NONE;
			fade.wait = 0;
			return true;
		}
		break;
	}
	return false;
}

void PutFade()
{
	RECT rcFade = { 0, 0, SURFACE_WIDTH, SURFACE_HEIGHT };
	switch (fade.mode)
	{
	case FADEMODE_FADEOUT:
		rcFade.left = fade.wait;
		CortBox(&rcFade, 0x000000);
		break;
	case FADEMODE_FADEIN:
		rcFade.right = fade.wait;
		if (fade.wait > SURFACE_WIDTH)
			rcFade = { 0, 0, SURFACE_WIDTH, SURFACE_HEIGHT };
		CortBox(&rcFade, 0x000000);
		break;
	}
}


// TODO: clean this up
int LifeCountFade = 0;
void PutPreStatus()
{
	if (++LifeCountFade < 300) {
		RECT rcGlyph = { 0, 0, 32, 32 };
		PutBitmap3(&grcFull, 268, 100, &rcGlyph, SURFACE_ID_GAMESTATUS);
		rcGlyph = { 192, 0, 208, 32 };
		PutBitmap3(&grcFull, 316, 100, &rcGlyph, SURFACE_ID_GAMESTATUS);
		//rcGlyph = { 32, 0, 48, 32 };
		rcGlyph = { 0, 0, 16, 32 };
		// This going negative should cause the doggy sprite to show up (real feature)
		rcGlyph.left += 16 * (doggy.life + 2);
		rcGlyph.right += 16 * (doggy.life + 2);
		PutBitmap3(&grcFull, 344, 100, &rcGlyph, SURFACE_ID_GAMESTATUS);
	}

	// TODO: animate this
	{
		RECT rcGlyph = { 0, 0, 16, 32 };
		rcGlyph.left += 16 * (map.stage + 2);
		rcGlyph.right += 16 * (map.stage + 2);

		int stagetxtwait[] = {0,44,158,26};
		RECT rcCurStage = { 0, 128, 16, 160 };
		for (int i = 0; i < 14; i++) {
			if (++stagetxts[i].wait <= stagetxtwait[stagetxts[i].mode]) {
				float tempy = 0;
				switch (stagetxts[i].mode)
				{
				case 0:
					stagetxts[i].pos = -32;
					break;
				case 1:
					//tempy = sin((((float)stagetxts[i].wait / stagetxtwait[stagetxts[i].mode]) * 3.14) / 2);
					tempy = 1 - pow(1 - ((float)stagetxts[i].wait / stagetxtwait[stagetxts[i].mode]), 3);
					stagetxts[i].pos = SURFACE_HEIGHT - (tempy * (SURFACE_HEIGHT - 152));
					break;
				case 2:
					stagetxts[i].pos = 152;
					break;
				case 3:
					//tempy = 1 - cos((((float)stagetxts[i].wait / stagetxtwait[stagetxts[i].mode]) * 3.14) / 2);
					tempy = ((float)stagetxts[i].wait / stagetxtwait[stagetxts[i].mode])
						* ((float)stagetxts[i].wait / stagetxtwait[stagetxts[i].mode])
						* ((float)stagetxts[i].wait / stagetxtwait[stagetxts[i].mode]);
					stagetxts[i].pos = 152 - (tempy * 152);
					break;
				}
			}
			else {
				if (stagetxts[i].mode != 4) {
					stagetxts[i].mode++;
					stagetxts[i].wait = 0;
				}
			}

			if (stagetxts[i].mode != 0 && stagetxts[i].mode != 4) {
				if (i == 6)
					PutBitmap3(&grcFull, 208 + (i * 16), stagetxts[i].pos, &rcGlyph, SURFACE_ID_GAMESTATUS);
				else
					PutBitmap3(&grcFull, 208 + (i * 16), stagetxts[i].pos, &rcCurStage, SURFACE_ID_GAMESTATUS);
			}

			rcCurStage.left += 16;
			rcCurStage.right += 16;
		}
	}
}

struct StageTriggerData
{
	int x;
	int event;
};
enum StageTriggerEvents
{
	TE_NULL,
	TE_BIRD_A,
	TE_BIRD_SPAM,
	TE_BUG_A,
	TE_BUG_B,
	TE_BUG_STRAIGHT,
	TE_MOVE_OBJECTS_A,
	TE_MOVE2_OBJECTS_A,
	TE_BOSS,
	TE_END,
	TE_LOOP,
};
// Stage 1
//StageTriggerData stagetriggerdat[] = {
//	{1597,TE_BIRD_A},
//	{1637,TE_BIRD_A},
//	{1637 + 40,TE_BIRD_A},
//	{1637 + (40 * 2),TE_BIRD_A},
//	{1637 + (40 * 3),TE_BIRD_A},
//	{1637 + (40 * 4),TE_BIRD_A},
//	{1637 + (40 * 5),TE_BIRD_A},
//	{1637 + (40 * 6),TE_BIRD_A},
//	{1637 + (40 * 7),TE_BIRD_A},
//	{6302,TE_MOVE_OBJECTS_A},
//	{6526,TE_BUG_A},
//	{6526 + 40,TE_BUG_A},
//	{6526 + (40 * 2),TE_BUG_A},
//	{6526 + (40 * 3),TE_BUG_A},
//	{6526 + (40 * 4),TE_BUG_A},
//	{6526 + (40 * 5),TE_BUG_A},
//	{6526 + (40 * 6),TE_BUG_A},
//	{6526 + (40 * 7),TE_BUG_A},
//	{8495,TE_BUG_B},
//	{8495 + 40,TE_BUG_B},
//	{8495 + (40 * 2),TE_BUG_B},
//	{8495 + (40 * 3),TE_BUG_B},
//	{8495 + (40 * 4),TE_BUG_B},
//	{8495 + (40 * 5),TE_BUG_B},
//	{8495 + (40 * 6),TE_BUG_B},
//	{8495 + (40 * 7),TE_BUG_B},
//	{10013,TE_BUG_STRAIGHT},
//	{10225,TE_BIRD_SPAM},
//	{11989,TE_END},
//};

//Stage 2
StageTriggerData stagetriggerdat[] = {
	{1411,TE_MOVE2_OBJECTS_A},
	{9655,TE_LOOP},
};

void PushTaggedObjects(int tag, int xm, int ym)
{
	for (int i = 0; i < MAXOBJECTS; i++) {
		if (map.objects[i].id != OBJ_NULL && map.objects[i].tag == tag) {
			map.objects[i].xm = xm;
			map.objects[i].ym = ym;
		}
	}
}

void GameLoop()
{
	// Loop Track
	if (!IsSoundObjectPlaying(SOUND_ID_MUS_STAGEA) && !IsSoundObjectPlaying(SOUND_ID_MUS_STAGEB)) {
		PlaySoundObject(SOUND_ID_MUS_STAGEB, SOUND_MODE_LOOP);
	}

	//CortBox(&grcFull, 0x800000);
	if (--gDelayGame < 0) 
	{
		map.progress += 1 * THAT_FULLPIXEL;
		MoveFrame(&frame, &map);
		ActObjects();
		ActDoggy();
		HitDoggy();
		ActFish();
		ActCaret();

		// Trigger content
		if (map.nexttrigger < (sizeof(stagetriggerdat) / sizeof(stagetriggerdat[0]))) {
			if ((map.progress / THAT_FULLPIXEL) > stagetriggerdat[map.nexttrigger].x) {
				int spawner = 0;
				switch (stagetriggerdat[map.nexttrigger].event) {
					case TE_BIRD_A:
						spawner = MapSpawn(OBJ_BIRD, map.progress + ((SURFACE_WIDTH / 2) * THAT_FULLPIXEL) , 0);
						map.objects[spawner].act_no = 3;
						map.objects[spawner].xm = Random(4, 8) * THAT_FULLPIXEL;
						map.objects[spawner].ym = Random(4, 5) * THAT_FULLPIXEL;
					break;
					case TE_BUG_A:
						spawner = MapSpawn(OBJ_BUG, map.progress, 116 * THAT_FULLPIXEL);
						map.objects[spawner].act_no = 1;
					break;
					case TE_BUG_B:
						spawner = MapSpawn(OBJ_BUG, map.progress, 524 * THAT_FULLPIXEL);
						map.objects[spawner].act_no = 2;
						break;
					case TE_MOVE_OBJECTS_A:
						PushTaggedObjects(1, THAT_FULLPIXEL / 6, THAT_FULLPIXEL / 16);
						break;
					case TE_MOVE2_OBJECTS_A:
						PushTaggedObjects(1, THAT_FULLPIXEL / 4, -(THAT_FULLPIXEL / 4));
						PushTaggedObjects(2, THAT_FULLPIXEL / 8, THAT_FULLPIXEL / 8);
						break;
					case TE_END:
						doggy.win = true;
						break;
				}

				map.nexttrigger++;
			}
		}

	}

	PutBackground(frame.x, frame.y);
	PutObjects(frame.x, frame.y);
	PutDoggy(&frame);
	PutFish(&frame);
	PutCaret(frame.x, frame.y);
	
	PutForeground(frame.x, frame.y);

	PutHud();
	if (fade.mode == FADEMODE_FADEOUT)
		ActFade();
	PutFade();

	PutPreStatus();
	
	// TODO: scroll screen around frame 160
	// frame 480 spawns player

}

struct StageData
{
	int object; //ObjectType
	int x, y;
	int tag;
};

// TODO: make this a binary file
// Stage 1
/*
StageData stagedat[] = {
	{OBJ_DIAMOND,657,481,0},
	{OBJ_DIAMONDSUPER,677,441,0},
	{OBJ_DIAMOND,697,471,0},
	{OBJ_SPIKE,896,639,0},
	{OBJ_ROCK,957,590,0},
	{OBJ_ROCK,1017,570,0},
	{OBJ_ROCK,1077,580,0},
	{OBJ_SPIKE,1197,310,0},
	{OBJ_SPIKE,1258,559,0},
	{OBJ_BIGROCK,1335,501,0},
	{OBJ_ROCK,1539,331,0}, {OBJ_BIRD,1539 - 16,331 - 32,0},
	{OBJ_ROCK,1589,331,0},
	{OBJ_ROCK,1639,520,0}, {OBJ_BIRD,1639 - 16,520 - 32,0},
	{OBJ_ROCK,1688,520,0},
	{OBJ_ROCK,1690,161,0},
	{OBJ_ROCK,1739,161,0}, {OBJ_BIRD,1739 - 16,161 - 32,0},
	{OBJ_DIAMOND,1740,356,0},
	{OBJ_DIAMOND,1798,356,0},
	{OBJ_DIAMOND,1856,356,0},
	{OBJ_ROCK,1856,530,0}, {OBJ_BIRD,1856 - 16,530 - 32,0},
	{OBJ_ROCK,1956,192,0}, {OBJ_BIRD,1956 - 16,192 - 32,0},
	{OBJ_ROCK,2005,192,0},
	{OBJ_ROCK,2006,381,0}, {OBJ_BIRD,2006 - 16,381 - 32,0},
	{OBJ_ROCK,2056,381,0},
	{OBJ_SPIKE,3037,401,0},
	{OBJ_DIAMOND,3037,493,0},
	{OBJ_ROCK,3089,403,0},
	{OBJ_DIAMOND,3089,522,0},
	{OBJ_ROCK,3141,402,0},
	{OBJ_DIAMOND,3140,542,0},
	{OBJ_ROCK,3191,402,0},
	{OBJ_DIAMOND,3190,552,0},
	{OBJ_ROCK,3241,402,0},
	{OBJ_DIAMOND,3241,552,0},
	{OBJ_DIAMOND,3292,542,0},
	{OBJ_DIAMOND,3343,522,0},
	{OBJ_ROCK,3393,462,0},
	{OBJ_ROCK,3543,522,0},
	{OBJ_ROCK,3693,582,0},
	{OBJ_DIAMOND,3743,582,0},
	{OBJ_DIAMOND,3783,582,0},
	{OBJ_DIAMOND,3823,582,0},
	{OBJ_DIAMOND,3863,582,0},
	{OBJ_ROCK,3913,582,0},
	{OBJ_ROCK,3963,582,0},
	{OBJ_ROCK,4114,522,0},
	{OBJ_ROCK,4115,463,0},
	{OBJ_ROCK,4116,403,0},
	{OBJ_ROCK,4117,343,0}, {OBJ_BIRD,4117 - 16,343 - 32,0},
	{OBJ_DIAMOND,4139,313,0},
	{OBJ_DIAMOND,4179,343,0},
	{OBJ_DIAMOND,4219,383,0},
	{OBJ_DIAMOND,4259,433,0},
	{OBJ_DIAMOND,4299,493,0},
	{OBJ_ROCK,4559,394,0},
	{OBJ_DIAMOND,4499,395,0},
	{OBJ_DIAMOND,4561,335,0},
	{OBJ_DIAMOND,4621,395,0},
	{OBJ_DIAMOND,4561,455,0},
	{OBJ_ROCK,4831,294,0},
	{OBJ_DIAMOND,4771,295,0},
	{OBJ_DIAMOND,4832,235,0},
	{OBJ_DIAMOND,4893,295,0},
	{OBJ_DIAMOND,4833,355,0},
	{OBJ_ROCK,5105,494,0},
	{OBJ_DIAMOND,5045,495,0},
	{OBJ_DIAMOND,5106,435,0},
	{OBJ_DIAMOND,5167,495,0},
	{OBJ_DIAMOND,5107,555,0},
	{OBJ_ROCK,5378,425,0}, {OBJ_BIRD,5378 - 16,425 - 32,0},
	{OBJ_DIAMOND,5318,426,0},
	{OBJ_DIAMOND,5379,366,0},
	{OBJ_DIAMOND,5440,426,0},
	{OBJ_DIAMOND,5380,486,0},
	{OBJ_DIAMOND,5593,386,0},
	{OBJ_DIAMOND,5642,336,0},
	{OBJ_DIAMOND,5692,386,0},
	{OBJ_DIAMOND,5742,336,0},
	{OBJ_DIAMOND,5792,386,0},
	{OBJ_DIAMOND,5843,336,0},
	{OBJ_DIAMOND,5892,386,0},
	{OBJ_SPIKE,5941,145,0},
	{OBJ_SPIKE,5981,564,0}, {OBJ_BIRD,5981 - 16,564 - 32,0},
	{OBJ_SPIKE,6022,155,0},
	{OBJ_ROCK,6063,556,0}, {OBJ_BIRD,6063 - 16,556 - 32,0},
	{OBJ_ROCK,6104,176,0},
	{OBJ_ROCK,6144,536,0}, {OBJ_BIRD,6144 - 16,536 - 32,0},
	{OBJ_ROCK,6185,206,0},
	{OBJ_ROCK,6226,506,0}, {OBJ_BIRD,6226 - 16,506 - 32,0},
	{OBJ_ROCK,6266,246,0},
	{OBJ_ROCK,6306,466,0}, {OBJ_BIRD,6306 - 16,466 - 32,0},
	{ OBJ_ROCK,6347,296,0 },
	{ OBJ_ROCK,6388,416,0 },
	{ OBJ_ROCK,6429,356,0 },
	{ OBJ_DIAMOND,6428,406,0 },
	{ OBJ_ROCK,6470,356,0 },
	{ OBJ_DIAMOND,6469,406,0 },
	{ OBJ_DIAMONDSUPER,6520,107,0 },
	{ OBJ_ROCK,6521,356,0 },
	{ OBJ_DIAMOND,6519,406,0 },
	{ OBJ_ROCK,6562,356,0 },
	{ OBJ_DIAMOND,6561,406,0 },
	{ OBJ_ROCK,6603,295,0 },
	{ OBJ_DIAMOND,6602,406,0 },
	{ OBJ_ROCK,6644,416,0 }, { OBJ_BIRD,6644 - 16,416 - 32,0 }, // killed offscreen originally
	{ OBJ_ROCK,6685,246,0 }, { OBJ_BIRD,6685 - 16,246 - 32,0 },
	{ OBJ_ROCK,6725,466,0 },
	{ OBJ_ROCK,6765,206,0 }, { OBJ_BIRD,6765 - 16,206 - 32,0 },
	{ OBJ_ROCK,6806,506,0 },
	{ OBJ_SPIKE,6846,175,0 }, { OBJ_BIRD,6846 - 16,175 - 32,0 },
	{ OBJ_ROCK,6889,536,0 },
	{ OBJ_SPIKE,6926,155,0 }, { OBJ_BIRD,6926 - 16,155 - 32,0 }, // killed offscreen originally
	{ OBJ_SPIKE,6969,555,0 },
	{ OBJ_SPIKE,7008,145,0 }, { OBJ_BIRD,7008 - 16,145 - 32,0 },
	{ OBJ_BIGROCK,7000,350,1 }, { OBJ_DIAMOND,7000,419,1 }, // moving rock
	{ OBJ_SPIKE,7049,565,0 },
	{ OBJ_SPIKE,8030,384,0 },
	{ OBJ_ROCK,8071,385,0 },
	{ OBJ_ROCK,8111,385,0 },
	{ OBJ_SPIKE,8311,106,0 },
	{ OBJ_SPIKE,8312,265,0 },
	{ OBJ_SPIKE,8313,424,0 },
	{ OBJ_ROCK,8314,584,0 },
	{ OBJ_DIAMOND,8410,305,0 },
	{ OBJ_DIAMOND,8460,305,0 },
	{ OBJ_DIAMOND,8511,305,0 },
	{ OBJ_ROCK,8605,106,0 },
	{ OBJ_ROCK,8606,225,0 },
	{ OBJ_SPIKE,8607,344,0 },
	{ OBJ_ROCK,8608,465,0 },
	{ OBJ_SPIKE,8610,584,0 },
	{ OBJ_DIAMOND,8669,165,0 },
	{ OBJ_DIAMOND,8679,285,0 },
	{ OBJ_DIAMOND,8689,405,0 },
	{ OBJ_DIAMOND,8699,524,0 },
	{ OBJ_BIGROCK,9097,337,0 },
	{ OBJ_ROCK,9801,527,0 },
	{ OBJ_ROCK,9850,527,0 },
	{ OBJ_SPIKE,9899,526,0 },
	{ OBJ_DIAMOND,9899,452,0 },
	{ OBJ_DIAMOND,9949,452,0 },
	{ OBJ_SPIKE,9897,376,0 },
	{ OBJ_ROCK,9949,377,0 },
	{ OBJ_SPIKE,9998,377,0 },
	{ OBJ_DIAMOND,9999,303,0 },
	{ OBJ_DIAMOND,10049,303,0 },
	{ OBJ_ROCK,9998,228,0 },
	{ OBJ_ROCK,10049,228,0 },
	{ OBJ_SPIKE,10099,227,0 },
	{ OBJ_BIGROCK,10899,306,0 },
	{ OBJ_DIAMOND,10903,224,0 },
	{ OBJ_BIGROCK,11332,406,0 }, { OBJ_BIRD,11323,344,0 },
	{ OBJ_DIAMOND,11335,485,0 },
	{ OBJ_BIGROCK,11914,406,0 },
	{ OBJ_DIAMOND,11837,405,0 },
	{ OBJ_DIAMOND,11863,355,0 },
	{ OBJ_DIAMOND,11862,455,0 },
	{ OBJ_DIAMOND,11919,325,0 },
	{ OBJ_DIAMOND,11919,485,0 },
	{ OBJ_DIAMOND,11976,355,0 },
	{ OBJ_DIAMOND,11975,455,0 },
	{ OBJ_DIAMOND,12001,405,0 },
};*/

// Stage 2
StageData stagedat[] = {
	{OBJ_SPIKE,907,180,0},
	{OBJ_ROCK,967,239,0},
	{OBJ_ROCK,1027,289,0},
	{OBJ_BIGROCK,1104,129,0},
	{OBJ_ROCK,1087,314,0}, { OBJ_BIRD,1087 - 16,314 - 32,0 },
	{OBJ_ROCK,1127,314,0},
	{OBJ_ROCK,1187,289,0}, { OBJ_BIRD,1187 - 16,289 - 32,0 },
	{OBJ_ROCK,1249,239,0},
	{OBJ_SPIKE,1311,180,0},

	{OBJ_DIAMOND,1248,419,0},
	{OBJ_DIAMOND,1279,419,0},
	{OBJ_DIAMOND,1310,419,0},
	{OBJ_DIAMOND,1341,419,0},

	{OBJ_SPIKE,1280,609,0},
	{OBJ_ROCK,1342,549,0},
	{OBJ_ROCK,1403,499,0},
	{OBJ_ROCK,1463,474,0},
	{OBJ_BIGROCK,1479,660,0},
	{OBJ_ROCK,1502,474,0},
	{OBJ_ROCK,1562,499,0},
	{OBJ_ROCK,1622,549,0},
	{OBJ_SPIKE,1683,609,0},

	{OBJ_SPIKE,1742,310,1},
	{OBJ_SPIKE,1773,240,1},
	{OBJ_SPIKE,1825,200,1},
	{OBJ_SPIKE,1887,181,1},
	{OBJ_ROCK,1958,180,1},
	{OBJ_SPIKE,2021,199,1},
	{OBJ_SPIKE,2074,238,1},
	{OBJ_SPIKE,2109,308,1},

	{OBJ_ROCK,1743,380,2},
	{OBJ_ROCK,1775,449,2},
	{OBJ_ROCK,1827,489,2},
	{OBJ_ROCK,1889,509,2},
	{OBJ_ROCK,1962,509,2},
	{OBJ_ROCK,2025,489,2},
	{OBJ_ROCK,2076,451,2},
	{OBJ_ROCK,2107,380,2},

	{OBJ_DIAMOND,1826,389,0},
	{OBJ_DIAMOND,1888,389,0},
	{OBJ_DIAMOND,1961,389,0},
	{OBJ_DIAMOND,2024,389,0},
};

void ReloadStage(bool OnHurt)
{
	if (OnHurt) {
		RespawnDoggy();
		doggy.maxfish = 2;
	}
		
	InitFish();

	// TODO: clean this
	frame.x = 0;
	frame.y = doggy.y - (SURFACE_HEIGHT << 9);
	map.height = 768; //780;
	map.progress = 0;
	map.nexttrigger = 0;

	LifeCountFade = 0;
	SetFade(FADEMODE_FADEOUT);

	for (int i = 0; i < MAXOBJECTS; i++) {
		memset(&map.objects[i], 0, sizeof(Object));
		memset(&map.carets[i], 0, sizeof(Object));
	}

	for (int a = 0; a < (sizeof(stagedat) / sizeof(stagedat[0])); a++) {
		int b = MapSpawn(stagedat[a].object, stagedat[a].x * THAT_FULLPIXEL, stagedat[a].y * THAT_FULLPIXEL);
		map.objects[b].tag = stagedat[a].tag;
	}
}

BOOL Game(HWND hWnd)
{
    DWORD tick;
    LoadGenericData();

	InitFade();
	InitDoggy();
	map.stage = 2; // TODO
	ReloadStage(false);
	SetFade(FADEMODE_FADEOUT);
	StageTxtReset();
	PlaySoundObject(SOUND_ID_MUS_TITLE, SOUND_MODE_PLAY);

	while (1)
	{
		//Start frame
		tick = GetTickCount();

		GetTrg();
		
		switch (mode) {
			case GAMEMODE_TITLE:
				GameTitle();
				break;
			case GAMEMODE_LOAD:
				GameLoad();
				break;
			case GAMEMODE_GAMEPLAY:
				GameLoop();
				break;
			case GAMEMODE_CLEAR:
				GameClear();
				break;
			case GAMEMODE_GAMEOVER:
				break;
			case GAMEMODE_END:
				break;
		}

		// FPS (probably hide later)
		PutNumber(SURFACE_WIDTH - 48, 0, CountFramePerSecound());

		//End frame
		if (!Flip_SystemTask(hWnd))
			return TRUE;
        
	}
    return FALSE;
}