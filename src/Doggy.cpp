#include "Doggy.h"
#include "Draw.h"
#include "Sound.h"
#include "Carets.h"
#include "Objects.h"
#include <math.h>

Doggy doggy;
Fish fish[MAXFISH];

void InitDoggy()
{
	doggy.x = 320 * THAT_FULLPIXEL;
	doggy.y = 610 * THAT_FULLPIXEL;
	doggy.xm = 0;
	doggy.ym = 0;
	doggy.flightbar = 128 * THAT_FULLPIXEL;
	doggy.fishpower = THAT_FULLPIXEL;
	doggy.maxfish = 2; //5;
	doggy.ani_detail = 0;
	doggy.ani_wait = 0;
	doggy.ani_no = 0;
	doggy.life = 3;
	doggy.diamonds = 0;
	doggy.score = 0;
	doggy.hit = { 12,16,12,16 };

	doggy.invutick = 240;
	doggy.blink = false;
	doggy.hurt = false;
	doggy.win = false;

	doggy.bubble = false;
	doggy.bubblespr.x = 0;
	doggy.bubblespr.y = 0;
}

void RespawnDoggy()
{
	doggy.x = 132 * THAT_FULLPIXEL; //320
	doggy.y = 610 * THAT_FULLPIXEL;
	doggy.xm = 0;
	doggy.ym = 0;
	doggy.flightbar = 128 * THAT_FULLPIXEL;
	doggy.fishpower = THAT_FULLPIXEL;
	doggy.ani_detail = 0;
	doggy.ani_wait = 0;
	doggy.ani_no = 0;

	doggy.invutick = 240;
	doggy.blink = false;
	doggy.hurt = false;
	doggy.win = false;
	doggy.bubble = false;
}

void ShowScore(int x, int y, int no) {
	doggy.score += no;

	int ntbl[3] = { 100, 10, 1 };

	BOOL v3 = FALSE;
	int v4;
	for (int i = 0; i < 3; i++)
	{
		v4 = 0;
		while (no >= ntbl[i])
		{
			++v4;
			no -= ntbl[i];
			v3 = 1;
		}
		if (v3 || i == 2) {
			int myscore = SpawnCaret(CARET_SCORE, x + ((i * 8) * THAT_FULLPIXEL), y, 0, 0);
			map.carets[myscore].ani_no = v4;
		}
	}
}

void HurtDoggy()
{
	if (doggy.invutick == 0) {
		if (doggy.bubble) {
			PlaySoundObject(SOUND_ID_HIT, SOUND_MODE_PLAY); // TODO: replace this

			for (int a = 0; a < 8; a++) {
				int myfish = SpawnCaret(CARET_BUBBLE, doggy.x + (16 * THAT_FULLPIXEL), doggy.y + (16 * THAT_FULLPIXEL),
					Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL), Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL));
				map.carets[myfish].act_no = 1;
				map.carets[myfish].ani_no = Random(0, 4);
			}

			for (int a = 0; a < 12; a++) {
				int myfish = SpawnCaret(CARET_BUBBLE, doggy.x + (16 * THAT_FULLPIXEL), doggy.y + (16 * THAT_FULLPIXEL),
					Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL), Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL));
				map.carets[myfish].ani_no = Random(0, 4);
			}


			doggy.bubble = false;
			doggy.invutick = 240;
		}
		else {
			PlaySoundObject(SOUND_ID_HIT, SOUND_MODE_PLAY);
			doggy.hurt = true;
			doggy.ani_no = DOGGY_HURT;
			doggy.life--;
			gDelayGame = 20;
		}
	}
}

