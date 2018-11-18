#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"
#include <winuser.h>
#include "YUSH_SR_DLL.h"

//���������� ����������

#define g_lpszClassName _TEXT("sp_pr_class")
#define g_lpszAplicationTitle _TEXT("����� ������������� ���� � ������ (����� �����������)")
#define g_lpszAdress _TEXT("SP_Lab2_4_SR_DLL.dll")

//��� ����������� ����������
HMODULE hDLL;
//��������� �� �������
bool(*pOpenDlgFile)(HWND); //��������� ������� ���� ��� ������ ������������ ����������� ����
bool(*pLoadFile)(); //��������� ����
bool(*pReadTextFromFile)(); //��������� ���������� ����� � ��������� ���
UINT(*pSearchingLengthWords)(); //������� ������������� ���� � ���������� ������������ �����
bool(*pFinalizeReport)(); //������� ����� � ���������� � ����� �����
LPTSTR(*pGetFilePath)(); //���������� ��������� �� ����

HINSTANCE hInst; //���������
HMENU hMenu; //���� ���������
HWND hWindow; //������������� �������� ����


//���������� ���������� �������
			  //������� �������� ����
LRESULT CALLBACK Pr6_WndProc(HWND, UINT, WPARAM, LPARAM);
//����������� ��������� �������� ����
bool WndProc_OnCreate(HWND, LPCREATESTRUCT);
void WndProc_OnPaint(HWND);
void WndProc_OnDestroy(HWND);
void WndProc_OnCommand(HWND, int, HWND, UINT);
//���������� �����, ������������ ������� DLL
bool ParseTheFile(HWND);
//��������� ������� � ���������� ������
void OpenNoteApp(HWND);

//������� ��������� ���������� ���� "� ���������..."
BOOL CALLBACK DbWndProc_About(HWND, UINT, WPARAM, LPARAM);
//������� ��� Handle Message �� ������� ��������� ���� "� ���������"
BOOL DbWndProc_ABOUT_OnInitDialog(HWND, HWND, LPARAM);
void DbWndProc_ABOUT_OnCommand(HWND, int, HWND, UINT);
void DbWndProc_OnClose(HWND);


//������� �������
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{

	WNDCLASSEX MainWin;
	MSG message;
	HWND hWnd;

	hInst = hInstance;

	memset(&MainWin, 0, sizeof(WNDCLASSEX));
	MainWin.cbSize = sizeof(WNDCLASSEX);
	MainWin.lpszClassName = g_lpszClassName;
	MainWin.lpfnWndProc = Pr6_WndProc;
	MainWin.style = CS_VREDRAW | CS_HREDRAW;
	MainWin.hInstance = hInstance;
	MainWin.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_YUSH_ICON));
	MainWin.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	MainWin.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	MainWin.lpszMenuName = MAKEINTRESOURCE(IDR_YUSH_MENU);
	MainWin.cbClsExtra = 0;
	MainWin.cbWndExtra = 0;

	if (!RegisterClassEx(&MainWin))
	{
		MessageBox(NULL, _TEXT("������ ����������� ������ ����!"), _TEXT("������"), MB_OK | MB_ICONERROR);
		return false;
	}

	hWnd = CreateWindowEx(NULL,
		g_lpszClassName,
		g_lpszAplicationTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
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


//������� ��������� �������� ����
LRESULT CALLBACK Pr6_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_PAINT, WndProc_OnPaint);
		HANDLE_MSG(hWnd, WM_CREATE, WndProc_OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, WndProc_OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, WndProc_OnCommand);

	default:
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}

	return FALSE;
}


