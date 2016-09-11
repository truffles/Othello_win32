
/****************************************/
/* This header file is used by dialog.c */
/*                             mainUI.c */
/****************************************/

#ifndef DIALOG_H
#define DIALOG_H

// The About Dialog Procedure
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// The Game Type Dialog Procedure
BOOL CALLBACK GameTypeDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// The Settings Dialog Procedure
BOOL CALLBACK SettingsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // DIALOG_H

