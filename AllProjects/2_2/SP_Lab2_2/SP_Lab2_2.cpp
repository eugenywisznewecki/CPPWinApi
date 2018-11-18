#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"

#pragma region ���������� ����������

#define THREAD_NUMBER 4
#define MAXSTRING 1000

//��������� ��� �������� ���������� ������
struct PARAM_THREAD
{
	int num;  	
	UINT xPos; 	 
	UINT yPos;	 	
};

HINSTANCE hInst; //���������
HMENU hMenu; //���� ���������
HWND hWindow; //������������� �������� ����
DWORD g_uThCount = 0; //���������� ��������� ��������� �������
HANDLE hSecThread[THREAD_NUMBER]; //��� ������������ �������
DWORD dwSecThreadId[THREAD_NUMBER]; //��� ��������������� �������
bool dwActiveOrSuspend[THREAD_NUMBER]; //��� ������ ��������� �������
PARAM_THREAD threadParam[THREAD_NUMBER]; //��� ���������� ������
HWND hPicture; //����� ��� ������ �����������
#pragma endregion

#pragma region ���������� ���������� �������
//������� �������� ����
LRESULT CALLBACK Pr6_WndProc(HWND, UINT, WPARAM, LPARAM);
//����������� ��������� �������� ����
bool WndProc_OnCreate(HWND, LPCREATESTRUCT);
void WndProc_OnDestroy(HWND);
void WndProc_OnCommand(HWND, int, HWND, UINT);

//������� ��������� ���������� ���� "� ���������..."
BOOL CALLBACK DbWndProc_About(HWND, UINT, WPARAM, LPARAM);
//������� ��� Handle Message �� ������� ��������� ���� "� ���������"
BOOL DbWndProc_ABOUT_OnInitDialog(HWND, HWND, LPARAM);
void DbWndProc_ABOUT_OnCommand(HWND, int, HWND, UINT);
void DbWndProc_OnClose(HWND);

//������� ������� ������
DWORD WINAPI ThreadString(PVOID);

//������� ������ ��������
DWORD WINAPI ThreadAnimation(PVOID);

//�������� ������
void CreateThisThread(int, bool);

//��������� ��������������� ����������� ����
INT_PTR CALLBACK DlgBoxInfo(HWND, UINT, WPARAM, LPARAM);
#pragma endregion

#pragma region ������� �������
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{

	WNDCLASSEX MainWin;
	MSG message;
	HWND hWnd;

	hInst = hInstance;

	memset(&MainWin, 0, sizeof(WNDCLASSEX));
	MainWin.cbSize = sizeof(WNDCLASSEX);
	MainWin.lpszClassName = TEXT("SimpleClassName2_2");
	MainWin.lpfnWndProc = Pr6_WndProc;
	MainWin.style = CS_VREDRAW | CS_HREDRAW;
	MainWin.hInstance = hInstance;
	MainWin.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	MainWin.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	MainWin.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	MainWin.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	MainWin.cbClsExtra = 0;
	MainWin.cbWndExtra = 0;

	if (!RegisterClassEx(&MainWin))
	{
		MessageBox(NULL, _TEXT("������ ����������� ������ ����!"), _TEXT("������"), MB_OK | MB_ICONERROR);
		return false;
	}

	hWnd = CreateWindowEx(NULL,
		TEXT("SimpleClassName2_2"),
		TEXT("SP-LB2-2 ������� �.�, ��. 70321-2"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 530,
		NULL, NULL,
		hInstance, NULL);

	hMenu = GetMenu(hWnd); //�������� ����� ����

	if (!hWnd)
	{
		MessageBox(NULL, _TEXT("�� ������� ������� ����!"), _TEXT("������"), MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return message.wParam;
}
#pragma endregion

#pragma region ������� ��������� �������� ����
LRESULT CALLBACK Pr6_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, WndProc_OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, WndProc_OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, WndProc_OnCommand);

	default:
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}

	return FALSE;
}
#pragma endregion