//�������� � ����������� �������� ����, ������������� ����������� �����������
bool WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT ipCreateStruct)
{
	//���������� ������������� ���� � ���������� ����������
	hWindow = hWnd;
	//��������� ����������
	if ((hDLL = LoadLibrary(g_lpszAdress)) == NULL)
	{
		MessageBox(NULL, _TEXT("�� ������� ��������� ����������!"), _TEXT("������!"), MB_OK);
		DestroyWindow(hWnd);
	}
	else
	{
		//������������� ���������� ��� ������ � �����������
		pOpenDlgFile = (bool(*)(HWND))GetProcAddress(hDLL, "OpenDlgFile");
		pLoadFile = (bool(*)())GetProcAddress(hDLL, "LoadFile");
		pReadTextFromFile = (bool(*)())GetProcAddress(hDLL, "ReadTextFromFile");
		pSearchingLengthWords = (UINT(*)())GetProcAddress(hDLL, "SearchingLengthWords");
		pFinalizeReport = (bool(*)())GetProcAddress(hDLL, "FinalizeReport");
		pGetFilePath = (LPTSTR(*)())GetProcAddress(hDLL, "GetFilePath");
	}

	return true;
}

//����� �����������
void WndProc_OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);

	EndPaint(hWnd, &ps);
}

void WndProc_OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}


//������� ����
void WndProc_OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify)
{
	switch (id)
	{
		//��������� ���������� �����
	case PARSE_FILE:
		if (!ParseTheFile(hWnd))
		{
			MessageBox(hWnd, _TEXT("�� ������� ���������� ����"), _TEXT("������!"), MB_OK);
			return;
		}

		else
			OpenNoteApp(hWnd);

		break;

		//� ����������
	case ABOUT_APP:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, DbWndProc_About);
		break;

		//����� �� ���������
	case IDM_FILE_EXIT:
		FreeLibrary(hDLL); //����������� �� ���������� ����� �������
		DestroyWindow(hWnd);

	default:

		break;
	}
}



//���������� ���� "� ���������"
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


//��������� ���������� �����
//���������� �����, ������������ ������� DLL
bool ParseTheFile(HWND hWnd)
{
	if (!pOpenDlgFile(hWnd)) //���� �� ������� ������� ����
		return false;

	if (!pLoadFile()) //���� �� ������� ������� ����
		return false;

	if (!pReadTextFromFile()) //���� �� ������� ��������� ����� �� �����
		return false;

	if (pSearchingLengthWords() == 0) //���� � ����� ��� ��������� ����
		return false;

	if (!pFinalizeReport()) //���� �� ������� ������� �����
		return false;

	return true;
}

//��������� ������� � ������� ���� ��� ����
void OpenNoteApp(HWND hWnd)
{
	STARTUPINFO si; //��������� ��� �������� ���������
	ZeroMemory(&si, sizeof(STARTUPINFO)); //������� ����������� ��������� �� ������
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi; //���������, ���� ��������� ���������� � ��������
	SECURITY_ATTRIBUTES sp, st; //�������� ������ ��� �������� � ������
	sp.nLength = sizeof(SECURITY_ATTRIBUTES);
	sp.lpSecurityDescriptor = NULL;
	sp.bInheritHandle = FALSE; //��������� ������������ ���������� �������� ��������
	st.nLength = sizeof(SECURITY_ATTRIBUTES);
	st.lpSecurityDescriptor = NULL;
	st.bInheritHandle = FALSE; ////��������� ������������ ���������� ������ �������� ��������

							   //����������
							   //CreateProcess ���������� TRUE �� ������������� ��������-
							   //����� ��������.��� ��������, ��� �� ������ ����� ��������� ������������
							   //������� ��� �� ����� ��� ����������� DLL.���� �� �� ������ ����� ����
							   //�� ���� �� DLL ��� ��������� �������� �������������, ������� �����-
							   //�����.��, ��������� CreateProcess ��� ������� TRUE, ������������ ���-
							   //���� ������ �� ������ �� ���� ���������.

	TCHAR cmdParam[300] = { 0 };
	wsprintf(cmdParam, _TEXT("C:\\Windows\\Notepad.exe "));
	lstrcat(cmdParam, pGetFilePath());

	if (!CreateProcess(NULL, cmdParam,
		&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(hWnd, TEXT("������ ������� ��������"), NULL, MB_OK);
		return;
	}
	else
	{
		CloseHandle(pi.hProcess); //���������� ������� �� ������ ����������
		CloseHandle(pi.hThread);
	}
}
 