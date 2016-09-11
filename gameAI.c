
/********************************************************/
/* Do the best move decision base on a set of algorithm */
/********************************************************/

#include <windows.h> 
#include "gameEngine.h"
#include "gameAI.h"

extern HISTORY_ST boardHistory;
extern BOARD_ST   *currentBoard;
extern enum {TwoHuman, HumanFirst, ComputerFirst} gameMode;
BOOL   movesOrdering = FALSE;
int    aiSearchDepth = 7;
int    aiEndGameSearch = 14;
int    totalDiskCount = 0;


void MergeSort(int n, int *A, int *Aindex)
{
	BOOL swapStatus = FALSE;
	int B[n];
	int Bindex[n];
	
	for(int width = 1; width < n; width <<= 1)
	{
		for(int i = 0; i < n; i += width << 1)
		{
			if(swapStatus)
				BottomUpMerge(B, Bindex, i, MIN(i+width, n), MIN(i+width*2, n), A, Aindex);
			else
				BottomUpMerge(A, Aindex, i, MIN(i+width, n), MIN(i+width*2, n), B, Bindex);
		}
		swapStatus = !swapStatus;
	}
	if(swapStatus)
	{
		CopyMemory(Aindex, Bindex, sizeof(Bindex));
		CopyMemory(A, B, sizeof(int) * n);
	}
}

void BottomUpMerge(int *A, int *Aindex, int iLeft, int iRight, int iEnd, int *B, int *Bindex)
{
	int i0 = iLeft;
	int i1 = iRight;
	
	for(int j = iLeft; j < iEnd; j++)
	{
		if(i0 < iRight && (i1 >= iEnd || A[i0] <= A[i1]))
		{
			Bindex[j] = Aindex[i0];
			B[j] = A[i0++];
		}
		else
		{
			Bindex[j] = Aindex[i1];
			B[j] = A[i1++];
		}
	}
}


int AI_Entrance(void)
{
	int bestScore = -INF;
	int bestMove  = 0;
	int alpha = -INF, beta = INF;
	int depth = (totalDiskCount + aiEndGameSearch >= 64)? 60: aiSearchDepth;
	MOVES_ST movesTable;
	SetUpLegalMoves(&movesTable);
	
	// if there's only one move available, just do it!
	if(movesTable.legalPosCount == 1)
		return 0;
	
	int order[40] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
		             21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
	
	if(depth >= 4)
	{
		// Order all possible moves by rough searching
		AI_OrderMoves(&movesTable, order);
	}
	
	// for each ordered legal moves...
	for(int i = 0; i < movesTable.legalPosCount; i++)
	{
		AI_MakeMove(&movesTable, order[i]); // make such move!
		
		if(bestScore == alpha) // if this is an alpha node...
		{
			ChangeTurn();
			int score = -AI_PVS(-alpha-1, -alpha, depth-1, 0); // Do null-window search
			ChangeTurn();
			
			if(score <= alpha) // if there's no better moves under this node
			{
				AI_UndoMove(&movesTable, order[i]);
				continue; // skip this node
			}
			
			if(score >= beta) // if this node is expected good enough
			{
				AI_UndoMove(&movesTable, order[i]);
				return order[i]; // cut off
			}
			
			bestScore = alpha = score; // update best score, do further search
			bestMove = order[i];
		}
		
		ChangeTurn();
		int score = -AI_PVS(-beta, -alpha, depth-1, 0); // normal search
		ChangeTurn();
		AI_UndoMove(&movesTable, order[i]);
		
		if(score > bestScore) // if this node is the best so far
		{
			bestScore = score; // update best score
			bestMove = order[i];
			
			if(score > alpha)
			{
				if(score >= beta) // this is the best move
					return order[i]; // return its score
				
				alpha = score; // update alpha
			}
		}
	}
	return bestMove;
}

inline void AI_MakeMove(const MOVES_ST *movesTable, int index)
{
	for(int i = 0; movesTable->diskTurned[index][i].x; i++)
	{
		currentBoard->board[movesTable->diskTurned[index][i].x][movesTable->diskTurned[index][i].y]
			= currentBoard->who;
	}
	totalDiskCount++;
}

