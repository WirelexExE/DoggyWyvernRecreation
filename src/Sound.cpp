#include "Sound.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIRECTSOUND_VERSION 0x500
#include <dsound.h>

#include "windows.h"

#include "Main.h"

LPDIRECTSOUND       lpDS;         
LPDIRECTSOUNDBUFFER lpPRIMARYBUFFER; 
LPDIRECTSOUNDBUFFER lpSECONDARYBUFFER[SE_MAX];

BOOL InitDirectSound(HWND hwnd)
{
	int i;
	DSBUFFERDESC dsbd;

	if (DirectSoundCreate(NULL, &lpDS, NULL) != DS_OK)
	{
		lpDS = NULL;
		return FALSE;
	}

	lpDS->SetCooperativeLevel(hwnd, DSSCL_EXCLUSIVE);

	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	lpDS->CreateSoundBuffer(&dsbd, &lpPRIMARYBUFFER, NULL);

	for (i = 0; i < SE_MAX; i++)
		lpSECONDARYBUFFER[i] = NULL;

	return TRUE;
}

void EndDirectSound(void)
{
	int i;

	if (lpDS == NULL)
		return;

	for (i = 0; i < SE_MAX; i++)
		if (lpSECONDARYBUFFER[i] != NULL)
			lpSECONDARYBUFFER[i]->Release();

	if (lpPRIMARYBUFFER != NULL)
		lpPRIMARYBUFFER->Release();

	if (lpDS != NULL)
		lpDS->Release();

	lpDS = NULL;
}

BOOL InitSoundObject(LPCSTR resname, int no)
{
	HRSRC hrscr;
	DSBUFFERDESC dsbd;
	DWORD* lpdword;	

	if (lpDS == NULL)
		return TRUE;

	if ((hrscr = FindResourceA(NULL, resname, "WAVE")) == NULL)
		return FALSE;

	lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));

	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)lpdword + 0x36);
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(lpdword + 5);

	if (lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no], NULL) != DS_OK)
		return FALSE;

	LPVOID lpbuf1, lpbuf2;
	DWORD dwbuf1, dwbuf2;

	lpSECONDARYBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)lpdword + 0x36), &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);

	CopyMemory(lpbuf1, (BYTE*)lpdword + 0x3A, dwbuf1);

	if (dwbuf2 != 0)
		CopyMemory(lpbuf2, (BYTE*)lpdword + 0x3A + dwbuf1, dwbuf2);

	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

	return TRUE;
}

BOOL LoadSoundObject(LPCSTR name, int no)
{
	//Get path
	char path[MAX_PATH];
	sprintf(path, "%s\\%s", gModulePath, name);

	DWORD file_size = 0;
	FILE* fp;
	HANDLE hFile;

	if (lpDS == NULL)
		return TRUE;

	hFile = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	file_size = GetFileSize(hFile, NULL);
	CloseHandle(hFile);

	if ((fp = fopen(path, "rb")) == NULL)
		return FALSE;

	DWORD* wp;
	wp = (DWORD*)malloc(file_size);

	fseek(fp, 0, SEEK_SET);

	for (int i = 0; i < file_size; i++)
		fread((BYTE*)wp + i, sizeof(BYTE), 1, fp);	// Pixel, stahp

	fclose(fp);

	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)wp + 0x36);
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(wp + 5);

	if (lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no], NULL) != DS_OK)
		return FALSE;

	LPVOID lpbuf1, lpbuf2;
	DWORD dwbuf1, dwbuf2;

	lpSECONDARYBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)wp + 0x36), &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);

	CopyMemory(lpbuf1, (BYTE*)wp + 0x3A, dwbuf1);

	if (dwbuf2 != 0)
		CopyMemory(lpbuf2, (BYTE*)wp + 0x3A + dwbuf1, dwbuf2);

	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

	free(wp);

	return TRUE;
}

void PlaySoundObject(int no, int mode)
{
	if (lpDS == NULL)
		return;

	if (lpSECONDARYBUFFER[no] != NULL)
	{
		switch (mode) {
			case SOUND_MODE_STOP:
				lpSECONDARYBUFFER[no]->Stop();
			break;
			case SOUND_MODE_PLAY:
				lpSECONDARYBUFFER[no]->Stop();
				lpSECONDARYBUFFER[no]->SetCurrentPosition(0);
				lpSECONDARYBUFFER[no]->Play(0, 0, 0);
			break;
			case SOUND_MODE_LOOP:
				lpSECONDARYBUFFER[no]->Stop();
				lpSECONDARYBUFFER[no]->SetCurrentPosition(0);
				lpSECONDARYBUFFER[no]->Play(0, 0, DSBPLAY_LOOPING);
			break;
		}
	}
}

bool IsSoundObjectPlaying(int no)
{
	if (lpDS == NULL)
		return false;

	if (lpSECONDARYBUFFER[no] != NULL)
	{
		DWORD dwStatus = 0;
		lpSECONDARYBUFFER[no]->GetStatus(&dwStatus);
		if (dwStatus & DSBSTATUS_PLAYING)
			return true;
		else
			return false;
	}
	return false;
}

void ChangeSoundFrequency(int no, DWORD rate)
{
	if (lpDS == NULL)
		return;

	lpSECONDARYBUFFER[no]->SetFrequency((rate * 10) + 100);
}

void ChangeSoundVolume(int no, long volume)
{
	if (lpDS == NULL)
		return;

	lpSECONDARYBUFFER[no]->SetVolume((volume - 300) * 8);
}

void ChangeSoundPan(int no, long pan)
{
	if (lpDS == NULL)
		return;

	lpSECONDARYBUFFER[no]->SetPan((pan - 256) * 10);
}