#pragma region �������� � ����������� �������� ����, ������������� ����������� �����������
bool WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT ipCreateStruct)
{
	//�������� ����� ��� ������ �����������
	hPicture = CreateWindowEx(0L, TEXT("Static"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 95,130, 309,332, hWnd, NULL, NULL, NULL);

	//���������� ������������� ���� � ���������� ����������
	hWindow = hWnd;

	//��������� ����������� �������� ������
	hSecThread[0] = GetCurrentThread();
	dwSecThreadId[0] = GetCurrentThreadId();
	dwActiveOrSuspend[0] = true;

	return 1;
}

void WndProc_OnDestroy(HWND hWnd)
{

	PostQuitMessage(0);
}
#pragma endregion

#pragma region ������� ����
void WndProc_OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify)
{

	switch (id)
	{
		//�������� �������
	case CREATE_THREAD_1:
		CreateThisThread(1, false);

		//��������� ����������� ������� ����
		EnableMenuItem(hMenu, CREATE_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, CREATE_WAIT_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, PAUSE_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, DESTROY_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, PRIOR_UP_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, PRIOR_DOWN_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, THREAD_1_INFO, MF_ENABLED);

		break;

	case CREATE_THREAD_2:
		CreateThisThread(2, false);

		//��������� ����������� ������� ����
		EnableMenuItem(hMenu, CREATE_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, CREATE_WAIT_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, PAUSE_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, DESTROY_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, PRIOR_UP_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, PRIOR_DOWN_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, THREAD_2_INFO, MF_ENABLED);

		break;

		//�������� ������ �������
	case CREATE_WAIT_THREAD_1:
		CreateThisThread(1, true);

		//��������� ����������� ������� ����
		EnableMenuItem(hMenu, CREATE_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, CREATE_WAIT_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, PAUSE_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, RESTORE_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, DESTROY_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, PRIOR_UP_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, PRIOR_DOWN_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, THREAD_1_INFO, MF_ENABLED);

		break;

	case CREATE_WAIT_THREAD_2:
		CreateThisThread(2, true);

		//��������� ����������� ������� ����
		EnableMenuItem(hMenu, CREATE_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, CREATE_WAIT_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, PAUSE_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, RESTORE_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, DESTROY_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, PRIOR_UP_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, PRIOR_DOWN_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, THREAD_2_INFO, MF_ENABLED);

		break;

		//���������� ������� �� �����
	case PAUSE_THREAD_1:
		SuspendThread(hSecThread[1]);
		dwActiveOrSuspend[1] = false;
		EnableMenuItem(hMenu, RESTORE_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, PAUSE_THREAD_1, MF_GRAYED);

		break;

	case PAUSE_THREAD_2:
		SuspendThread(hSecThread[2]);
		dwActiveOrSuspend[2] = false;
		EnableMenuItem(hMenu, RESTORE_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, PAUSE_THREAD_2, MF_GRAYED);
		break;

		//������ ������ �������
	case RESTORE_THREAD_1:
		ResumeThread(hSecThread[1]);
		dwActiveOrSuspend[1] = true;
		EnableMenuItem(hMenu, RESTORE_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, PAUSE_THREAD_1, MF_ENABLED);
		break;

	case RESTORE_THREAD_2:
		ResumeThread(hSecThread[2]);
		dwActiveOrSuspend[2] = true;
		EnableMenuItem(hMenu, RESTORE_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, PAUSE_THREAD_2, MF_ENABLED);
		break;

		//����������� �������
	case DESTROY_THREAD_1:
		TerminateThread(hSecThread[1], 1);
		//��������� ����������� ������� ����
		EnableMenuItem(hMenu, CREATE_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, CREATE_WAIT_THREAD_1, MF_ENABLED);
		EnableMenuItem(hMenu, PAUSE_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, RESTORE_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, DESTROY_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, PRIOR_UP_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, PRIOR_DOWN_THREAD_1, MF_GRAYED);
		EnableMenuItem(hMenu, THREAD_1_INFO, MF_GRAYED);

		break;

	case DESTROY_THREAD_2:
		TerminateThread(hSecThread[2], 2);
		//��������� ����������� ������� ����
		EnableMenuItem(hMenu, CREATE_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, CREATE_WAIT_THREAD_2, MF_ENABLED);
		EnableMenuItem(hMenu, PAUSE_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, RESTORE_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, DESTROY_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, PRIOR_UP_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, PRIOR_DOWN_THREAD_2, MF_GRAYED);
		EnableMenuItem(hMenu, THREAD_2_INFO, MF_GRAYED);
		break;

		//���������� ���������� �������
	case PRIOR_UP_THREAD_1:
		SetThreadPriority(hSecThread[1], GetThreadPriority(hSecThread[1]) + 1);
		break;

	case PRIOR_UP_THREAD_2:
		SetThreadPriority(hSecThread[2], GetThreadPriority(hSecThread[2]) + 1);
		break;

		//���������� ���������� �������
	case PRIOR_DOWN_THREAD_1:
		SetThreadPriority(hSecThread[1], GetThreadPriority(hSecThread[1]) - 1);
		break;

	case PRIOR_DOWN_THREAD_2:
		SetThreadPriority(hSecThread[2], GetThreadPriority(hSecThread[2]) - 1);
		break;

		//���������� � ������� ������
	case THIS_THREAD_INFO:
		DialogBoxParam(hInst, MAKEINTRESOURCE(DIALOG_THREAD_INFO), hWnd, DlgBoxInfo, 0);
		break;

		//���������� � �������� �������
	case THREAD_1_INFO:
		DialogBoxParam(hInst, MAKEINTRESOURCE(DIALOG_THREAD_INFO), hWnd, DlgBoxInfo, 1);
		break;

	case THREAD_2_INFO:
		DialogBoxParam(hInst, MAKEINTRESOURCE(DIALOG_THREAD_INFO), hWnd, DlgBoxInfo, 2);
		break;

	case CREATE_ANIM:
		hSecThread[3] = CreateThread(NULL, 0, ThreadAnimation, NULL, NULL, &dwSecThreadId[3]);
		EnableMenuItem(hMenu, CREATE_ANIM, MFS_GRAYED);
		EnableMenuItem(hMenu, DESTROY_ANIM, MFS_ENABLED);
		break;

	case DESTROY_ANIM:
		TerminateThread(hSecThread[3], 3);
		EnableMenuItem(hMenu, DESTROY_ANIM, MFS_GRAYED);
		EnableMenuItem(hMenu, CREATE_ANIM, MFS_ENABLED);
		break;

		//� ����������
	case ABOUT_APP:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, DbWndProc_About);
		break;

		//����� �� ���������
	case IDM_FILE_EXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);

	default:

		break;
	}
}
#pragma endregion 

