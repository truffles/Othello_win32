
/********************************************/
/* This header file is used by gameAI.c     */
/*                             gameEngine.c */
/********************************************/

#ifndef GAMEAI_H
#define GAMEAI_H

#define MIN(a,b) (((a)<(b))?(a):(b))

#define INF      1000000L
#define WIN       800000L
#define LOSE     -800000L
#define ENDGAME   100000L

typedef struct tagPOSITION
{
	int x;
	int y;
} POS_ST;

typedef struct tagMOVESTABLE
{
	int    legalPosCount;
	POS_ST diskTurned[40][21];
} MOVES_ST;

/* Function declarations */
/*********************************************************************/
void MergeSort(int n, int *A, int *Aindex);
void BottomUpMerge(int *A, int *Aindex, int iLeft, int iRight, int iEnd, int *B, int *Bindex);
int  AI_Entrance(void);
void AI_MakeMove(const MOVES_ST *movesTable, int index);
void AI_UndoMove(const MOVES_ST *movesTable, int index);
void AI_OrderMoves(MOVES_ST *movesTable, int *order);
int  AI_PVS(int alpha, int beta, int depth, int passed);
int  AI_getEvaluate(void);
int  AI_getExact(void);
int  AI_evalCorner(void);
int  AI_evalDiskCount(void);
int  AI_evalMobility(int who);
int  AI_evalPMobility(int who);
int  AI_varWeightUpDown(void);
int  AI_varWeightInc(void);
int  AI_varWeightDec(void);
/*********************************************************************/

#endif // GAMEAI_H

