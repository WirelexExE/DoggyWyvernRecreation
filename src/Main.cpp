#include "Game.h"
#include "Main.h"
#include "Draw.h"
#include "Sound.h"
#include "Generic.h"
#include <stdio.h>

//Keys
DWORD gKey;

//Window name
LPCTSTR lpCaption = "DX_Game";

//Windows objects
HWND ghWnd;
HACCEL hAccel;

//Window size
WND_SIZE gWndSize;
int gWndWidth, gWndHeight;

//Filepath
TCHAR gModulePath[MAX_PATH];

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	LPCTSTR lpMutexName = "DX_Game";
	RECT unused_rect = { 0, 0, SURFACE_WIDTH, SURFACE_HEIGHT };

	//Create Ikachan mutex (prevent program from running multiple instances)
	HANDLE hObject, hMutex;
	if ((hObject = OpenMutex(MUTEX_ALL_ACCESS, FALSE, lpMutexName)) != NULL)
	{
		CloseHandle(hObject);
		return 1;
	}
	hMutex = CreateMutex(0, TRUE, lpMutexName);

	//Get module path
	size_t i;
	GetModuleFileName(NULL, gModulePath, MAX_PATH);
	for (i = strlen(gModulePath); gModulePath[i] != '\\'; i--);
	gModulePath[i] = 0;
	
    //Define window class
    WNDCLASS wndClass;
    memset(&wndClass, 0, sizeof(WNDCLASS));
    wndClass.lpfnWndProc = WndProc;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = (HICON)LoadImage(hInstance, "0", IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //DKGRAY_BRUSH
    wndClass.lpszClassName = lpCaption;

    //Create window (full screen or windowed)
    HWND hWnd;

    gWndSize = WS_WINDOWED;
	//gWndSize = WS_FULLSCREEN;
    switch (gWndSize)
    {
        case WS_WINDOWED:
        {
            //Register class
            wndClass.lpszMenuName = "MENU_MAIN";
            if (!RegisterClass(&wndClass))
            {
                //Release Ikachan mutex
                ReleaseMutex(hMutex);
                return 0;
            }

            //Get window dimensions
			gWndWidth = SURFACE_WIDTH;
			gWndHeight = SURFACE_HEIGHT;
            
            // Dumb Hack for modern windows
            RECT windowRect;
            int nWidth, nHeight, X, Y;
            static DWORD dumbhack_window = (WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER | WS_DLGFRAME | WS_VISIBLE);
            static DWORD dumbhack_window2 = (WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);
            windowRect.left = 0;
			windowRect.top = 0;
			windowRect.right = gWndWidth;
			windowRect.bottom = gWndHeight;

			AdjustWindowRectEx(&windowRect, dumbhack_window, FALSE, dumbhack_window2);

			nWidth = windowRect.right - windowRect.left;
			nHeight = windowRect.bottom - windowRect.top;

			X = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
			Y = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;

			SetClientOffset(GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER) + (GetSystemMetrics(SM_CXPADDEDBORDER) == 0 ? 0 : 1), GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER) + (GetSystemMetrics(SM_CXPADDEDBORDER) == 0 ? 0 : 1));

			hWnd = CreateWindowExA(dumbhack_window2, lpCaption, lpCaption, dumbhack_window, X, Y, nWidth, nHeight, NULL, NULL, hInstance, NULL);
            

            ghWnd = hWnd;
            if (hWnd == NULL)
            {
                //Release Ikachan mutex
                ReleaseMutex(hMutex);
                return 0;
            }
			
            //Start DirectDraw
			if (StartDirectDraw(hWnd, WS_WINDOWED) == FALSE)
			{
				MessageBox(NULL, "StartDirectDraw Error", "Message", 0);
				return 0;
			}

                
            break;
        }
        case WS_FULLSCREEN:
        {
            //Register class
            if (!RegisterClass(&wndClass))
            {
                //Release Ikachan mutex
                ReleaseMutex(hMutex);
                return 0;
            }

            //Get window dimensions
            gWndWidth = SURFACE_WIDTH;
            gWndHeight = SURFACE_HEIGHT;

            //Get window area
            SetClientOffset(0, 0);

            //Create window
            hWnd = CreateWindowEx(NULL, lpCaption, lpCaption, WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
            
            ghWnd = hWnd;
            if (hWnd == NULL)
            {
                //Release Ikachan mutex
                ReleaseMutex(hMutex);
                return 0;
            }

            //Set cursor position
            StartDirectDraw(hWnd, WS_FULLSCREEN);
            SetCursorPos(gWndWidth / 2, gWndHeight / 2);

            //Start DirectDraw
            SetCursorPos(0, 0);
            
            ShowCursor(FALSE);
            break;
        }
    }

    //Set rects
    RECT rcFull = { 0, 0, 0, 0 };
    rcFull.right = SURFACE_WIDTH;
    rcFull.bottom = SURFACE_HEIGHT;
    CortBox(&rcFull, 0x000000);
    
    if (!Flip_SystemTask(hWnd))
    {
        //Release Ikachan mutex
        ReleaseMutex(hMutex);
        return 1;
    }
    
    InitTextObject(NULL);
	InitDirectSound(hWnd);
    Game(hWnd);
	EndDirectSound();
    EndDirectDraw(hWnd);
    EndTextObject();
    
	//Release Ikachan mutex
	ReleaseMutex(hMutex);
	return 1;
}