#pragma region ���������� ���� "� ���������"
BOOL CALLBACK DbWndProc_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, DbWndProc_ABOUT_OnInitDialog);
		HANDLE_MSG(hDlg, WM_COMMAND, DbWndProc_ABOUT_OnCommand);
		HANDLE_MSG(hDlg, WM_CLOSE, DbWndProc_OnClose);

	default:
		return FALSE;
	}

	return FALSE;
}

//��� ������ ������� ���� � ������� � ��������� ���� "� ���������"
BOOL DbWndProc_ABOUT_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	LPTSTR lpszRunTime = new TCHAR[50];
	wsprintf(lpszRunTime, TEXT("������� ���� � �����:\n%02d.%02d.%04d %02d:%02d\0"), time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute);
	SetDlgItemText(hDlg, IDC_INFO_DATE, lpszRunTime);
	return TRUE;
}

//��� ��������� ������� ������� � ��������� ���� "� ���������"
void DbWndProc_ABOUT_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
		//��������� ������, ��������� � ���������� ����
		EndDialog(hDlg, TRUE);
	}
}

//��� �������� ���������� ������� �� ����������� �������
void DbWndProc_OnClose(HWND hDlg)
{
	EndDialog(hDlg, TRUE);
}
#pragma endregion

#pragma region �������� ������
//�������� ������
void CreateThisThread(int index, bool pause)
{
	//������������� ���������� ������ � ��������� � ���������� ������
	threadParam[index].num = index;
	threadParam[index].xPos = 0;
	threadParam[index].yPos = 50 * index;
	
	//��������� ����� �������� ������
	DWORD flag = 0;
	if(pause)
		flag = CREATE_SUSPENDED;

	//�������� ������ � ��������� ��� ����������� � ��������������
	hSecThread[index] = CreateThread(NULL, NULL, ThreadString, &threadParam[index], flag, &dwSecThreadId[index]);
	dwActiveOrSuspend[index] = !pause;
}