void PutDoggy(Frame* frame)
{
	if (doggy.blink)
		return;

	RECT rcDoggy[] = {
		{ 0, 0, 64, 64}, // Normal

		{64, 0, 128, 64}, // Flap 1
		{128, 0, 192, 64}, // Flap 2
		{192, 0, 256, 64}, // Flap 3
		{256, 0, 320, 64}, // Flap 4 (up up)
		{320, 0, 384, 64}, // Flap 5
		{384, 0, 448, 64}, // Flap 6

		{64, 0, 128, 64}, // Pre-Glide
		{448, 0, 512, 64}, // Glide
		{64, 0, 128, 64}, // Glide-End

		{0, 64, 64, 128}, // Throw 1
		{64, 64, 128, 128}, // Throw 2 (hold)
		{128, 64, 192, 128 }, // Throw 3
		{192, 64, 256, 128}, // Throw 4 (fire)
		{256, 64, 320, 128}, // Throw 5

		{448, 64, 512, 128}, // Hurt

		{0, 128, 64, 192}, // Run 1
		{64, 128, 128, 192}, // Run 2
		{128, 128, 192, 192},// Run 3
		{192, 128, 256, 192},// Run 4
		{256, 128, 320, 192},// Run 5
		{320, 128, 384, 192},// Run 6
	};
	RECT rcDoggyEars[] = {
		{48, 192, 80, 208}, // Normal
		{48, 208, 80, 224},
		{48, 224, 80, 240},
		{0, 192, 32, 208}, // Up
		{0, 208, 32, 224},
		{0, 224, 32, 240},
		{96, 192, 128, 208}, // Down
		{96, 208, 128, 224},
		{96, 224, 128, 240},
	};
	RECT rcDoggyTail[] = {
		{80, 192, 96, 208}, // Normal
		{80, 208, 96, 224},
		{80, 224, 96, 240},
		{32, 192, 48, 208}, // Up
		{32, 208, 48, 224},
		{32, 224, 48, 240},
		{128, 192, 144, 208}, // Down
		{128, 208, 144, 224},
		{128, 224, 144, 240},
	};

	// TODO: adjust all frames
	// Left Top (Tail) Right Bottom (Ears)
	RECT rcDoggyTailEarsOfs[] = {
		{ 34, 7, 13, 28}, // Normal
		{ 34, 7, 13, 28}, // Flap 1
		{ 34, 7, 13, 28}, // 2
		{ 34, 7, 13, 28}, // 3
		{ 34, 7, 13, 28}, // 4
		{ 34, 7, 13, 28}, // 5
		{ 34, 7, 13, 28}, // 6
		{ 34, 7, 13, 28}, // Pre-Glide
		{ 34, 7, 13, 28}, // Glide
		{ 34, 7, 13, 28}, // Glide-End
		{ 25, -2, 16, 32}, // Throw 1
		{ 25, -2, 16, 32}, // Throw 2
		{ 25, -2, 16, 32}, // Throw 3
		{ 25, -2, 16, 32}, // Throw 4
		{ 34, 7, 13, 28}, // Throw 5
		{ 34, 7, 13, 28}, // Hurt
		{ 34, 7, 13, 28}, // Run 1
		{ 34, 7, 13, 28}, // 2
		{ 34, 7, 13, 28}, // 3
		{ 34, 7, 13, 28}, // 4
		{ 34, 7, 13, 28}, // 5
		{ 34, 7, 13, 28}, // 6
	};

	PutBitmap3(&grcFull, ((doggy.x - (rcDoggyTailEarsOfs[doggy.ani_no].right * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->x / THAT_FULLPIXEL),
		((doggy.y - (rcDoggyTailEarsOfs[doggy.ani_no].bottom * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->y / THAT_FULLPIXEL),
		&rcDoggyEars[doggy.ani_detail], SURFACE_ID_DOGGY);

	PutBitmap3(&grcFull, ((doggy.x - (rcDoggyTailEarsOfs[doggy.ani_no].left * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->x / THAT_FULLPIXEL),
		((doggy.y - (rcDoggyTailEarsOfs[doggy.ani_no].top * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->y / THAT_FULLPIXEL),
		&rcDoggyTail[doggy.ani_detail], SURFACE_ID_DOGGY);

	PutBitmap3(&grcFull, ((doggy.x - (32 * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->x / THAT_FULLPIXEL),
		((doggy.y - (32 * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->y / THAT_FULLPIXEL),
		&rcDoggy[doggy.ani_no], SURFACE_ID_DOGGY);

	RECT rcDoggyBubble[] = {
		{0, 0, 80, 80},
		{80, 0, 160, 80},
		{160, 0, 240, 80},
		{80, 0, 160, 80},
	};

	// Bubble
	if (doggy.bubble) {
		PutBitmap3(&grcFull, ((doggy.bubblespr.x - (40 * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->x / THAT_FULLPIXEL),
			((doggy.bubblespr.y - (40 * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->y / THAT_FULLPIXEL),
			&rcDoggyBubble[doggy.bubblespr.ani_no], SURFACE_ID_BUBBLES);
	}
}

void ActDoggyWin()
{
	if (++doggy.ani_wait > 3) {
		doggy.ani_wait = 0;
		doggy.ani_detail++;
		doggy.ani_detail %= 3;

		if (doggy.ani_no == DOGGY_GLIDESTART)
			doggy.ani_no++;

		if (doggy.ani_no != DOGGY_GLIDESTART && doggy.ani_no != DOGGY_GLIDESTART + 1)
			doggy.ani_no = DOGGY_GLIDESTART;
	}

	if (doggy.x > map.progress + (SURFACE_WIDTH * THAT_FULLPIXEL)) {
		SetFade(FADEMODE_FADEIN);
		if (ActFade() == true) {
			//TODO: finding a better way
			RECT rcFade = { 0, 0, SURFACE_WIDTH, SURFACE_HEIGHT };
			CortBox(&rcFade, 0x000000);
			mode = GAMEMODE_CLEAR;
		}
	}

	if (doggy.xm > 4 * THAT_FULLPIXEL)
		doggy.xm = 4 * THAT_FULLPIXEL;
	else
		doggy.xm += (THAT_FULLPIXEL/8);
	doggy.x += doggy.xm;
	//doggy.x += 4 * THAT_FULLPIXEL;
}

void ActDoggyHurt()
{
	doggy.xm -= 0x80;

	if (doggy.xm > 8 * THAT_FULLPIXEL)
		doggy.xm = 8 * THAT_FULLPIXEL;
	if (doggy.xm < -8 * THAT_FULLPIXEL)
		doggy.xm = -8 * THAT_FULLPIXEL;

	if (doggy.ym > 4 * THAT_FULLPIXEL)
		doggy.ym = 4 * THAT_FULLPIXEL;
	if (doggy.ym < -4 * THAT_FULLPIXEL)
		doggy.ym = -4 * THAT_FULLPIXEL;

	if (doggy.x < map.progress) {
		SetFade(FADEMODE_FADEIN);
		if (ActFade() == true) {
			ReloadStage(true);
		}
	}
		

	doggy.x += doggy.xm;
	doggy.y += doggy.ym;

}

void ActDoggyNormal()
{
	if (doggy.xm < 0)
		doggy.xm += 0x40;
	if (doggy.xm > 0)
		doggy.xm -= 0x40;

	if (gKey & KEY_LEFT)
		doggy.xm -= 0x100;
	if (gKey & KEY_RIGHT)
		doggy.xm += 0x100;

	if (gKey & KEY_UP) {
		if (doggy.flightbar > 0 && !(doggy.ym < 0x40 && doggy.ym > -0x40)) {
			if (doggy.ani_no == DOGGY_IDLE) {
				doggy.ani_wait = 0; doggy.ani_no = DOGGY_GLIDESTART;
			}
		}
	}
	else {
		if (doggy.ani_no == DOGGY_GLIDE) {
			doggy.ani_wait = 0;
			doggy.ani_no = DOGGY_GLIDEEND;
		}
	}

	if (doggy.ani_no == DOGGY_GLIDE)
	{
		if (doggy.flightbar > 0)
		{
			doggy.ym -= 0x40;
			doggy.flightbar -= (THAT_FULLPIXEL / 4);
		}
		else {
			doggy.ym += 0x40;
			// I have no clue if there would be a special frame here or not
		}
	}
	else {
		doggy.ym += 0x40;
		if (doggy.flightbar < 128 * THAT_FULLPIXEL)
		{
			doggy.flightbar += (THAT_FULLPIXEL / 4);
		}
	}

	// TODO: make this part of the flap animation
	// repeats 16 frames when held

	//gKeyTrg
	if (gKey & KEY_Z && doggy.ani_no == DOGGY_IDLE) {
		if (doggy.flightbar > 16 * THAT_FULLPIXEL) {
			doggy.ani_wait = 0;
			doggy.ani_no = DOGGY_FLAPSTART;
			doggy.flightbar -= 16 * THAT_FULLPIXEL;
		}
	}

	if (gKey & KEY_X) {
		if (doggy.ani_no == DOGGY_IDLE) {
			doggy.ani_wait = 0;
			doggy.ani_no = DOGGY_THROWSTART;
			doggy.holdingfish = true;
		}
	}
	else {
		if (doggy.ani_no == DOGGY_THROWHOLD)
			doggy.ani_no++;
	}

	static int fishpowerpip = 0;
	if (doggy.ani_no == DOGGY_THROWHOLD) {
		if (fishpowerpip != doggy.fishpower / THAT_FULLPIXEL) {
			fishpowerpip = doggy.fishpower / THAT_FULLPIXEL;
			if (fishpowerpip == 2)
				PlaySoundObject(SOUND_ID_FISH1, SOUND_MODE_PLAY);
			if (fishpowerpip == 3)
				PlaySoundObject(SOUND_ID_FISH2, SOUND_MODE_PLAY);
			if (fishpowerpip == 4)
				PlaySoundObject(SOUND_ID_FISH3, SOUND_MODE_PLAY);
			if (fishpowerpip == 5)
				PlaySoundObject(SOUND_ID_FISH4, SOUND_MODE_PLAY);
		}

		if (doggy.fishpower < (doggy.maxfish) * THAT_FULLPIXEL)
			doggy.fishpower += THAT_FULLPIXEL / 6;
	}

	if (++doggy.ani_wait > 3) {
		doggy.ani_wait = 0;
		doggy.ani_detail++;

		// Flap Sequence
		if (doggy.ani_no == DOGGY_FLAP2) {
			PlaySoundObject(SOUND_ID_FLAP, SOUND_MODE_PLAY);
			doggy.ym -= 4 * THAT_FULLPIXEL;
		}
		if (doggy.ani_no == DOGGY_FLAPEND)
			doggy.ani_no = DOGGY_IDLE;
		if (doggy.ani_no >= DOGGY_FLAPSTART && doggy.ani_no < DOGGY_FLAPEND)
			doggy.ani_no++;

		// Glide Sequence
		if (doggy.ani_no == DOGGY_GLIDESTART)
			doggy.ani_no++;
		if (doggy.ani_no == DOGGY_GLIDEEND)
			doggy.ani_no = DOGGY_IDLE;

		// Throw Sequence
		if (doggy.ani_no == DOGGY_THROWSTART)
			doggy.ani_no++;
		if (doggy.ani_no == DOGGY_THROWRELEASE) {
			// FISH
			if (doggy.holdingfish) {
				PlaySoundObject(SOUND_ID_THROW, SOUND_MODE_PLAY);
				ShootFish(doggy.fishpower / THAT_FULLPIXEL, doggy.x - (20 * THAT_FULLPIXEL), doggy.y - (20 * THAT_FULLPIXEL));
				doggy.holdingfish = false;
				doggy.fishpower = THAT_FULLPIXEL;//-(THAT_FULLPIXEL / 2);
			}
		}
		if (doggy.ani_no == DOGGY_THROWRELEASEEND)
			doggy.ani_no = DOGGY_IDLE;
		if (doggy.ani_no >= DOGGY_THROWRELEASESTART && doggy.ani_no < DOGGY_THROWRELEASEEND)
			doggy.ani_no++;
	}
	doggy.ani_detail %= 3;

	if (doggy.ym > THAT_FULLPIXEL)
		doggy.ani_detail += 6;
	if (doggy.ym < -THAT_FULLPIXEL)
		doggy.ani_detail += 3;

	if (doggy.xm > 4 * THAT_FULLPIXEL)
		doggy.xm = 4 * THAT_FULLPIXEL;
	if (doggy.xm < -4 * THAT_FULLPIXEL)
		doggy.xm = -4 * THAT_FULLPIXEL;

	if (doggy.ym > 4 * THAT_FULLPIXEL)
		doggy.ym = 4 * THAT_FULLPIXEL;
	if (doggy.ym < -4 * THAT_FULLPIXEL)
		doggy.ym = -4 * THAT_FULLPIXEL;

	doggy.x += doggy.xm;
	doggy.y += doggy.ym;
}

#define DIAMOND_GRADEUP 20 //20

void ActDoggy()
{
	if (doggy.invutick > 0) {
		doggy.blink = !doggy.blink;
		doggy.invutick--;
	}
	else {
		doggy.blink = false;
		doggy.invutick = 0;
	}

	// Gradeup
	static int curDiamond = 0;
	if ((doggy.diamonds / DIAMOND_GRADEUP) != curDiamond) {
		if (curDiamond < (doggy.diamonds / DIAMOND_GRADEUP)) {
			PlaySoundObject(SOUND_ID_GRADEUP, SOUND_MODE_PLAY);
			SpawnCaret(CARET_GRADEUP, doggy.x, doggy.y + (8 * THAT_FULLPIXEL), 0, 0);
			if (doggy.maxfish != 5)
				doggy.maxfish++;
		}
		curDiamond = (doggy.diamonds / DIAMOND_GRADEUP);
	}

	// Bubble
	if (++doggy.bubblespr.ani_wait > 3) {
		doggy.bubblespr.ani_wait = 0;
		doggy.bubblespr.ani_no++;
		doggy.bubblespr.ani_no %= 4;

		// Particles
		if (doggy.bubble) {
			int myfish = SpawnCaret(CARET_BUBBLE, doggy.x + (Random(-32, 32) * THAT_FULLPIXEL), doggy.y + (Random(-32, 32) * THAT_FULLPIXEL),
				-4 * THAT_FULLPIXEL, 0);
			map.carets[myfish].act_no = Random(0, 1);
			map.carets[myfish].ani_no = Random(0, 3);
		}
	}

	if (doggy.bubblespr.x < doggy.x)
		doggy.bubblespr.x += (doggy.x - doggy.bubblespr.x) / 4;
	if (doggy.bubblespr.x > doggy.x)
		doggy.bubblespr.x += (doggy.x - doggy.bubblespr.x) / 4;
	if (doggy.bubblespr.y < doggy.y)
		doggy.bubblespr.y += (doggy.y - doggy.bubblespr.y) / 4;
	if (doggy.bubblespr.y > doggy.y)
		doggy.bubblespr.y += (doggy.y - doggy.bubblespr.y) / 4;
	
	if (doggy.win) {
		ActDoggyWin();
		return;
	}

	if (doggy.hurt) {
		ActDoggyHurt();
		return;
	}

	ActDoggyNormal();
}

bool JudgeTouch(Object* obj)
{
	int id = obj->id;
	if (doggy.x + (24 * THAT_FULLPIXEL) > obj->x - (gObjectTable[id].hit.right * THAT_FULLPIXEL) &&
		doggy.x - (24 * THAT_FULLPIXEL) < obj->x + (gObjectTable[id].hit.left * THAT_FULLPIXEL) &&
		doggy.y + (24 * THAT_FULLPIXEL) > obj->y - (gObjectTable[id].hit.top * THAT_FULLPIXEL) &&
		doggy.y - (24 * THAT_FULLPIXEL) < obj->y + (gObjectTable[id].hit.bottom * THAT_FULLPIXEL))
	{
		return true;
	}
	return false;
}

bool JudgeSolid(Object* obj)
{
	int id = obj->id;

	int fx = abs(obj->x - doggy.x);
	int fy = abs(obj->y - doggy.y);

	if (fx == 0)
		fx = 1;
	if (fy == 0)
		fy = 1;

	bool hit = false;

	if (fy / fx > (gObjectTable[id].hit.top * THAT_FULLPIXEL) / ((gObjectTable[id].hit.left * THAT_FULLPIXEL) + THAT_FULLPIXEL))
	{
		if (doggy.x - (doggy.hit.left * THAT_FULLPIXEL) < obj->x + (gObjectTable[id].hit.right * THAT_FULLPIXEL) &&
			doggy.x + (doggy.hit.right * THAT_FULLPIXEL) > obj->x - (gObjectTable[id].hit.left * THAT_FULLPIXEL)) {
			if (doggy.y - (doggy.hit.top * THAT_FULLPIXEL) < obj->y + (gObjectTable[id].hit.bottom * THAT_FULLPIXEL) &&
				doggy.y - (doggy.hit.bottom * THAT_FULLPIXEL) > obj->y) {

				if (doggy.ym < obj->ym)
				{
					doggy.ym = obj->ym;
					doggy.y = obj->y + (gObjectTable[id].hit.bottom * THAT_FULLPIXEL) + (doggy.hit.top * THAT_FULLPIXEL) + THAT_FULLPIXEL;
				}
				else {
					if (doggy.ym < 0)
						doggy.ym = 0;
				}
				hit = true;
			}
			if (doggy.y + (doggy.hit.bottom * THAT_FULLPIXEL) > obj->y - (gObjectTable[id].hit.top * THAT_FULLPIXEL) &&
				doggy.y + (doggy.hit.bottom * THAT_FULLPIXEL) < obj->y + (4 * THAT_FULLPIXEL)) {
				if (doggy.ym > obj->ym) {
					doggy.ym = obj->ym;
					doggy.y = obj->y - (gObjectTable[id].hit.top * THAT_FULLPIXEL) - (doggy.hit.bottom * THAT_FULLPIXEL) + THAT_FULLPIXEL;
				}
				hit = true;
			}
		}
	}
	else {
		if (doggy.y - (doggy.hit.top * THAT_FULLPIXEL) < obj->y + (gObjectTable[id].hit.bottom * THAT_FULLPIXEL) &&
			doggy.y + (doggy.hit.bottom * THAT_FULLPIXEL) > obj->y - (gObjectTable[id].hit.top * THAT_FULLPIXEL)) {
			if (doggy.x - (doggy.hit.left * THAT_FULLPIXEL) < obj->x + (gObjectTable[id].hit.right * THAT_FULLPIXEL) &&
				doggy.x - (doggy.hit.left * THAT_FULLPIXEL) > obj->x) {
				if (doggy.xm < obj->xm)
					doggy.xm = obj->xm;
				doggy.x = obj->x + (gObjectTable[id].hit.left * THAT_FULLPIXEL) + (doggy.hit.right * THAT_FULLPIXEL);
				hit = true;
			}
			if (doggy.x + (doggy.hit.right * THAT_FULLPIXEL) > obj->x - (gObjectTable[id].hit.left * THAT_FULLPIXEL) &&
				doggy.x + (doggy.hit.right * THAT_FULLPIXEL) < obj->x) {
				if (doggy.xm > obj->xm)
					doggy.xm = obj->xm;
				doggy.x = obj->x - (gObjectTable[id].hit.right * THAT_FULLPIXEL) - (doggy.hit.left * THAT_FULLPIXEL);
				hit = true;
			}
		}
	}
	return hit;
}

void HitDoggy()
{
	if (doggy.win)
		return;

	if (doggy.hurt)
		return;

	for (int i = 0; i < MAXOBJECTS; i++) {
		if (map.objects[i].id != OBJ_NULL) {
			int id = map.objects[i].id;
			bool hit = false;

			if(gObjectTable[id].flags & isSolid)
				hit = JudgeSolid(&map.objects[i]);
			if (gObjectTable[id].flags & isTouchable)
				hit = JudgeTouch(&map.objects[i]);

			if (hit) {
				switch (id) {
				case OBJ_DIAMONDSUPER:
					// 5 Diamonds + 500 Score + Bubble + Refill Flight
					doggy.diamonds += 5;
					ShowScore(map.objects[i].x, map.objects[i].y, 500);
					doggy.bubble = true;
					PlaySoundObject(SOUND_ID_BLUEDIAMOND, SOUND_MODE_PLAY);

					for (int a = 0; a < 8; a++) {
						int myfish = SpawnCaret(CARET_BUBBLE, doggy.x + (16 * THAT_FULLPIXEL), doggy.y + (16 * THAT_FULLPIXEL),
							Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL), Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL));
						map.carets[myfish].act_no = 1;
						map.carets[myfish].ani_no = Random(0, 4);
					}

					for (int a = 0; a < 16; a++) {
						int myfish = SpawnCaret(CARET_BUBBLE, doggy.x + (16 * THAT_FULLPIXEL), doggy.y + (16 * THAT_FULLPIXEL),
							Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL), Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL));
						map.carets[myfish].ani_no = Random(0, 4);
					}

					doggy.flightbar = 128 * THAT_FULLPIXEL;

					map.objects[i].id = OBJ_NULL;
					break;
				case OBJ_DIAMOND:
					// 1 Diamond + 100 Score
					doggy.diamonds += 1;
					ShowScore(map.objects[i].x, map.objects[i].y, 100);
					PlaySoundObject(SOUND_ID_DIAMOND, SOUND_MODE_PLAY);

					for (int a = 0; a < 8; a++)
						SpawnCaret(CARET_STAR, map.objects[i].x + (8 * THAT_FULLPIXEL), map.objects[i].y + (8 * THAT_FULLPIXEL), Random(-8 * THAT_FULLPIXEL, 8 * THAT_FULLPIXEL), Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL));

					doggy.flightbar += 8 * THAT_FULLPIXEL;
					if (doggy.flightbar > 128 * THAT_FULLPIXEL)
						doggy.flightbar = 128 * THAT_FULLPIXEL;

					map.objects[i].id = OBJ_NULL;
					break;
				}
				if(gObjectTable[id].flags & isHazard)
					HurtDoggy();
			}
		}
	}

	// Ground (Temp)
	if (doggy.y > (620 - doggy.hit.bottom) * THAT_FULLPIXEL) { //636
		HurtDoggy();
		doggy.ym = 0;
		doggy.y = (620 - doggy.hit.bottom) * THAT_FULLPIXEL;
	}

	// Ceiling
	if (doggy.y < doggy.hit.top * THAT_FULLPIXEL) {
		doggy.ym = 0;
		doggy.y = doggy.hit.top * THAT_FULLPIXEL;
	}

	// Edge of the Screen
	if (doggy.x < map.progress + (32 * THAT_FULLPIXEL)) {
		HurtDoggy();
		doggy.xm = THAT_FULLPIXEL;
		doggy.x = map.progress + (32 * THAT_FULLPIXEL);
	}

	if (doggy.x > map.progress + ((640 - 32) * THAT_FULLPIXEL)) {
		doggy.xm = 0;
		doggy.x = map.progress + ((640 - 32) * THAT_FULLPIXEL);
	}
}

void InitFish()
{
	// Clear Fishes
	for (int i = 0; i < MAXFISH; i++) {
		memset(&fish[i], 0, sizeof(Fish));
	}
}

void PutFish(Frame* frame)
{
	RECT rcFish[] = {
		{0, 0, 32, 32},
		{32, 0, 64, 32},
		{64, 0, 96, 32},
		{96, 0, 128, 32},
		{128, 0, 160, 32},
		{160, 0, 192, 32},
		{192, 0, 224, 32},
		{224, 0, 256, 32},
	};

	for (int i = 0; i < MAXFISH; i++) {
		if (fish[i].active) {
			switch (fish[i].power) {
			case 0:
				PutBitmap3(&grcFull, ((fish[i].x - (8 * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->x / THAT_FULLPIXEL),
					((fish[i].y - (8 * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->y / THAT_FULLPIXEL),
					&rcFish[fish[i].ani_no], SURFACE_ID_ITEMS);
				break;
			default:
				// TODO: turn the float into a big int moment
				for (int a = 0; a < fish[i].power; a++) {
					if (fish[i].fishpart[a]) {
						float tempofs = (3.14 * 2) / (fish[i].power);
						int tempx = ((16 * (fish[i].power - 1)) * cos(((float)fish[i].ani_no / 8) + (tempofs * a)));
						int tempy = ((16 * (fish[i].power - 1)) * sin(((float)fish[i].ani_no / 8) + (tempofs * a)));
						PutBitmap3(&grcFull, ((fish[i].x - (tempx * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->x / THAT_FULLPIXEL),
							((fish[i].y - (tempy * THAT_FULLPIXEL)) / THAT_FULLPIXEL) - (frame->y / THAT_FULLPIXEL),
							&rcFish[0], SURFACE_ID_ITEMS);
					}
				}
				break;
			}
			
		}
	}
}

void HitFish(int hitid, int fishid, int fishpart)
{
	int hitscore = 0;
	int hitcaret = CARET_FEATHER_ALT; // i'll just default it to this weird blue one
	switch (map.objects[hitid].id) {
		case OBJ_BIRD:
			hitscore = 50;
			hitcaret = CARET_FEATHER;
			map.objects[hitid].id = OBJ_NULL;
		break;
		case OBJ_BUG:
			hitscore = 10;
			map.objects[hitid].id = OBJ_DIAMOND;
			map.objects[hitid].act_no = 1;
			map.objects[hitid].xm = 2 * THAT_FULLPIXEL;
			map.objects[hitid].ym = 0;
		break;
		default:
			map.objects[hitid].id = OBJ_NULL;
		break;
	}
	
	ShowScore(map.objects[hitid].x, map.objects[hitid].y, hitscore);
	PlaySoundObject(SOUND_ID_HIT, SOUND_MODE_PLAY);

	for (int i = 0; i < 8; i++) {
		int myfish = SpawnCaret(hitcaret, map.objects[hitid].x + (16 * THAT_FULLPIXEL), map.objects[hitid].y + (16 * THAT_FULLPIXEL),
			Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL), Random(-4 * THAT_FULLPIXEL, 4 * THAT_FULLPIXEL));
		map.carets[myfish].act_no = Random(0, 1);
		map.carets[myfish].ani_no = Random(0, 4);
	}
	
	fish[fishid].fishpart[fishpart] = false;
}

void ActFish()
{
	for (int i = 0; i < MAXFISH; i++) {
		if (fish[i].active) {
			if (fish[i].power == 1)
				fish[i].power = 0;
			if (fish[i].power == 0) {
				if (++fish[i].ani_wait > 2) {
					fish[i].ani_wait = 0;
					fish[i].ani_no++;
					fish[i].ani_no %= 8;
				}
			}
			else {
				fish[i].ani_no++;
			}
			fish[i].x += fish[i].xm;
			fish[i].xm -= THAT_FULLPIXEL / 6;

			//TODO: hit detection
			bool isallgone = true;
			for (int a = 0; a < 5; a++) {
				//fishpart
				if (fish[i].fishpart[a]) {
					isallgone = false;
					float tempofs = (3.14 * 2) / (fish[i].power);
					int tempx = ((16 * (fish[i].power - 1)) * cos(((float)fish[i].ani_no / 8) + (tempofs * a)));
					int tempy = ((16 * (fish[i].power - 1)) * sin(((float)fish[i].ani_no / 8) + (tempofs * a)));

					for (int e = 0; e < MAXOBJECTS; e++) {
						if (map.objects[e].id != OBJ_NULL && gObjectTable[map.objects[e].id].flags & isKillable) {
							if ((fish[i].x - (tempx * THAT_FULLPIXEL)) + (8 * THAT_FULLPIXEL) > map.objects[e].x - (gObjectTable[map.objects[e].id].hit.right * THAT_FULLPIXEL) &&
								(fish[i].x - (tempx * THAT_FULLPIXEL)) - (8 * THAT_FULLPIXEL) < map.objects[e].x + (gObjectTable[map.objects[e].id].hit.left * THAT_FULLPIXEL) &&
								(fish[i].y - (tempy * THAT_FULLPIXEL)) + (8 * THAT_FULLPIXEL) > map.objects[e].y - (gObjectTable[map.objects[e].id].hit.top * THAT_FULLPIXEL) &&
								(fish[i].y - (tempy * THAT_FULLPIXEL)) - (8 * THAT_FULLPIXEL) < map.objects[e].y + (gObjectTable[map.objects[e].id].hit.bottom * THAT_FULLPIXEL))
							{
								HitFish(e, i, a);
							}
						}
					}
				}
			}

			if(isallgone)
				fish[i].active = false;

			if (fish[i].x < map.progress - (160 * THAT_FULLPIXEL))
				fish[i].active = false;
			if (fish[i].x > map.progress + ((SURFACE_WIDTH + 160) * THAT_FULLPIXEL))
				fish[i].active = false;

		}
	}
}

void ShootFish(int power, int x, int y)
{
	int ind = 0;
	while (ind < MAXFISH && fish[ind].active)
		ind++;

	if (ind >= MAXFISH)
		return;

	memset(&fish[ind], 0, sizeof(Fish));
	fish[ind].active = true;
	fish[ind].x = x;
	fish[ind].y = y;
	fish[ind].power = power;

	for (int i = 0; i < fish[ind].power; i++)
		fish[ind].fishpart[i] = true;

	fish[ind].xm = 12 * THAT_FULLPIXEL;
}
