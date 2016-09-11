
#ifndef WNDCLIENT_H
#define WNDCLIENT_H

HWND CreateMainToolbar(HWND hwndParent);
BOOL CreateGameInfoDlg(HWND hwndParent);
void DestroyChild(void);
void ShowGameInfoDlg(void);
BOOL CALLBACK GameInfoProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void RefreshCount(void);
void AddMoveToListbox(int x, int y, int who);
void DeleteMovesFromListbox(int count);
void DeleteAllMovesFromListbox(void);

#endif // WNDCLIENT_H

