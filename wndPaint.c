
/***********************************************/
/* Handle everything about window painting job */
/***********************************************/

#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "wndPaint.h"
#include "gameEngine.h"

int iToolHeight; // For window size control
int iBoardWidth = 2*OFFSET_X + 8*GRID_WIDTH;
int bShowLegalMoves = 0;
extern BOARD_ST *currentBoard;
extern MOVES_ST currentMovesTable;

HBITMAP hTexture = NULL;

inline void CalcClientSize(HWND hwnd)
{
	HWND hTool;
	RECT rcTool;
	
	hTool = GetDlgItem(hwnd, IDT_MAINTOOL);
	SendMessage(hTool, TB_AUTOSIZE, 0, 0);
	GetWindowRect(hTool, &rcTool);
	iToolHeight = rcTool.bottom - rcTool.top;
}

BOOL LoadBoardTexture(HWND hwnd)
{
	hTexture = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BOARDTEXTURE));
	if(hTexture == NULL)
	{
		MessageBox(hwnd, "Could not load board texture.", "Error",
			MB_ICONWARNING | MB_OK);
		return FALSE;
	}
	return TRUE;
}

void DrawBoard(HWND hwnd)
{
	BITMAP      bm;
	PAINTSTRUCT ps;
	RECT        rcWindow;
	int         iWindowHeight, iWindowWidth;
	
	HDC     hdc    = BeginPaint(hwnd, &ps);   // Actual screen DC
	HDC     hdcMem = CreateCompatibleDC(hdc); // DC in memory
	HBITMAP hbmOld = SelectObject(hdcMem, hTexture);
	HPEN    hPen, hPenOld;
	HBRUSH  hBrush, hBrushOld;
	SetViewportOrgEx(hdc, 0, iToolHeight, NULL);
	
	SetBkMode(hdc, TRANSPARENT);
	GetObject(hTexture, sizeof(bm), &bm);
	GetWindowRect(hwnd, &rcWindow);
	
	iWindowHeight = rcWindow.bottom - rcWindow.top;
	iWindowWidth  = rcWindow.right - rcWindow.left;
	
	for(int x = 0; x < iWindowWidth; x += bm.bmWidth)
		for(int y = 0; y < iWindowHeight; y += bm.bmHeight)
			BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY); 
	
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
	
	// Draw grid
	hPen = CreatePen(PS_SOLID, 3, RGB(50, 50, 50));
    hPenOld = SelectObject(hdc, hPen);
    
    char ch;
    
	for(int i = 0; i < 9; i++)
	{
		MoveToEx(hdc, OFFSET_X + i*GRID_WIDTH, OFFSET_Y, (LPPOINT)NULL); 
    	LineTo(hdc, OFFSET_X + i*GRID_WIDTH, OFFSET_Y + 8*GRID_WIDTH);
    	
    	MoveToEx(hdc, OFFSET_X, OFFSET_Y + i*GRID_WIDTH, (LPPOINT)NULL); 
    	LineTo(hdc, OFFSET_X + 8*GRID_WIDTH, OFFSET_Y + i*GRID_WIDTH);
	}
	
	// Draw board coordinate
	ch = 'A';
	SetTextAlign(hdc, TA_CENTER | TA_BOTTOM);
	for(int i = 0; i < 8; i++, ch++)
	{
		TextOut(hdc, OFFSET_X + HALF_GRID_WIDTH + i*GRID_WIDTH, OFFSET_Y, &ch, 1);
	}
	
	ch = '1';
	SetTextAlign(hdc, TA_RIGHT);
	for(int i = 0; i < 8; i++, ch++)
	{
		TextOut(hdc, OFFSET_X - 5, OFFSET_Y - 2 + HALF_GRID_WIDTH + i*GRID_WIDTH, &ch, 1);
	}
	
	
	if(currentBoard != NULL)
	{
	
		// Paint white pieces
		hPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		DeleteObject(SelectObject(hdc, hPen));
		hBrushOld = SelectObject(hdc, hBrush);
		
		for(int x = 1; x <= 8; x++)
			for(int y = 1; y <= 8; y++)
			{
				if(currentBoard->board[x][y] == WHITE)
					Ellipse(
						hdc,
						OFFSET_X + 4 + (x-1)*GRID_WIDTH,
						OFFSET_Y + 4 + (y-1)*GRID_WIDTH,
						OFFSET_X - 3 + x*GRID_WIDTH,
						OFFSET_Y - 3 + y*GRID_WIDTH);
			}
		
		
		// Paint black pieces
		hPen = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		DeleteObject(SelectObject(hdc, hPen));
		DeleteObject(SelectObject(hdc, hBrush));
		
		for(int x = 1; x <= 8; x++)
			for(int y = 1; y <= 8; y++)
			{
				if(currentBoard->board[x][y] == BLACK)
					Ellipse(
						hdc,
						OFFSET_X + 4 + (x-1)*GRID_WIDTH,
						OFFSET_Y + 4 + (y-1)*GRID_WIDTH,
						OFFSET_X - 3 + x*GRID_WIDTH,
						OFFSET_Y - 3 + y*GRID_WIDTH);
			}
		
		
		// Show legal points
		if (bShowLegalMoves) {
			hPen = CreatePen(PS_SOLID, 1, RGB(200, 0, 0));
			hBrush = CreateSolidBrush(RGB(200, 0, 0));
			DeleteObject(SelectObject(hdc, hPen));
			DeleteObject(SelectObject(hdc, hBrush));
			
			for(int i = 0; i < currentMovesTable.legalPosCount; i++)
			{
				Ellipse(
					hdc,
					OFFSET_X - HALF_GRID_WIDTH - 5 + currentMovesTable.diskTurned[i][0].x*GRID_WIDTH,
					OFFSET_Y - HALF_GRID_WIDTH - 5 + currentMovesTable.diskTurned[i][0].y*GRID_WIDTH,
					OFFSET_X - HALF_GRID_WIDTH + 5 + currentMovesTable.diskTurned[i][0].x*GRID_WIDTH,
					OFFSET_Y - HALF_GRID_WIDTH + 5 + currentMovesTable.diskTurned[i][0].y*GRID_WIDTH);
			}
		}
		
		SelectObject(hdc, hBrushOld);
	}
	
	SelectObject(hdc, hPenOld);
	EndPaint(hwnd, &ps);
}

