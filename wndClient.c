
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "wndClient.h"
#include "wndPaint.h"
#include "gameEngine.h"

extern int iToolHeight; // For window size control
extern int iBoardWidth; // From wndPaint.c

HWND hGameInfoDlg = NULL;

HWND CreateMainToolbar(HWND hwndParent)
{
	HWND hTool;
	
	hTool = CreateWindowEx(
	        	0,
				TOOLBARCLASSNAME,
				NULL,
				WS_CHILD | WS_VISIBLE,
				0, 0, 0, 0,
				hwndParent,
				(HMENU)IDT_MAINTOOL,
				GetModuleHandle(NULL),
				NULL
				);
	
	
	
	SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	
	TBBUTTON tbb[7];
	TBADDBITMAP tbab;
	
	tbab.hInst = HINST_COMMCTRL;
	tbab.nID = IDB_STD_LARGE_COLOR;
	SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);
	
	ZeroMemory(tbb, sizeof(tbb));
	tbb[0].iBitmap   = STD_FILENEW;
	tbb[0].fsState   = TBSTATE_ENABLED;
	tbb[0].fsStyle   = TBSTYLE_BUTTON;
	tbb[0].idCommand = IDM_GAME_NEWGAME;
	tbb[0].iString   = (INT_PTR)"New Game";
	
	tbb[1].iBitmap   = STD_FILEOPEN;
	tbb[1].fsState   = TBSTATE_ENABLED;
	tbb[1].fsStyle   = TBSTYLE_BUTTON;
	tbb[1].idCommand = IDM_GAME_OPENFILE;
	tbb[1].iString   = (INT_PTR)"Open File";
	
	tbb[2].iBitmap   = STD_FILESAVE;
	tbb[2].fsState   = TBSTATE_ENABLED;
	tbb[2].fsStyle   = TBSTYLE_BUTTON;
	tbb[2].idCommand = IDM_GAME_SAVEGAME;
	tbb[2].iString   = (INT_PTR)"Save Game";
	
	tbb[3].fsStyle   = TBSTYLE_SEP;
	
	tbb[4].iBitmap   = STD_UNDO;
	tbb[4].fsState   = TBSTATE_INDETERMINATE;
	tbb[4].fsStyle   = TBSTYLE_BUTTON;
	tbb[4].idCommand = IDM_GAME_UNDOMOVE;
	tbb[4].iString   = (INT_PTR)"Undo Move";
	
	tbb[5].fsStyle   = TBSTYLE_SEP;
	
	tbb[6].iBitmap   = STD_PROPERTIES;
	tbb[6].fsState   = TBSTATE_ENABLED;
	tbb[6].fsStyle   = TBSTYLE_BUTTON;
	tbb[6].idCommand = IDM_GAME_SETTINGS;
	tbb[6].iString   = (INT_PTR)"Settings";
	
	SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);
	SendMessage(hTool, TB_AUTOSIZE, 0, 0); // Resize the toolbar
	
	return hTool;
}

BOOL CreateGameInfoDlg(HWND hwndParent)
{
	hGameInfoDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GAMEINFODLG),
	                   hwndParent, (DLGPROC)GameInfoProc);
	
	if(hGameInfoDlg == NULL)
	{
		MessageBox(hwndParent, "GameInfo child window creation failed!", "Error",
			MB_ICONERROR | MB_OK);
		
		return FALSE;
	}
	
	return TRUE;
}

void ShowGameInfoDlg(void)
{
	SetWindowPos(hGameInfoDlg, NULL, iBoardWidth, iToolHeight,
		800 - iBoardWidth, 600 - iToolHeight, SWP_NOZORDER);
	
	SetWindowPos(GetDlgItem(hGameInfoDlg, IDL_RECLISTBOX), NULL,
		20, 120, 750 - iBoardWidth, 350, SWP_NOZORDER);
	
	SetWindowPos(GetDlgItem(hGameInfoDlg, IDT_BLACKTEXT), NULL,
		80, 30, 60, 40, SWP_NOZORDER);
	
	SetWindowPos(GetDlgItem(hGameInfoDlg, IDT_WHITETEXT), NULL,
		80, 80, 60, 40, SWP_NOZORDER);
	
	ShowWindow(hGameInfoDlg, SW_SHOW);
}	

void DestroyChild(void)
{
	DestroyWindow(hGameInfoDlg);
}

BOOL CALLBACK GameInfoProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_PAINT:
			DrawGameInfo(hwnd);
			break;
		
		default:
			return FALSE;
	}
	return TRUE;
}

void RefreshCount(void)
{
	extern BOARD_ST *currentBoard;
	char string[3];
	int bCount = 0, wCount = 0;
	
	for(int x = 1; x <= 8; x++)
		for(int y = 1; y <= 8; y++)
		{
			if(currentBoard->board[x][y] == BLACK)
				bCount++;
			else if(currentBoard->board[x][y] == WHITE)
				wCount++;
		}
	
	sprintf(string, "%d", bCount);
	SendMessage(GetDlgItem(hGameInfoDlg, IDT_BLACKTEXT), WM_SETTEXT, 0, (LPARAM)string);
	
	sprintf(string, "%d", wCount);
	SendMessage(GetDlgItem(hGameInfoDlg, IDT_WHITETEXT), WM_SETTEXT, 0, (LPARAM)string);
}

inline void AddMoveToListbox(int x, int y, int who)
{
	char string[20];
	int index;
	
	if(who == BLACK)
		sprintf(string, "BLACK : %c%c", x-1 + 'A', y + '0');
	else if(who == WHITE)
		sprintf(string, "WHITE : %c%c", x-1 + 'A', y + '0');
	
	index = SendMessage(GetDlgItem(hGameInfoDlg, IDL_RECLISTBOX), LB_ADDSTRING, 0, (LPARAM)string);
	SendMessage(GetDlgItem(hGameInfoDlg, IDL_RECLISTBOX), LB_SETCURSEL, index, 0);
}

inline void DeleteMovesFromListbox(int count)
{
	int index;
	index = SendMessage(GetDlgItem(hGameInfoDlg, IDL_RECLISTBOX), LB_GETCOUNT, 0, 0) - 1;
	
	while(count > 0)
	{
		SendMessage(GetDlgItem(hGameInfoDlg, IDL_RECLISTBOX), LB_DELETESTRING, index--, 0);
		count--;
	}
	
	SendMessage(GetDlgItem(hGameInfoDlg, IDL_RECLISTBOX), LB_SETCURSEL, index, 0);
}

inline void DeleteAllMovesFromListbox(void)
{
	SendMessage(GetDlgItem(hGameInfoDlg, IDL_RECLISTBOX), LB_RESETCONTENT, 0, 0);
}

