#include "Game.h"
#include "Generic.h"
#include "Draw.h"
#include "Doggy.h"
#include "Carets.h"
#include "Objects.h"
#include "Sound.h"
#include <math.h>
#include <stdio.h>

// TODO: most things here need to be seperated

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

	// TODO: fixing this
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
StageTriggerData *stagetriggerdat;

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
		
		//if (map.nexttrigger < (sizeof(stagetriggerdat) / sizeof(stagetriggerdat[0]))) {
		if (map.nexttrigger < map.maxtriggers) {
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
	char object; //ObjectType
	int x, y;
	char tag;
};
StageData *stagedat;

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
	map.height = 768;
	map.progress = 0;
	map.nexttrigger = 0;

	LifeCountFade = 0;
	SetFade(FADEMODE_FADEOUT);

	for (int i = 0; i < MAXOBJECTS; i++) {
		memset(&map.objects[i], 0, sizeof(Object));
		memset(&map.carets[i], 0, sizeof(Object));
	}


	//for (int a = 0; a < (sizeof(stagedat) / sizeof(stagedat[0])); a++) {
	for (int a = 0; a < map.maxobjects; a++) {
		int b = MapSpawn(stagedat[a].object, stagedat[a].x * THAT_FULLPIXEL, stagedat[a].y * THAT_FULLPIXEL);
		map.objects[b].tag = stagedat[a].tag;
	}
}


const char* stagedatchecksum = "DWSTAGE";
// TODO: making a proper level editor?
/*
void ExportStage()
{
	FILE* fp = fopen("stage.dat","wb");

	if (fp == NULL)
		return;

	// Checksum
	fwrite(stagedatchecksum, sizeof(char), strlen(stagedatchecksum), fp);

	// Objects
	int maxobjectshere = (sizeof(stagedat) / sizeof(stagedat[0]));
	fwrite(&maxobjectshere, 4, 1, fp);
	for (int i = 0; i < maxobjectshere; i++) {
		fwrite(&stagedat[i].object, 1, 1, fp);
		fwrite(&stagedat[i].x, 4, 1, fp);
		fwrite(&stagedat[i].y, 4, 1, fp);
		fwrite(&stagedat[i].tag, 1, 1, fp);
	}

	// Trigger Events
	int maxtriggershere = (sizeof(stagetriggerdat) / sizeof(stagetriggerdat[0]));
	fwrite(&maxtriggershere, 4, 1, fp);
	for (int i = 0; i < maxtriggershere; i++) {
		fwrite(&stagetriggerdat[i].x, 4, 1, fp);
		fwrite(&stagetriggerdat[i].event, 4, 1, fp);
	}
	fclose(fp);
}*/

void LoadStage(LPCTSTR stage)
{
	//Get path
	char path[MAX_PATH];
	sprintf(path, "%s\\%s", gModulePath, stage);

	int maxobjectshere;
	int maxtriggershere;
	FILE* fp = fopen(path, "rb");

	if (fp == NULL)
		return;

	char datchecksum[7];
	fread(datchecksum, 1, 7, fp);

	if (memcmp(datchecksum, stagedatchecksum, sizeof(datchecksum)) != 0) {
		fclose(fp);
		MessageBox(NULL, "STAGE ERROR!!", "Message", 0);
		return;
	}

	fread(&maxobjectshere, 4, 1, fp);
	map.maxobjects = maxobjectshere;
	stagedat = (StageData*)malloc(maxobjectshere * sizeof(StageData));
	for (int i = 0; i < maxobjectshere; i++) {
		fread(&stagedat[i].object, 1, 1, fp);
		fread(&stagedat[i].x, 4, 1, fp);
		fread(&stagedat[i].y, 4, 1, fp);
		fread(&stagedat[i].tag, 1, 1, fp);
	}
	fread(&maxtriggershere, 4, 1, fp);
	map.maxtriggers = maxtriggershere;
	stagetriggerdat = (StageTriggerData*)malloc(maxtriggershere * sizeof(StageTriggerData));
	for (int i = 0; i < maxtriggershere; i++) {
		fread(&stagetriggerdat[i].x, 4, 1, fp);
		fread(&stagetriggerdat[i].event, 4, 1, fp);
	}

	fclose(fp);
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
		LoadStage("Estage1.m4");
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
		LoadStage("Estage2.m4");
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
	ReloadStage(false);
	PlaySoundObject(SOUND_ID_MUS_STAGEA, SOUND_MODE_PLAY);
	mode = GAMEMODE_GAMEPLAY;
}

BOOL Game(HWND hWnd)
{
    DWORD tick;
    LoadGenericData();

	InitFade();
	InitDoggy();
	map.stage = 1;
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