inline void AI_UndoMove(const MOVES_ST *movesTable, int index)
{
	currentBoard->board[movesTable->diskTurned[index][0].x][movesTable->diskTurned[index][0].y]
		= EMPTY;
	
	for(int i = 1; movesTable->diskTurned[index][i].x; i++)
	{
		currentBoard->board[movesTable->diskTurned[index][i].x][movesTable->diskTurned[index][i].y]
			= -currentBoard->who;
	}
	totalDiskCount--;
}

void AI_OrderMoves(MOVES_ST *movesTable, int *order)
{
	int scoreTable[40];
	
	movesOrdering = TRUE;
	
	for(int i = 0; i < movesTable->legalPosCount; i++)
	{
		AI_MakeMove(movesTable, i);
		ChangeTurn();
		scoreTable[i] = AI_PVS(-INF, INF, 1, 0);
		ChangeTurn();
		AI_UndoMove(movesTable, i);
	}
	
	movesOrdering = FALSE;
	
	MergeSort(movesTable->legalPosCount, scoreTable, order);
}

int AI_PVS(int alpha, int beta, int depth, int passed)
{
	if(depth <= 0)
		return AI_getEvaluate();
	
	int bestScore = -INF;
	MOVES_ST movesTable;
	SetUpLegalMoves(&movesTable);
	
	if(movesTable.legalPosCount <= 0)
	{
		if(passed) // end of the game
			return AI_getExact();
		
		// pass
		ChangeTurn();
		bestScore = -AI_PVS(-beta, -alpha, depth, 1); // normal search
		ChangeTurn();
		
		return bestScore;
	}
	
	int order[40] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
		             21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};
	
	if(depth >= 4 && !movesOrdering)
	{
		// Order all possible moves by rough searching
		AI_OrderMoves(&movesTable, order);
	}
	
	// for each ordered legal moves...
	for(int i = 0; i < movesTable.legalPosCount; i++)
	{
		AI_MakeMove(&movesTable, order[i]); // make such move!
		
		if(bestScore == alpha) // if this is an alpha node...
		{
			ChangeTurn();
			int score = -AI_PVS(-alpha-1, -alpha, depth-1, 0); // Do null-window search
			ChangeTurn();
			
			if(score <= alpha) // if there's no better moves under this node
			{
				AI_UndoMove(&movesTable, order[i]);
				continue; // skip this node
			}
			
			if(score >= beta) // if this node is expected good enough
			{
				AI_UndoMove(&movesTable, order[i]);
				return score; // cut off
			}
			
			bestScore = alpha = score; // update best score, do further search
		}
		
		ChangeTurn();
		int score = -AI_PVS(-beta, -alpha, depth-1, 0); // normal search
		ChangeTurn();
		AI_UndoMove(&movesTable, order[i]);
		
		if(score > bestScore) // if this node is the best so far
		{
			bestScore = score; // update best score
			
			if(score > alpha)
			{
				if(score >= beta) // this is the best move
					return score; // return its score
				
				alpha = score; // update alpha
			}
		}
	}
	
	return bestScore;
}

inline int AI_getEvaluate(void)
{
	return  AI_evalDiskCount()
	      + AI_evalCorner() * AI_varWeightUpDown()
	      + (AI_evalMobility(currentBoard->who) - AI_evalMobility(-currentBoard->who)) * AI_varWeightInc()
	      + (AI_evalPMobility(currentBoard->who) - AI_evalPMobility(-currentBoard->who)) * AI_varWeightDec();
}

inline int AI_getExact(void)
{
	int difference = AI_evalDiskCount();
	
	if(!AI_evalMobility(-currentBoard->who) && !AI_evalMobility(currentBoard->who))
	{
		if(difference > 0)
			return WIN - totalDiskCount;
		
		else if(difference < 0)
			return LOSE + totalDiskCount;
		
		else
			return 0;
	}
	
	else
	{
		if(difference > 0)
			return ENDGAME + difference;
		
		else if(difference < 0)
			return -ENDGAME + difference;
		
		else
			return 0;
	}
}