//Window process
BOOL bActive;

void InactiveWindow()
{
	bActive = FALSE;
}

void ActiveWindow()
{
	bActive = TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    BOOL window_focus;
	switch (Msg)
	{
		case WM_CREATE:
            //DrawMenuBar(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case 40001: // END
					PostMessage(hWnd, WM_DESTROY, 0, 0);
					break;
				case 40002: // MEMO
				{
					int year, month, day;
					int v1, v2, v3, v4;
					TCHAR text[0x40];

					GetCompileDate(&year, &month, &day);
					GetCompileVersion(&v1, &v2, &v3, &v4);
					sprintf(text, "version.%d.%d.%d\n1999/04/XX - %04d/%02d/%02d\nby \x8A\x4A\x94\xAD\x8E\xBA\x82\x6Fixel", v1, v2, v3, year, month, day);
					MessageBoxA(hWnd, text, lpCaption, MB_OK);
					break;
				}
				case 40004:
					ShowWindow(hWnd, SW_MINIMIZE);
					break;
			}
			break;

		case WM_SYSCOMMAND:
			switch (wParam)
			{
				case SC_MONITORPOWER:
					break;
				case SC_KEYMENU:
					break;
				case SC_SCREENSAVE:
					break;
				default:
					DefWindowProc(hWnd, Msg, wParam, lParam);
					break;
			}
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				case VK_LEFT:
					gKey |= KEY_LEFT;
					break;
				case VK_RIGHT:
					gKey |= KEY_RIGHT;
					break;
				case VK_UP:
					gKey |= KEY_UP;
					break;
				case VK_DOWN:
					gKey |= KEY_DOWN;
					break;
				case VK_SPACE:
					gKey |= KEY_SPACE;
					break;
				case 'X':
					gKey |= KEY_X;
					break;
				case 'Z':
					gKey |= KEY_Z;
					break;
				case 'S':
					gKey |= KEY_S;
					break;
			}
			break;

		case WM_KEYUP:
			switch (wParam)
			{
				case VK_LEFT:
					gKey &= ~KEY_LEFT;
					break;
				case VK_RIGHT:
					gKey &= ~KEY_RIGHT;
					break;
				case VK_UP:
					gKey &= ~KEY_UP;
					break;
				case VK_DOWN:
					gKey &= ~KEY_DOWN;
					break;
				case VK_SPACE:
					gKey &= ~KEY_SPACE;
					break;
				case 'X':
					gKey &= ~KEY_X;
					break;
				case 'Z':
					gKey &= ~KEY_Z;
					break;
				case 'S':
					gKey &= ~KEY_S;
					break;
			}
			break;

		case WM_IME_NOTIFY:
			if (wParam == IMN_SETOPENSTATUS)
			{
				HIMC hImc = ImmGetContext(hWnd);
				ImmSetOpenStatus(hImc, 0);
				ImmReleaseContext(hWnd, hImc);
			}
			break;
			
        case WM_ACTIVATE:
			switch (LOWORD(wParam))
			{
				case WA_INACTIVE:
					window_focus = FALSE;
					break;

				case WA_ACTIVE:
				case WA_CLICKACTIVE:
					if (HIWORD(wParam) != 0)
						window_focus = FALSE;
					else
						window_focus = TRUE;

					break;
			}

			if (window_focus)
				ActiveWindow();
			else
				InactiveWindow();

			break;
			
		default:
			return DefWindowProcA(hWnd, Msg, wParam, lParam);
	}
	return TRUE;
}

//System task
BOOL SystemTask()
{
	MSG Msg;
	while (PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE) || !bActive)
	{
		if (!GetMessage(&Msg, NULL, 0, 0))
			return FALSE;
		TranslateMessage(&Msg);
		DispatchMessageA(&Msg);
	}
	return TRUE;
}
