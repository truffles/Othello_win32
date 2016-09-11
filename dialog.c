
/**********************************************************/
/* Implementation of all kinds of Dialog Procedure needed */
/**********************************************************/

#include <windows.h>
#include "resource.h"
#include "dialog.h"
#include "gameEngine.h"

int aiLevel = 4;

// The About Dialog Procedure
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					EndDialog(hwnd, IDOK);
					break;
				
				default:
					return FALSE;
			}
			break;
		
		case WM_CLOSE:
			EndDialog(hwnd, IDOK);
			break;
				
		default:
			return FALSE;
	}
	return TRUE;
}

// The Game Type Dialog Procedure
BOOL CALLBACK GameTypeDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	extern enum {TwoHuman, HumanFirst, ComputerFirst} gameMode;
	
	switch(msg)
	{
		case WM_INITDIALOG:
			switch(gameMode)
			{
				case TwoHuman:
					CheckRadioButton(hwnd, IDR_GAMETYPERADIO_0, IDR_GAMETYPERADIO_2,
						IDR_GAMETYPERADIO_0);
					break;
				
				case HumanFirst:
					CheckRadioButton(hwnd, IDR_GAMETYPERADIO_0, IDR_GAMETYPERADIO_2,
						IDR_GAMETYPERADIO_1);
					break;
				
				case ComputerFirst:
					CheckRadioButton(hwnd, IDR_GAMETYPERADIO_0, IDR_GAMETYPERADIO_2,
						IDR_GAMETYPERADIO_2);
					break;
			}
			break;
		
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
				
					if(IsDlgButtonChecked(hwnd, IDR_GAMETYPERADIO_0))
						gameMode = TwoHuman;
					else if(IsDlgButtonChecked(hwnd, IDR_GAMETYPERADIO_1))
						gameMode = HumanFirst;
					else if(IsDlgButtonChecked(hwnd, IDR_GAMETYPERADIO_2))
						gameMode = ComputerFirst;
					
					EndDialog(hwnd, IDOK);
					break;
				
				default:
					return FALSE;
			}
			break;
		
		case WM_CLOSE:
			EndDialog(hwnd, IDOK);
			break;
		
		default:
			return FALSE;
	}
	return TRUE;
}

// The Settings Dialog Procedure
BOOL CALLBACK SettingsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	extern int aiSearchDepth;
	extern int aiEndGameSearch;
	
	
	switch(msg)
	{
		case WM_INITDIALOG:
			CheckRadioButton(hwnd, IDR_AILEVELRADIO_0, IDR_AILEVELRADIO_6,
						IDR_AILEVELRADIO_0 + aiLevel);
			break;
		
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDOK:
					
					if(IsDlgButtonChecked(hwnd, IDR_AILEVELRADIO_0))
					{
						aiLevel = 0;
						aiSearchDepth = 1;
						aiEndGameSearch = 4;
					}
					else if(IsDlgButtonChecked(hwnd, IDR_AILEVELRADIO_1))
					{
						aiLevel = 1;
						aiSearchDepth = 2;
						aiEndGameSearch = 6;
					}
					else if(IsDlgButtonChecked(hwnd, IDR_AILEVELRADIO_2))
					{
						aiLevel = 2;
						aiSearchDepth = 4;
						aiEndGameSearch = 8;
					}
					else if(IsDlgButtonChecked(hwnd, IDR_AILEVELRADIO_3))
					{
						aiLevel = 3;
						aiSearchDepth = 6;
						aiEndGameSearch = 10;
					}
					else if(IsDlgButtonChecked(hwnd, IDR_AILEVELRADIO_4))
					{
						aiLevel = 4;
						aiSearchDepth = 7;
						aiEndGameSearch = 14;
					}
					else if(IsDlgButtonChecked(hwnd, IDR_AILEVELRADIO_5))
					{
						aiLevel = 5;
						aiSearchDepth = 9;
						aiEndGameSearch = 15;
					}
					else if(IsDlgButtonChecked(hwnd, IDR_AILEVELRADIO_6))
					{
						aiLevel = 6;
						aiSearchDepth = 10;
						aiEndGameSearch = 16;
					}
					
					EndDialog(hwnd, IDOK);
					break;
				
				case IDCANCEL:
					EndDialog(hwnd, IDCANCEL);
					break;
				
				default:
					return FALSE;
			}
			break;
		
		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
			break;
		
		default:
			return FALSE;
	}
	return TRUE;
}

