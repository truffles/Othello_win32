
/********************************************/
/* This header file is used by gameEngine.c */
/*                             mainUI.c     */
/********************************************/

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "gameAI.h"

/* Board status */
typedef struct tagBOARDSTATUS
{
	enum {WHITE = -1, EMPTY = 0, BLACK = 1, EDGE = 2} board[10][10];
	int  who;
} BOARD_ST;

/* Moves history for undo */
typedef struct tagMOVESHISTORY
{
	int      count;
	BOARD_ST *list[61];
} HISTORY_ST;

/* Function declarations */
/**************************************************/
void ResetBoard(void);
BOOL ProcessBoard(HWND hwnd, int x, int y);
BOOL MakeMoveByPos(int x, int y);
BOOL MakeMoveByIndex(MOVES_ST *movesTable, int index);
void SaveBoard(void);
void ChangeTurn(void);
BOOL UndoMove(void);
void SetUpLegalMoves(MOVES_ST *movesTable);
BOOL IsPass(void);
void ClearBoardHistory(void);
/**************************************************/

#endif // GAMEENGINE_H