void DrawGameInfo(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC    hdc = BeginPaint(hwnd, &ps);
	HPEN   hPen, hPenOld;
	HBRUSH hBrush, hBrushOld;
	
	hPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
	hBrush = CreateSolidBrush(RGB(0, 0, 0));
	hPenOld = SelectObject(hdc, hPen);
	hBrushOld = SelectObject(hdc, hBrush);
	Ellipse(hdc, 30, 20, 70, 60);
	
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	DeleteObject(SelectObject(hdc, hBrush));
	Ellipse(hdc, 30, 70, 70, 110);
	
	SelectObject(hdc, hPenOld);
	SelectObject(hdc, hBrushOld);
	
	DeleteObject(hPen);
	DeleteObject(hBrush);
	EndPaint(hwnd, &ps);
}

inline void CauseRedraw(HWND hwnd)
{
	RECT rcClient;
	HDC  hdc;
	
	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rcClient);
	rcClient.top = iToolHeight;
	rcClient.right = iBoardWidth;
	InvalidateRect(hwnd, &rcClient, FALSE);
	
	ReleaseDC(hwnd, hdc);
}

BOOL scrnToBoard(int *x, int *y)
{
	int bx = *x - OFFSET_X;
	int by = *y - OFFSET_Y - iToolHeight;
	
	if(bx < 0 || by < 0)
		return FALSE;
	
	bx /= (int)GRID_WIDTH;
	by /= (int)GRID_WIDTH;
	
	if(++bx > 8 || ++by > 8)
		return FALSE;
	
	*x = bx;
	*y = by;
	
	return TRUE;
}

inline void DestroyBoardTexture(void)
{
	DeleteObject(hTexture);
}