int AI_evalCorner(void)
{
	int score = 0;
	static const POS_ST corner[4]      = {{1,1}, {1,8}, {8,1}, {8,8}};
	static const POS_ST x_corner[4]    = {{2,2}, {2,7}, {7,2}, {7,7}};
	static const POS_ST c_corner[4][2] = {{{1,2}, {2,1}},  // {1,1}
	                                      {{1,7}, {2,8}},  // {1,8}
										  {{7,1}, {8,2}},  // {8,1}
										  {{7,7}, {8,8}}}; // {8,8}
	
	for(int i = 0; i < 4; i++)
	{
		if(currentBoard->board[corner[i].x][corner[i].y] == currentBoard->who)
		{
			score += 30;
			
			if(currentBoard->board[c_corner[i][0].x][c_corner[i][0].y] == currentBoard->who)
				score += 5;
			
			if(currentBoard->board[c_corner[i][1].x][c_corner[i][1].y] == currentBoard->who)
				score += 5;
		}
		else if(currentBoard->board[corner[i].x][corner[i].y] == -currentBoard->who)
		{
			score -= 30;
			
			if(currentBoard->board[c_corner[i][0].x][c_corner[i][0].y] == -currentBoard->who)
				score -= 5;
			
			if(currentBoard->board[c_corner[i][1].x][c_corner[i][1].y] == -currentBoard->who)
				score -= 5;
		}
		else
		{
			if(currentBoard->board[c_corner[i][0].x][c_corner[i][0].y] == currentBoard->who)
				score -= 5;
			
			if(currentBoard->board[c_corner[i][1].x][c_corner[i][1].y] == currentBoard->who)
				score -= 5;
			
			if(currentBoard->board[x_corner[i].x][x_corner[i].y] == currentBoard->who)
				score -= 10;
			
			else if(currentBoard->board[x_corner[i].x][x_corner[i].y] == -currentBoard->who)
				score += 10;
		}
	}
	return score;
}

inline int AI_evalDiskCount(void)
{
	int score = 0;
	
	for(int x = 1; x <= 8; x++)
		for(int y = 1; y <= 8; y++)
			score += currentBoard->board[x][y];
	
	return (currentBoard->who == 1)? score: -score;
}

int AI_evalMobility(int who)
{
	static const int xDirection[] = { 0, 1, 0,-1, 1,-1, 1,-1};
	static const int yDirection[] = { 1, 0,-1, 0, 1, 1,-1,-1};
	int legalPosCount = 0;
	int sx, sy;
	
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
				
				if(currentBoard->board[sx][sy] != -who)
					continue;
				
				do {
					sx += xDirection[i];
					sy += yDirection[i];
				} while(currentBoard->board[sx][sy] == -who);
		
				if(currentBoard->board[sx][sy] != who)
					continue;
				
				legalPosCount++;
			}
		}
	}
	return legalPosCount;
}

inline int AI_evalPMobility(int who)
{
	int score = 0;
	
	for(int x = 1; x <= 8; x++)
		for(int y = 1; y <= 8; y++)
		{
			if(currentBoard->board[x][y] == EMPTY)
			{
				if(currentBoard->board[x-1][y] == -who)
					score++;
				if(currentBoard->board[x+1][y] == -who)
					score++;
				if(currentBoard->board[x][y-1] == -who)
					score++;
				if(currentBoard->board[x][y+1] == -who)
					score++;
				if(currentBoard->board[x-1][y-1] == -who)
					score++;
				if(currentBoard->board[x-1][y+1] == -who)
					score++;
				if(currentBoard->board[x+1][y-1] == -who)
					score++;
				if(currentBoard->board[x+1][y+1] == -who)
					score++;
			}
		}
	return score;
}

inline int AI_varWeightUpDown(void)
{
	/* Original settings: When < 20 disks return 1+(count*0.2+0.5)   */
	/*                               else return 1+(61-count*0.2+0.5)*/ 
	if(totalDiskCount < 20)
		return (1 + (int)(totalDiskCount * 0.2f + 0.5f));
	else
		return (1 + (int)(61 - totalDiskCount * 0.2f + 0.5f));
}

inline int AI_varWeightInc(void)
{
	/* Original settings: 4+(count*0.1+0.5) */
	return (4 + (int)(totalDiskCount * 0.1f + 0.5f));
}

inline int AI_varWeightDec(void)
{
	/* Original settings: 7-(count*0.1+0.5) */
	return (7 - (int)(totalDiskCount * 0.1f + 0.5f));
}

