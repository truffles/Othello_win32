
/************************/
/* Handle the game rule */
/************************/

#include <windows.h>
#include <time.h>
#include "gameEngine.h"
#include "gameAI.h"
#include "wndPaint.h"
#include "wndClient.h"

HISTORY_ST boardHistory;
BOARD_ST   *currentBoard = NULL;
MOVES_ST   currentMovesTable;
enum {TwoHuman, HumanFirst, ComputerFirst} gameMode = HumanFirst;
extern int totalDiskCount;

void ResetBoard(void)
{
	srand((unsigned)clock());
	ClearBoardHistory();
	DeleteAllMovesFromListbox();
	
	currentBoard = (BOARD_ST*)calloc(1, sizeof(BOARD_ST));
	
	ZeroMemory(&boardHistory, sizeof(boardHistory));
	
	/* Initialize chess placement */
	currentBoard->board[4][4] = currentBoard->board[5][5] = WHITE;
	currentBoard->board[4][5] = currentBoard->board[5][4] = BLACK;
	
	for(int i = 0; i <= 9; i++)
	{
		currentBoard->board[i][0] = currentBoard->board[i][9]
			= currentBoard->board[0][i] = currentBoard->board[9][i] = EDGE;
	}
	
	currentBoard->who = BLACK;
	boardHistory.list[boardHistory.count++] = currentBoard;
	SetUpLegalMoves(&currentMovesTable);
	
	if(gameMode == ComputerFirst) // Generate and apply a random move
	{
		int random = rand() % currentMovesTable.legalPosCount;
		
		MakeMoveByIndex(&currentMovesTable, random);
		AddMoveToListbox(
			currentMovesTable.diskTurned[random][0].x,
			currentMovesTable.diskTurned[random][0].y,
			currentBoard->who);
		ChangeTurn();
		SetUpLegalMoves(&currentMovesTable);
	}
	
	RefreshCount();
}

BOOL ProcessBoard(HWND hwnd, int x, int y) // return TRUE for legal moves
{
	if(!MakeMoveByPos(x, y))
		return FALSE;
	
	AddMoveToListbox(x, y, currentBoard->who);
	ChangeTurn();
	SetUpLegalMoves(&currentMovesTable);
	CauseRedraw(hwnd);
	DrawBoard(hwnd);
	
	if(IsPass())
	{
		ChangeTurn();
		SetUpLegalMoves(&currentMovesTable);
		
		if(IsPass())
		{
			if(gameMode == HumanFirst || gameMode == ComputerFirst)
			{
				int difference = AI_evalDiskCount();
				if(difference > 0)
					MessageBox(hwnd, "Game over! You win!", "Othello", MB_OK | MB_ICONINFORMATION);
				else if(difference == 0)
					MessageBox(hwnd, "Game over! Draw!", "Othello", MB_OK | MB_ICONINFORMATION);
				else
					MessageBox(hwnd, "Game over! You lose!", "Othello", MB_OK | MB_ICONINFORMATION);
			}
			else
				MessageBox(hwnd, "Game over!", "Othello", MB_OK | MB_ICONINFORMATION);
			
			CauseRedraw(hwnd);
			DrawBoard(hwnd);
			return TRUE;
		}
		
		if(currentBoard->who == BLACK)
			MessageBox(hwnd, "White pass!", "Othello", MB_OK | MB_ICONINFORMATION);
		else
			MessageBox(hwnd, "Black pass!", "Othello", MB_OK | MB_ICONINFORMATION);
		
		CauseRedraw(hwnd);
		DrawBoard(hwnd);
	}
	
	else if(gameMode == HumanFirst || gameMode == ComputerFirst)
	{
		for(;;)
		{
			int moveIndex;
			
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			
			totalDiskCount = boardHistory.count + 3;
			moveIndex = AI_Entrance();
			MakeMoveByIndex(&currentMovesTable, moveIndex);
			AddMoveToListbox(
				currentMovesTable.diskTurned[moveIndex][0].x,
				currentMovesTable.diskTurned[moveIndex][0].y,
				currentBoard->who);
			ChangeTurn();
			SetUpLegalMoves(&currentMovesTable);
			
			Sleep(200);
			
			RefreshCount();
			CauseRedraw(hwnd);
			DrawBoard(hwnd);
			
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			
			if(IsPass())
			{
				ChangeTurn();
				SetUpLegalMoves(&currentMovesTable);
				
				if(IsPass())
				{
					if(gameMode == HumanFirst || gameMode == ComputerFirst)
					{
						int difference = AI_evalDiskCount();
						if(difference < 0)
							MessageBox(hwnd, "Game over! You win!", "Othello", MB_OK | MB_ICONINFORMATION);
						else if(difference == 0)
							MessageBox(hwnd, "Game over! Draw!", "Othello", MB_OK | MB_ICONINFORMATION);
						else
							MessageBox(hwnd, "Game over! You lose!", "Othello", MB_OK | MB_ICONINFORMATION);
					}
					else
						MessageBox(hwnd, "Game over!", "Othello", MB_OK | MB_ICONINFORMATION);
					
					CauseRedraw(hwnd);
					DrawBoard(hwnd);
					return TRUE;
				}
				
				if(currentBoard->who == BLACK)
					MessageBox(hwnd, "White pass!", "Othello", MB_OK | MB_ICONINFORMATION);
				else
					MessageBox(hwnd, "Black pass!", "Othello", MB_OK | MB_ICONINFORMATION);
				
				CauseRedraw(hwnd);
				DrawBoard(hwnd);
			}
			
			else return TRUE;
		}
	}
	
	return TRUE;
}