//������� ������ ������� ������
DWORD WINAPI ThreadString(PVOID pvParam)
{
	PARAM_THREAD *Param = (PARAM_THREAD *)pvParam;
	TCHAR CreepingLine[MAXSTRING];		// ����� ��� �������� ������� ������
	TCHAR buf[MAXSTRING] = { 0 };			// ������� ����� ��� ������������ ������ ������
	int iBeginningIndex;				// ������ ������ ��������� ������������������ ��������
	int StringLength = 0;				// ����� ������
	RECT  rc;
	HDC   hDC;
	int cRun = 0;						// ������� ���������
	int counter = 0;

	// ������������ ������ ������� ������
	wsprintf(CreepingLine,
		TEXT("                                                                                                                             ��������� ����� ������ <������� �.�.>"));
	StringLength = iBeginningIndex = lstrlen(CreepingLine);// ����� ������
	lstrcpy(buf, CreepingLine);

	// ������� ������������� ������� ������
	GetClientRect(hWindow, &rc);
	rc.top = Param->yPos;
	rc.left = Param->xPos;
	rc.right = rc.left + 500;

	// ��������� ��������� ��� ������ ������
	hDC = GetDC(hWindow);

	// ����������� ���� ������ ����� ������� �� N �����
	while (true)
	{
		// ���� ������������ ����������� ������ �� ����������
		// ������� �� �������(����������� ����� ������� � ������� ������)
		for (int j = 0; j< StringLength; j++)
		{
			
			if (iBeginningIndex == 0)
			{
				//����� �� ��������� ������������
					if(counter >= 244)
						counter = 244;

				iBeginningIndex = StringLength;
			}

			// C���� �������� � ������� ������ �� ���� �������
			TCHAR c = buf[StringLength]; //�������� ��������� ������ - � ������ ����� �� ����� \0

			for (int i = StringLength; i>0; i--)//�������� ������ �� ���� ������ ������ � �����
				buf[i] = buf[i - 1]; //������������ ���������� ������ �� ����� ��������
			
			buf[0] = c; //��������� ������ ���������� �����

			++counter; //����������� ���������� ��������� �������� (���������� 0)

			// ����� ������
			DrawText(hDC, buf, lstrlen(buf), &rc, DT_LEFT | DT_SINGLELINE);

			iBeginningIndex--; //��������� ������ ������ ������

			Sleep(120); // ������������ ������  - ���������� �����
			
		} // ����� ����� ������� ������������ ��������� ������
	} // ����� ����� ������ ����� �����

	ReleaseDC(hWindow, hDC);

	return 0;
}

