#pragma once
#include <windows.h>
#include "Main.h"

#define SURFACE_WIDTH  (640)
#define SURFACE_HEIGHT (480)

enum SURFACE_ID
{
    SURFACE_ID_TITLE = 1,
    SURFACE_ID_GAMEINTERFACE,
    SURFACE_ID_GAMESTATUS,
    SURFACE_ID_DOGGY,
    SURFACE_ID_ITEMS,
    SURFACE_ID_OBSTACLES,
    SURFACE_ID_STAR,
    SURFACE_ID_ENEMY1,
    SURFACE_ID_ENEMY2,
    SURFACE_ID_BUBBLES,
    // 10 Layers of Backgrounds
    SURFACE_ID_BACK0,
    SURFACE_ID_BACK1,
    SURFACE_ID_BACK2,
    SURFACE_ID_BACK3,
    SURFACE_ID_BACK4,
    SURFACE_ID_BACK5,
    SURFACE_ID_BACK6,
    SURFACE_ID_BACK7,
    SURFACE_ID_BACK8,
    SURFACE_ID_BACK9,
    SURFACE_ID_BACKUP,
};

extern RECT grcFull;

void SetClientOffset(int width, int height);
BOOL Flip_SystemTask(HWND hWnd);
BOOL StartDirectDraw(HWND hWnd, int wndSize);
void EndDirectDraw(HWND hWnd);
BOOL MakeSurface_File(LPCTSTR name, int surf_no);
BOOL MakeSurface_Resource(LPCTSTR name, int surf_no);
BOOL MakeSurface_Generic(int bxsize, int bysize, int surf_no);
BOOL ReloadSurface_File(LPCTSTR name, int surf_no);
void BackupSurface(int surf_no, const RECT *rect);
void PutBitmap3(const RECT *rcView, int x, int y, const RECT *rect, int surf_no);
void CortBox(const RECT *rect, DWORD col);
void CortBox2(const RECT *rect, DWORD col, int surf_no);
void InitTextObject(LPCTSTR name);
void PutText(int x, int y, LPCTSTR text, DWORD color, BOOL unk);
void PutText2(int x, int y, LPCTSTR text, DWORD color, int surf_no, BOOL unk);
void EndTextObject();
void PutNumber(int x, int y, int no);