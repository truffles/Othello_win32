
/******************************************/
/* This header file is used by wndPaint.c */
/*                             mainUI.c   */
/******************************************/

#ifndef WNDPAINT_H
#define WNDPAINT_H

#define OFFSET_X 50
#define OFFSET_Y 50
#define GRID_WIDTH 50
#define HALF_GRID_WIDTH 25

void CalcClientSize(HWND hwnd);
BOOL LoadBoardTexture(HWND hwnd);
void DrawBoard(HWND hwnd);
void DrawGameInfo(HWND hwnd);
void CauseRedraw(HWND hwnd);
BOOL scrnToBoard(int *x, int *y);
void DestroyBoardTexture(void);

#endif // WNDPAINT_H

