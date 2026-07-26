#pragma once
#include <windows.h>
#include "Game.h"

#define MAXFISH 32

// This makes my life easier lol
enum DoggyFrame {
	DOGGY_IDLE = 0,
	DOGGY_FLAPSTART,
	DOGGY_FLAP1,
	DOGGY_FLAP2,
	DOGGY_FLAP3,
	DOGGY_FLAP4,
	DOGGY_FLAPEND,
	DOGGY_GLIDESTART,
	DOGGY_GLIDE,
	DOGGY_GLIDEEND,
	DOGGY_THROWSTART,
	DOGGY_THROWHOLD,
	DOGGY_THROWRELEASESTART,
	DOGGY_THROWRELEASE,
	DOGGY_THROWRELEASEEND,
	DOGGY_HURT,
	DOGGY_RUN1,
	DOGGY_RUN2,
	DOGGY_RUN3,
	DOGGY_RUN4,
	DOGGY_RUN5,
	DOGGY_RUN6,
};

struct DoggyBubble {
	int x, y;
	int ani_no;
	int ani_wait;
};

struct Doggy {
	int x, y;
	int xm, ym;
	int flightbar;

	int maxfish;
	bool holdingfish;
	int fishpower;

	int ani_detail;
	int ani_wait;
	int ani_no;

	int invutick;
	bool blink;
	bool invu; // 240
	bool hurt;
	bool win;

	bool bubble;
	DoggyBubble bubblespr;

	int life;
	int diamonds;
	int score;
	RECT hit;
};

struct Fish {
	int x, y;
	int xm, ym;
	int power;
	int ani_wait;
	int ani_no;
	bool active;
	bool fishpart[5];
};

extern Fish fish[MAXFISH];

void InitFish();
void PutFish(Frame* frame);
void ActFish();
void ShootFish(int power, int x, int y);

void InitDoggy();
void RespawnDoggy();
void PutDoggy(Frame* frame);
void ActDoggy();
void HitDoggy();


extern Doggy doggy;