//������� ������ ��������
DWORD WINAPI ThreadAnimation(PVOID pvParam)
{
	UINT imageID = 119;

	while (TRUE)
	{
		HDC hDC;
		HDC hMemDC;
		HANDLE hBmp;
		HANDLE hOldBmp;
		BITMAP Bmp;		
		hBmp = LoadImage(hInst, MAKEINTRESOURCE(imageID), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
		HWND hWndImage = hPicture;
		hDC = GetDC(hWndImage);
		hMemDC = CreateCompatibleDC(hDC);
		GetObject(hBmp, sizeof(BITMAP), &Bmp);
		hOldBmp = SelectObject(hMemDC, hBmp);
		RECT Rect;
		GetClientRect(hWndImage, &Rect);
		StretchBlt(hDC, 0, 0, Rect.right, Rect.bottom, hMemDC, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);
		SelectObject(hMemDC, hOldBmp);
		DeleteDC(hMemDC);
		DeleteObject(hBmp);
		ReleaseDC(hWndImage, hDC);
		if (imageID == 122)
		{
			imageID = 119;
		}
		else
		{
			imageID++;
		}
		Sleep(200);
	}
	return 0;
}
#pragma endregion

#pragma region �������������� ���������� ����
INT_PTR CALLBACK DlgBoxInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD PriorityCode;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		//������������� ��������� �����
	case WM_INITDIALOG:
		//��������� ��� �������� ��������� �������������
		FILETIME ftKernelTime;
		FILETIME ftUserTime;
		FILETIME ftStart, ftEnd, ftCurrent;
		FILETIME ftNow;
		FILETIME ftTemp;
		//����� ��� ������ ������ � ���� �����
		TCHAR Buff[200];
		//��������� ��� �������������� ���������� ��� �������� �������
		ULARGE_INTEGER uiTempTime, uiTempTimeNow, uiCurrent;
		//��� ����������
		DWORD dwExitCode;
		//����� ��� ������ � ���� �����
		SYSTEMTIME stCurrent;

		//���������� ������
		wsprintf(Buff, TEXT("0x%08X"), hSecThread[lParam]);
		SetDlgItemText(hDlg, HANDLE_THREAD, Buff);

		//������������� ������
		wsprintf(Buff, TEXT("%d"), dwSecThreadId[lParam]);
		SetDlgItemText(hDlg, ID_THREAD, Buff);

		//�������� ����� �������� ������, ����� ��������� ������ ������, ����� ������ � ������ ����, ����� ������ � ������ ������������
		GetThreadTimes(hSecThread[lParam], &ftStart, &ftEnd, &ftKernelTime, &ftUserTime);

		//��������� ������
		if (!GetExitCodeThread(hSecThread[lParam], &dwExitCode))
		{
			DWORD ErrCode = GetLastError();
			wsprintf(Buff, TEXT("��� ������: %d"), ErrCode);
		}
		else if (dwExitCode == STILL_ACTIVE)
		{
			if(dwActiveOrSuspend[lParam])
				wsprintf(Buff, TEXT("����� �������!"));
			else
				wsprintf(Buff, TEXT("����� �������������!"));
			//�������� ������� �����, ����� ������ ����� ����� �������� � ���������� �������
			GetSystemTimeAsFileTime(&ftNow);
			uiTempTimeNow.HighPart = ftNow.dwHighDateTime;
			uiTempTimeNow.LowPart = ftNow.dwLowDateTime;
		}
		else
		{
			wsprintf(Buff, TEXT("����� ��������. ��� ���������� - %d."), dwExitCode);
			uiTempTimeNow.HighPart = ftEnd.dwHighDateTime;
			uiTempTimeNow.LowPart = ftEnd.dwLowDateTime;
		}

		SetDlgItemText(hDlg, STATUS_THREAD, Buff);

		//������������� ������ ���������� ������
		PriorityCode = GetThreadPriority(hSecThread[lParam]);
		wsprintf(Buff, TEXT("%d"), PriorityCode);
		SetDlgItemText(hDlg, PRIOR_THREAD, Buff);

		//����� ������ ������

		//��������� ���� ULARGE_INTEGER
		uiTempTime.HighPart = ftStart.dwHighDateTime;
		uiTempTime.LowPart = ftStart.dwLowDateTime;
		//�������� �������
		uiCurrent.QuadPart = uiTempTimeNow.QuadPart - uiTempTime.QuadPart;
		//���������� � �������� ��������� FILETIME
		ftCurrent.dwHighDateTime = uiCurrent.HighPart;
		ftCurrent.dwLowDateTime = uiCurrent.LowPart;
		//��������� � ��������� �����
		FileTimeToSystemTime(&ftCurrent, &stCurrent);
		//������� ������
		wsprintf(Buff, TEXT("%d:%d:%d.%d"), stCurrent.wHour, stCurrent.wMinute, stCurrent.wSecond, stCurrent.wMilliseconds);
		SetDlgItemText(hDlg, TIME_THREAD, Buff);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			break;
		}
	}
	return (INT_PTR)FALSE;
}
#pragma endregion