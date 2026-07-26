#include "Generic.h"
#include "Draw.h"
#include "Sound.h"
#include <stdio.h>

void GetCompileDate(int *year, int *month, int *day)
{
	int i;
	char strMonth[0x10];

	const char *table[13] = {
		"XXX",
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec",
	};

	sscanf("Dec  4 2000", "%s %d %d", strMonth, day, year);	//The expansion of __DATE__ is not reproductible. (TODO: Think about changing this to be reproductible)
	
	for (i = 0; i < 12; ++i)	//This being 12 instead of 13 might be a bug, but it works anyway by accident
		if (!memcmp(&strMonth, table[i], 3))
			break;
	
	*month = i;
}

BOOL GetCompileVersion(int *v1, int *v2, int *v3, int *v4)
{
	unsigned int puLen;
	VS_FIXEDFILEINFO *lpBuffer;
	DWORD dwHandle;
	DWORD dwLen;
	char path[MAX_PATH];
	LPVOID lpData;
	BOOL bResult;

	lpData = NULL;
	bResult = FALSE;

	GetModuleFileName(NULL, path, sizeof(path));
	dwLen = GetFileVersionInfoSize(path, &dwHandle);

	if (dwLen == 0)
		goto fail;

	lpData = malloc(dwLen);

	if (lpData == NULL)
		goto fail;

	if (!GetFileVersionInfo(path, 0, dwLen, lpData))
		goto fail;

	if (!VerQueryValueA(lpData, "\\", (LPVOID*)&lpBuffer, &puLen))
		goto fail;

	*v1 = (unsigned short)(lpBuffer->dwFileVersionMS >> 16);
	*v2 = (unsigned short)(lpBuffer->dwFileVersionMS & 0xFFFF);
	*v3 = (unsigned short)(lpBuffer->dwFileVersionLS >> 16);
	*v4 = (unsigned short)(lpBuffer->dwFileVersionLS & 0xFFFF);
	bResult = TRUE;

fail:

	if (lpData != NULL)
		free(lpData);

	return bResult;
}

void LoadGenericData()
{
	//Load surfaces
	MakeSurface_File("DogPrev.bmp", SURFACE_ID_TITLE);
	MakeSurface_Resource("GAMEINTERFACE", SURFACE_ID_GAMEINTERFACE);
	MakeSurface_Resource("GAMESTATUS", SURFACE_ID_GAMESTATUS);
	MakeSurface_File("Doggy.bmp", SURFACE_ID_DOGGY);
	MakeSurface_File("Items.bmp", SURFACE_ID_ITEMS);
	MakeSurface_File("Obstacles.bmp", SURFACE_ID_OBSTACLES);
	MakeSurface_File("Star.bmp", SURFACE_ID_STAR);
	MakeSurface_File("Enemy1.bmp", SURFACE_ID_ENEMY1);
	MakeSurface_File("Enemy2.bmp", SURFACE_ID_ENEMY2);
	MakeSurface_File("EffectBubble.bmp", SURFACE_ID_BUBBLES);

	//Background
	MakeSurface_File("Back1_1.bmp", SURFACE_ID_BACK0);
	MakeSurface_File("Back1_2.bmp", SURFACE_ID_BACK1);
	MakeSurface_File("Back1_3.bmp", SURFACE_ID_BACK2);
	MakeSurface_File("Back1_4.bmp", SURFACE_ID_BACK3);
	MakeSurface_File("Back1_5.bmp", SURFACE_ID_BACK4);
	MakeSurface_File("Back1_6.bmp", SURFACE_ID_BACK5);
	MakeSurface_File("Back1_7.bmp", SURFACE_ID_BACK6);
	MakeSurface_File("Back1_8.bmp", SURFACE_ID_BACK7);
	MakeSurface_File("Back1_9.bmp", SURFACE_ID_BACK8);
	MakeSurface_File("Back1_10.bmp", SURFACE_ID_BACK9);
	
	InitSoundObject("DIAMOND", SOUND_ID_DIAMOND);
	InitSoundObject("BLUEDIAMOND", SOUND_ID_BLUEDIAMOND);
	InitSoundObject("FISH1", SOUND_ID_FISH1);
	InitSoundObject("FISH1", SOUND_ID_FISH2);
	InitSoundObject("FISH1", SOUND_ID_FISH3);
	InitSoundObject("FISH1", SOUND_ID_FISH4);
	InitSoundObject("FLAP", SOUND_ID_FLAP);
	InitSoundObject("THROW", SOUND_ID_THROW);
	InitSoundObject("HIT", SOUND_ID_HIT);
	InitSoundObject("GRADEUP", SOUND_ID_GRADEUP);
	

	LoadSoundObject("MusTitle.wav", SOUND_ID_MUS_TITLE);

	//LoadSoundObject("MusStage1A.wav", SOUND_ID_MUS_STAGEA);
	//LoadSoundObject("MusStage1B.wav", SOUND_ID_MUS_STAGEB);

    //MakeSurface_Generic(SURFACE_WIDTH, SURFACE_HEIGHT, SURFACE_ID_BACKUP);
	

	//Make generic surfaces
	//MakeSurface_Generic(SURFACE_WIDTH, SURFACE_HEIGHT, SURFACE_ID_BACKUP);
	//MakeSurface_Generic(272, 16, SURFACE_ID_TEXT0);
	//MakeSurface_Generic(272, 16, SURFACE_ID_TEXT1);
	//for (int i = 0; i < MAX_PSLINES; i++)
	//	MakeSurface_Generic(SURFACE_WIDTH, 16, i + SURFACE_ID_WORDS0);

	//Load sounds
	//InitSoundObject("NODMG", SOUND_ID_NODMG);
	//InitSoundObject("QUAKE", SOUND_ID_QUAKE);
	//InitSoundObject("BOSSOUCH", SOUND_ID_BOSSOUCH);
	//ChangeSoundFrequency(SOUND_ID_NODMG, 1200);
	//ChangeSoundFrequency(SOUND_ID_QUAKE, 700);
	//ChangeSoundFrequency(SOUND_ID_BOSSOUCH, 1200);
}