BOOL MakeMoveByPos(int x, int y)
{
	for(int i = 0; i < currentMovesTable.legalPosCount; i++)
	{
		if(currentMovesTable.diskTurned[i][0].x == x && currentMovesTable.diskTurned[i][0].y == y)
		{
			SaveBoard();
			
			for(int j = 0; currentMovesTable.diskTurned[i][j].x; j++)
			{
				currentBoard->board[currentMovesTable.diskTurned[i][j].x][currentMovesTable.diskTurned[i][j].y]
					= currentBoard->who;
			}
			RefreshCount();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL MakeMoveByIndex(MOVES_ST *movesTable, int index)
{
	if(index >= movesTable->legalPosCount)
		return FALSE;
	
	SaveBoard();
	
	for(int i = 0; movesTable->diskTurned[index][i].x; i++)
	{
		currentBoard->board[movesTable->diskTurned[index][i].x][movesTable->diskTurned[index][i].y]
			= currentBoard->who;
	}
	RefreshCount();
	return TRUE;
}
 
inline void SaveBoard(void)
{
	currentBoard = (BOARD_ST*)malloc(sizeof(BOARD_ST));
	CopyMemory(currentBoard, boardHistory.list[boardHistory.count - 1], sizeof(BOARD_ST));
	boardHistory.list[boardHistory.count++] = currentBoard;
}

inline void ChangeTurn(void)
{
	currentBoard->who = -currentBoard->who;
}

BOOL UndoMove(void) // return TRUE or FALSE for undo buttons control
{
	switch(gameMode)
	{
		case TwoHuman:
			if(boardHistory.count > 1)
			{
				free(currentBoard);
				boardHistory.count--;
				currentBoard = boardHistory.list[boardHistory.count - 1];
				
				SetUpLegalMoves(&currentMovesTable);
				DeleteMovesFromListbox(1);
				RefreshCount();
				
				if(boardHistory.count > 1)
					return TRUE;
			}
			break;
		
		case HumanFirst:
			if(boardHistory.count > 2)
			{
				int index = boardHistory.count - 1;
				
				do {
					free(boardHistory.list[index--]);
				} while(index > 0 && boardHistory.list[index]->who == WHITE);
				
				DeleteMovesFromListbox(boardHistory.count - index - 1);
				boardHistory.count = index + 1;
				currentBoard = boardHistory.list[index];
				SetUpLegalMoves(&currentMovesTable);
				RefreshCount();
				
				if(boardHistory.count > 2)
					return TRUE;
			}
			break;
		
		case ComputerFirst:
			if(boardHistory.count > 2)
			{
				int index = boardHistory.count - 1;
				
				do {
					free(boardHistory.list[index--]);
				} while(index > 0 && boardHistory.list[index]->who == BLACK);
				
				DeleteMovesFromListbox(boardHistory.count - index - 1);
				boardHistory.count = index + 1;
				currentBoard = boardHistory.list[index];
				SetUpLegalMoves(&currentMovesTable);
				RefreshCount();
				
				if(boardHistory.count > 2)
					return TRUE;
			}
			break;
	}
	return FALSE;
}

void SetUpLegalMoves(MOVES_ST *movesTable)
{
	static const int xDirection[] = { 0, 1, 0,-1, 1,-1, 1,-1};
	static const int yDirection[] = { 1, 0,-1, 0, 1, 1,-1,-1};
	int flipCount = 0;
	int sx, sy;
	
	movesTable->legalPosCount = 0;
	
	for(int x = 1; x <= 8; x++)
	{
		for(int y = 1; y <=8; y++)
		{
			if(currentBoard->board[x][y] != EMPTY)
				continue;
			
			for(int i = 0; i < 8; i++)
			{
				sx = x + xDirection[i];
				sy = y + yDirection[i];
				
				if(currentBoard->board[sx][sy] != -currentBoard->who)
					continue;
				
				do {
					sx += xDirection[i];
					sy += yDirection[i];
				} while(currentBoard->board[sx][sy] == -currentBoard->who);
		
				if(currentBoard->board[sx][sy] != currentBoard->who)
					continue;
				
				if(!flipCount)
				{
					movesTable->diskTurned[movesTable->legalPosCount][flipCount].x = x;
					movesTable->diskTurned[movesTable->legalPosCount][flipCount++].y = y;
				}
				
				sx = x + xDirection[i];
				sy = y + yDirection[i];
		
				do {
					movesTable->diskTurned[movesTable->legalPosCount][flipCount].x = sx;
					movesTable->diskTurned[movesTable->legalPosCount][flipCount++].y = sy;
					sx += xDirection[i];
					sy += yDirection[i];
				} while(currentBoard->board[sx][sy] == -currentBoard->who);
			}
			
			if(flipCount)
			{
				movesTable->diskTurned[movesTable->legalPosCount][flipCount].x = 0;
				movesTable->diskTurned[movesTable->legalPosCount][flipCount].y = 0;
				movesTable->legalPosCount++;
			}
			
			flipCount = 0;
		}
	}
}

inline BOOL IsPass(void)
{
	return currentMovesTable.legalPosCount <= 0;
}

inline void ClearBoardHistory(void)
{
	for(int i = 0; i < boardHistory.count; i++)
		free(boardHistory.list[i]);
	
	boardHistory.count = 0;
}

