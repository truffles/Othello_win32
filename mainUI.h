
/****************************************/
/* This header file is used by mainUI.c */
/****************************************/

#ifndef MAINUI_H
#define MAINUI_H

// The Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// The WinMain Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow);


#endif // MAINUI_H

