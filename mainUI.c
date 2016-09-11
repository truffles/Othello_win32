#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "dialog.h"
#include "gameEngine.h"
#include "mainUI.h"
#include "resource.h"
#include "wndClient.h"
#include "wndPaint.h"

const char *szClassName = "MainWindowClass";

// The Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// message handler
	switch(msg)
	{
		case WM_SHOWWINDOW:
			ResetBoard();
			SendMessage(hwnd, WM_COMMAND, IDM_GAME_NEWGAME, 0);
			break;
			
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDM_GAME_NEWGAME:
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GAMETYPEDLG),
						hwnd, (DLGPROC)GameTypeDlgProc);
					ResetBoard();
					EnableMenuItem(GetMenu(hwnd), IDM_GAME_UNDOMOVE, MF_BYCOMMAND | MF_GRAYED);
					SendMessage(GetDlgItem(hwnd, IDT_MAINTOOL), TB_SETSTATE, IDM_GAME_UNDOMOVE, TBSTATE_INDETERMINATE);
					CauseRedraw(hwnd);
					break;
				
				case IDM_GAME_UNDOMOVE:
					if(UndoMove())
					{
						EnableMenuItem(GetMenu(hwnd), IDM_GAME_UNDOMOVE, MF_BYCOMMAND | MF_ENABLED);
						SendMessage(GetDlgItem(hwnd, IDT_MAINTOOL), TB_SETSTATE, IDM_GAME_UNDOMOVE, TBSTATE_ENABLED);
					}
					else
					{
						EnableMenuItem(GetMenu(hwnd), IDM_GAME_UNDOMOVE, MF_BYCOMMAND | MF_GRAYED);
						SendMessage(GetDlgItem(hwnd, IDT_MAINTOOL), TB_SETSTATE, IDM_GAME_UNDOMOVE, TBSTATE_INDETERMINATE);
					}
					
					CauseRedraw(hwnd);
					break;
				
				case IDM_GAME_SHOWLEGAL: {
					extern int bShowLegalMoves;
					bShowLegalMoves = ! bShowLegalMoves;
					if (bShowLegalMoves) {
						CheckMenuItem(GetMenu(hwnd), IDM_GAME_SHOWLEGAL, MF_BYCOMMAND | MF_CHECKED);
					} else {
						CheckMenuItem(GetMenu(hwnd), IDM_GAME_SHOWLEGAL, MF_BYCOMMAND | MF_UNCHECKED);
					}
					CauseRedraw(hwnd);
					break;
				}
				
				case IDM_GAME_SETTINGS:
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGSDLG),
						hwnd, (DLGPROC)SettingsDlgProc);
					break;
				
				case IDM_GAME_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				
				case IDM_HELP_ABOUT:
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTDLG),
						hwnd, (DLGPROC)AboutDlgProc);
					break;
				
				default:
					MessageBox(hwnd, "Not implemented yet.", "Sorry", MB_ICONWARNING | MB_OK);
					break;
			}
			break;
			
		
		case WM_SIZE:
			CalcClientSize(hwnd);
			ShowGameInfoDlg();
			break;
			
		case WM_PAINT:
			DrawBoard(hwnd);
			break;
			
		case WM_LBUTTONDOWN:
		{
			int xPos, yPos;
			
			xPos = GET_X_LPARAM(lParam);
			yPos = GET_Y_LPARAM(lParam);
			if(scrnToBoard(&xPos, &yPos))
			{
				
				if(ProcessBoard(hwnd, xPos, yPos))
				{
					EnableMenuItem(GetMenu(hwnd), IDM_GAME_UNDOMOVE, MF_BYCOMMAND | MF_ENABLED);
					SendMessage(GetDlgItem(hwnd, IDT_MAINTOOL), TB_SETSTATE, IDM_GAME_UNDOMOVE, TBSTATE_ENABLED);
				}
				
			}
		
		}
		break;
		
		case WM_RBUTTONDOWN:
			{
				char str[30];
				sprintf(str, "Your Current Score: %d", AI_getEvaluate());
				
				MessageBox(hwnd, str, "Static Evaluation", MB_ICONINFORMATION | MB_OK);
			}
			break;
			
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
			
		case WM_DESTROY:
			ClearBoardHistory();
			DestroyBoardTexture();
			DestroyChild();
			PostQuitMessage(0);
			break;
			
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

// The WinMain Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;      // The windowclass data structure
	HWND hwnd;          // This is the handle for the window
	MSG Msg;            // Messages to the application are saved here
	HACCEL haccel;      // Accelerator Table handler
	
	// Register the Window Class
	wc.cbSize         = sizeof(WNDCLASSEX);                 // The size of the WINDOWCLASSEX structure
	wc.style          = 0;                                  // Class style (CS_*)
	wc.lpfnWndProc    = WndProc;                            // Pointer to the window procedure for this window class
	wc.cbClsExtra     = 0;                                  // Amount of extra data allocated for this class in memory
	wc.cbWndExtra     = 0;                                  // Amount of extra data allocated in memory per window of this type
	wc.hInstance      = hInstance;                          // Handle to application instance
	wc.hIcon          = LoadIcon(hInstance,                 // Large (usually 32x32) icon shown when the user presses Alt+Tab
	                    	MAKEINTRESOURCE(IDI_MAINICON));
	wc.hIconSm        = (HICON)LoadImage(hInstance,         // Small (usually 16x16) icon shown in the top left corner of the window
	                    	MAKEINTRESOURCE(IDI_MAINICON),
	                    	IMAGE_ICON, 16, 16,
	                    	LR_DEFAULTCOLOR);
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);        // Cursor that will be displayed over our window
	wc.hbrBackground  = (HBRUSH)COLOR_WINDOWFRAME;          // Background Brush to set the color of our window
	wc.lpszMenuName   = MAKEINTRESOURCE(IDM_MAINMENU);      // Name of a menu resource to use for the windows with this class
	wc.lpszClassName  = szClassName;                        // Name to identify the class with
	
	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	
	// Create the Window
	hwnd = CreateWindowEx(
	       0,                                        // extended window style
	       szClassName,                              // Classname
	       "Win32 API Othello (Testing Ver.)",       // Title Text
	       WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, // window style
	       CW_USEDEFAULT,                            // window x position on the screen
		   CW_USEDEFAULT,                            // window y position on the screen
		   800,                                      // The programs width in pixels
		   600,                                      // The programs height in pixels
	       NULL,                                     // parent window handler
		   NULL,                                     // menu handler
		   hInstance,                                // Program Instance Handler
		   NULL                                      // pointer window creation data
		   );
	       
	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	
	// Create the main toolbar
	CreateMainToolbar(hwnd);
	
	if(!LoadBoardTexture(hwnd))
		PostQuitMessage(0);
			
	if(!CreateGameInfoDlg(hwnd))
		PostQuitMessage(0);
	
	// Show the window
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	
	// Load accelerators from resource file
	haccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_MENUACCEL));
	
	// The Message Loop
	while(GetMessage(&Msg, (HWND)NULL, 0, 0) > 0)
	{
		if(!TranslateAccelerator(hwnd, haccel, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	return Msg.wParam;
}

