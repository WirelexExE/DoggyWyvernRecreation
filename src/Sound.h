#pragma once

#ifndef DIRECTSOUND_VERSION
#define DIRECTSOUND_VERSION 0x500
#endif
#include <dsound.h>

#include "windows.h"

#define SE_MAX 160	

enum SOUND_ID
{
    SOUND_ID_DIAMOND,
    SOUND_ID_BLUEDIAMOND,
    SOUND_ID_FISH1,
    SOUND_ID_FISH2,
    SOUND_ID_FISH3,
    SOUND_ID_FISH4,
    SOUND_ID_FLAP,
    SOUND_ID_THROW,
    SOUND_ID_HIT,
    SOUND_ID_GRADEUP,
    SOUND_ID_MUS_TITLE,
    SOUND_ID_MUS_STAGEA,
    SOUND_ID_MUS_STAGEB,
    SOUND_ID_MUS_RAIN,
};

enum SOUND_MODE
{
    SOUND_MODE_STOP,
    SOUND_MODE_PLAY,
    SOUND_MODE_LOOP,
};

BOOL InitSoundObject(LPCSTR resname, int no);
BOOL LoadSoundObject(LPCSTR name, int no);
void PlaySoundObject(int no, int mode);
bool IsSoundObjectPlaying(int no);
BOOL InitDirectSound(HWND hwnd);
void EndDirectSound(void);

void ChangeSoundVolume(int no, long volume);