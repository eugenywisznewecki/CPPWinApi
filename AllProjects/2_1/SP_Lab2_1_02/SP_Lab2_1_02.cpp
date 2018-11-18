#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"

#define	IDC_EDIT_BLOCK	105 //��� ���������� ����
#define MAX_BYTES  100000

HINSTANCE hInst;
UINT exitCode = 1; //��� ���������� �������� "�������", ������ ����������� ������ � ������
TCHAR fileName[260] = { 0 };
TCHAR path[260] = { 0 };
static HWND hEdit; //��� ���������� ����
HANDLE hFile;
WCHAR BuffText[MAX_BYTES]; // ����� ��� ������
						   //���������� ��� ��������� ���������� TextProc
DWORD dwX, dwY, dwXSize, dwYSize;
WORD wShowWindow;
//��� ������ � ComboBox
LPWSTR ComboBoxStrings[4] = { TEXT("SW_SHOW"), TEXT("SW_SHOWDEFAULT"), TEXT("SW_SHOWMAXIMIZED"), TEXT("SW_SHOWMINIMIZED") };
WORD ComboBoxValues[4] = { SW_SHOW, SW_SHOWDEFAULT, SW_SHOWMAXIMIZED, SW_SHOWMINIMIZED };
bool show = false;

// ���������� �������
HANDLE ProcHandle[5]; //��� ������������ ���������;
DWORD ProcId[5]; //��� ��������������� ���������;
HANDLE ThreadHandle[5]; //��� ������������ �������;
DWORD ThreadId[5];// ���.��������������� �������;
LPTSTR ProcImage[6] = { 0,	TEXT("C:\\Windows\\Notepad.exe"),
TEXT("C:\\Windows\\Notepad.exe"),
TEXT("C:\\Windows\\System32\\Calc.exe"),
TEXT("C:\\Windows\\Notepad.exe"),
TEXT("TestProc.exe") };//��� ���������� �����, ��������������� ����� ����������� ��������;
LPTSTR CmdParam[5] = { 0, 0, TEXT("C:\\Windows\\Notepad.exe SP_Lab2_1_02.cpp"), 0, TEXT("") };; //��� ����� c ����������� ����������� ��������.


																								// ���������� ���������� �������
																								//������� ������� �������
LRESULT CALLBACK Pr2_1_WndProc(HWND, UINT, WPARAM, LPARAM);
//��������� ���������
bool WndProc_OnCreate(HWND, LPCREATESTRUCT);
void WndProc_OnPaint(HWND);
void WndProc_OnDestroy(HWND);
void WndProc_OnCommand(HWND, int, HWND, UINT);
//��������� ����������� ����
INT_PTR CALLBACK DlgBoxInfo(HWND, UINT, WPARAM, LPARAM);
//��������� ������ � �������� � �������
void SavingInfoAboutProc(HANDLE, DWORD, HANDLE, DWORD, int);
//��������� ������ ������� ��� ������� � ������
void OpenNoteApp(HWND, bool, int);
//��������� ������� ���� ��� �������� ����� ����������� ���� �������
bool OpenDlgFile(HWND);
//������� ���������� ���������� ����� � ���� Edit
bool PrintTxtInEdit();
//������� ��������� ���������� ���� "������� ���������"
BOOL CALLBACK DbWndProc_GetParam(HWND, UINT, WPARAM, LPARAM);
//������� ��� Handle Message �� ������� ��������� ���� "�������� �����"
BOOL DbWndProc_GETPARAM_OnInitDialog(HWND, HWND, LPARAM);
void DbWndProc_GETPARAM_OnCommand(HWND, int, HWND, UINT);
void DbWndProc_GETPARAM_OnClose(HWND);

// ������� ������� �������
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{

	WNDCLASSEX MainWin;
	MSG message;
	HWND hWnd;

	hInst = hInstance;

	memset(&MainWin, 0, sizeof(WNDCLASSEX));
	MainWin.cbSize = sizeof(WNDCLASSEX);
	MainWin.lpszClassName = TEXT("SimpleClassName2_1");
	MainWin.lpfnWndProc = Pr2_1_WndProc;
	MainWin.style = CS_VREDRAW | CS_HREDRAW;
	MainWin.hInstance = hInstance;
	MainWin.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	MainWin.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	MainWin.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	MainWin.lpszMenuName = MAKEINTRESOURCE(THIS_MENU);
	MainWin.cbClsExtra = 0;
	MainWin.cbWndExtra = 0;

	if (!RegisterClassEx(&MainWin))
	{
		MessageBox(NULL, _TEXT("������ ����������� ������ ����!"), _TEXT("������"), MB_OK | MB_ICONERROR);
		return false;
	}


	hWnd = CreateWindowEx(NULL,
		TEXT("SimpleClassName2_1"),
		TEXT("SP-LB2-1 ������� �.�. ��. 70321-2"),
		WS_OVERLAPPEDWINDOW,
		600, 200,
		620, 460,
		NULL, NULL,
		hInstance, NULL);

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

//��������� ���������
LRESULT CALLBACK Pr2_1_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

bool WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT ipCreateStruct)
{
	//���������� ����������� � �������������� �������� �������� � ������
	ProcHandle[0] = GetCurrentProcess();
	ProcId[0] = GetCurrentProcessId();
	ThreadHandle[0] = GetCurrentThread();
	ThreadId[0] = GetCurrentThreadId();

	if (!(hEdit = CreateWindowEx(0L, _TEXT("Edit"), _TEXT(""),
		WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE | WS_DISABLED,
		0, 0, 600, 400, hWnd, (HMENU)(IDC_EDIT_BLOCK),
		NULL, NULL)))
		return (false);

	return true;
}

void WndProc_OnPaint(HWND hWnd)
{

}

void WndProc_OnDestroy(HWND hWnd)
{

	PostQuitMessage(0);
}

// ��������� ��������� ����
void WndProc_OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sp, st;

	switch (id)
	{
	case OPEN_NOTE: //�������
		OpenNoteApp(hWnd, true, 1); //�������� ����� ����, ���� �������� ������������ � ������
		break;

	case OPEN_NOTE_TXT: //������� � ������
		OpenNoteApp(hWnd, false, 2); //�������� ����� ����, ���� �������� ������������ � ������
		break;

	case OPEN_CALC: //�����������
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES sp, st;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);

		sp.nLength = sizeof(SECURITY_ATTRIBUTES);
		sp.lpSecurityDescriptor = NULL;
		sp.bInheritHandle = FALSE;
		st.nLength = sizeof(SECURITY_ATTRIBUTES);
		st.lpSecurityDescriptor = NULL;
		st.bInheritHandle = FALSE;

		if (!CreateProcess(ProcImage[3], NULL,
			&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
		{
			MessageBox(hWnd, TEXT("������ ������� ��������"), NULL, MB_OK);
			break;
		}
		else
			SavingInfoAboutProc(pi.hProcess, pi.dwProcessId, pi.hThread, pi.dwThreadId, 3);
		break;
	}

	case CLOSE_NOTE: //�������� �������
		//TerminateProcess(ProcHandle[1], exitCode);
		PostThreadMessage(ThreadId[1], WM_QUIT, 0, 0);
		break;

	case OPEN_NOTE_ANY_TXT: //�������� �������� � ������� ������
		OpenDlgFile(hWnd); //��� ������ �����
		OpenNoteApp(hWnd, false, 4); //��� �������� ��������
		break;

	case OPEN_TESTPROC:
	{
		//��������� ��������� ������������� ���������� ����
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_GETPARAM), hWnd, DbWndProc_GetParam, 0);

		if (!show)
			break;

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES sp, st;

		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);

		sp.nLength = sizeof(SECURITY_ATTRIBUTES);
		sp.lpSecurityDescriptor = NULL;
		sp.bInheritHandle = FALSE;
		st.nLength = sizeof(SECURITY_ATTRIBUTES);
		st.lpSecurityDescriptor = NULL;
		st.bInheritHandle = FALSE;


		//����� ����� ���� ��������� ���������, ����� ����������� � ������
		si.dwFlags = STARTF_USEPOSITION | STARTF_USESHOWWINDOW | STARTF_USESIZE;
		//���������
		si.dwX = dwX;
		si.dwY = dwY;
		//������
		si.dwXSize = dwXSize;
		si.dwYSize = dwYSize;
		//����� �����������
		si.wShowWindow = wShowWindow;

		if (!CreateProcess(ProcImage[5], NULL,
			&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
		{
			MessageBox(hWnd, TEXT("������ ������� ��������"), NULL, MB_OK);
			break;
		}
		break;
	}

	case OPEN_NOTE_PRINT_TXT:
		OpenDlgFile(hWnd); //��� ������ �����
		OpenNoteApp(hWnd, false, 4); //��� �������� ��������
		if (!PrintTxtInEdit())
			MessageBox(hWnd, TEXT("�� ������� ��������� ���������� �����."), TEXT("������!"), MB_OK);
		break;

	case THIS_PROCESS:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 0); //�������� ������ �������� ��������� ���������� (����������� ���������� ����������� ���� ��� lParam)
		break;

	case INFO_NOTE:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 1);
		break;

	case INFO_NOTE_TXT:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 2);
		break;

	case INFO_CALC:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 3);
		break;

	case INFO_NOTE_ANY_TXT:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 4);
		break;

	default:

		break;
	}
}

// ���������� ����
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

		//���� ��������� � ��������� ��������� ������
		SetDlgItemText(hDlg, IDC_NAME_PROG, ProcImage[lParam]);
		SetDlgItemText(hDlg, IDC_INFO_COMMAND, CmdParam[lParam]);

		//����������� �������� � ������
		wsprintf(Buff, TEXT("0x%08X"), ProcHandle[lParam]);
		SetDlgItemText(hDlg, IDC_HANDLE_PROC, Buff);
		wsprintf(Buff, TEXT("0x%08X"), ThreadHandle[lParam]);
		SetDlgItemText(hDlg, IDC_HANDLE_THREAD, Buff);

		//�������������� �������� � ������
		wsprintf(Buff, TEXT("%d"), ThreadId[lParam]);
		SetDlgItemText(hDlg, IDC_ID_THREAD, Buff);
		wsprintf(Buff, TEXT("%d"), ProcId[lParam]);
		SetDlgItemText(hDlg, IDC_ID_PROC, Buff);

		// �������� ����� �������� ��������, ����� ��������� ������ ��������, ����� ������ � ������ ����, ����� ������ � ������ ������������
		GetProcessTimes(ProcHandle[lParam], &ftStart, &ftEnd, &ftKernelTime, &ftUserTime);

		//��� ���������� ��������
		if (!GetExitCodeProcess(ProcHandle[lParam], &dwExitCode))
		{
			DWORD ErrCode = GetLastError();
			wsprintf(Buff, TEXT("��� ������: %d"), ErrCode);
		}
		else if (dwExitCode == STILL_ACTIVE)
		{
			wsprintf(Buff, TEXT("������� �������!"));
			//�������� ������� �����, ����� ������ ����� ����� �������� � ���������� �������
			GetSystemTimeAsFileTime(&ftNow);
			uiTempTimeNow.HighPart = ftNow.dwHighDateTime;
			uiTempTimeNow.LowPart = ftNow.dwLowDateTime;
		}
		else
		{
			wsprintf(Buff, TEXT("%d"), dwExitCode);
			uiTempTimeNow.HighPart = ftEnd.dwHighDateTime;
			uiTempTimeNow.LowPart = ftEnd.dwLowDateTime;
		}


		SetDlgItemText(hDlg, IDC_EXIT_CODE, Buff);

		//��� ���������� ������
		if (!GetExitCodeThread(ThreadHandle[lParam], &dwExitCode))
		{
			DWORD ErrCode = GetLastError();
			wsprintf(Buff, TEXT("��� ������: %d"), ErrCode);
		}
		else if (dwExitCode == STILL_ACTIVE)
			wsprintf(Buff, TEXT("������� �������!"));
		else
			wsprintf(Buff, TEXT("%d"), dwExitCode);

		SetDlgItemText(hDlg, IDC_EXIT_CODE_THREAD, Buff);

		//������������� ������ ���������� ��������
		PriorityCode = GetPriorityClass(ProcHandle[lParam]);
		wsprintf(Buff, TEXT("%d"), PriorityCode);
		SetDlgItemText(hDlg, IDC_CLASS_PRIORITY, Buff);

		//��������� ��������������

		//����� ����� ����������
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
		SetDlgItemText(hDlg, IDC_TIMELIFE, Buff);

		//����� �������
		uiTempTime.HighPart = ftUserTime.dwHighDateTime;
		uiTempTime.LowPart = ftUserTime.dwLowDateTime;
		uiTempTimeNow.HighPart = ftKernelTime.dwHighDateTime;
		uiTempTimeNow.LowPart = ftKernelTime.dwLowDateTime;

		uiCurrent.QuadPart = uiCurrent.QuadPart - uiTempTime.QuadPart - uiTempTimeNow.QuadPart;
		ftCurrent.dwHighDateTime = uiCurrent.HighPart;
		ftCurrent.dwLowDateTime = uiCurrent.LowPart;
		FileTimeToSystemTime(&ftCurrent, &stCurrent);
		wsprintf(Buff, TEXT("%d:%d:%d.%d"), stCurrent.wHour, stCurrent.wMinute, stCurrent.wSecond, stCurrent.wMilliseconds);
		SetDlgItemText(hDlg, IDC_TIMEPAUSE, Buff);

		//����� ���������� � ������ ������������
		FileTimeToSystemTime(&ftUserTime, &stCurrent);
		wsprintf(Buff, TEXT("%d:%d:%d.%d"), stCurrent.wHour, stCurrent.wMinute, stCurrent.wSecond, stCurrent.wMilliseconds);
		SetDlgItemText(hDlg, IDC_TIMEUSER, Buff);

		//����� ���������� � ������ ����
		FileTimeToSystemTime(&ftKernelTime, &stCurrent);
		wsprintf(Buff, TEXT("%d:%d:%d.%d"), stCurrent.wHour, stCurrent.wMinute, stCurrent.wSecond, stCurrent.wMilliseconds);
		SetDlgItemText(hDlg, IDC_TIMECORE, Buff);

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

// ���������� ���� "������� ���������"
BOOL CALLBACK DbWndProc_GetParam(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, DbWndProc_GETPARAM_OnInitDialog);
		HANDLE_MSG(hDlg, WM_COMMAND, DbWndProc_GETPARAM_OnCommand);
		//HANDLE_MSG(hDlg, WM_CLOSE, DbWndProc_GETPARAM_OnClose);

	default:
		return FALSE;
	}

	return FALSE;
}

//��� �������� ���� �������������� ���������
BOOL DbWndProc_GETPARAM_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	HWND hCombo = GetDlgItem(hDlg, IDC_SHOW_STYLE);

	for (int i = 0; i < 4; i++)
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)ComboBoxStrings[i]);

	return 1;
}

void DbWndProc_GETPARAM_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDOK)
	{
		TCHAR buffTxt[100] = { 0 };

		GetDlgItemText(hDlg, IDC_X, buffTxt, 100);
		dwX = _wtoi(buffTxt);

		GetDlgItemText(hDlg, IDC_Y, buffTxt, 100);
		dwY = _wtoi(buffTxt);

		GetDlgItemText(hDlg, IDC_W, buffTxt, 100);
		dwXSize = _wtoi(buffTxt);

		GetDlgItemText(hDlg, IDC_H, buffTxt, 100);
		//strcpy(buffText, item);
		dwYSize = _wtoi(buffTxt);

		for (int i = 0; i < 4; i++)
		{
			GetDlgItemText(hDlg, IDC_SHOW_STYLE, buffTxt, 100);

			if (lstrcmp(buffTxt, ComboBoxStrings[i]) == 0)
			{
				wShowWindow = ComboBoxValues[i];
				break;
			}
		}

		show = true;

		EndDialog(hDlg, TRUE);
		//SendMessage(hDlg, WM_CLOSE, 0, 0);
	}

	else if (id == IDCANCEL)
	{
		show = false;
		EndDialog(hDlg, TRUE);
	}
}

//��� �������� ���� ���������� ��������� ������ � ���������� ����������
//void DbWndProc_GETPARAM_OnClose(HWND hDlg)
//{


//}



// ��������������� �������
//��������� ������ � �������� � �������
void SavingInfoAboutProc(HANDLE pHandle, DWORD pId, HANDLE thHandle, DWORD thId, int index)
{
	ProcHandle[index] = pHandle;
	ProcId[index] = pId;
	ThreadHandle[index] = thHandle;
	ThreadId[index] = thId;
}

//��������� ������� � ������� ���� ��� ����
void OpenNoteApp(HWND hWnd, bool close, int index)
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

	if (!CreateProcess(ProcImage[index], CmdParam[index],
		&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(hWnd, TEXT("������ ������� ��������"), NULL, MB_OK);
		return;
	}
	else
	{
		//��������� �������� ������������ � ���������������
		SavingInfoAboutProc(pi.hProcess, pi.dwProcessId, pi.hThread, pi.dwThreadId, index);

		if (close)
		{
			//��������� ����������� �������� � ������, ����� "��������" �������� ������� �� ������������� ����������
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
}

bool PrintTxtInEdit()
{
	WaitForSingleObject(ProcHandle[4], INFINITE); //�������� ������� �������, ���� ���������� ��������
	hFile = CreateFile(path, GENERIC_READ,
		0, (LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	CHAR TempText[MAX_BYTES] = { 0 }; //����� ��� ������������� ������

									  //������ �� ����� � �����
	DWORD dwNumbOfBytes; //�������� ����� ����������� ������
	ReadFile(hFile, TempText, MAX_BYTES, &dwNumbOfBytes, NULL); //�� ��������� ������

																//��������� ����
	if (hFile)
		CloseHandle(hFile);

	//���������� ���������� ����� � ���� ��������������
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, TempText, -1, BuffText, dwNumbOfBytes); //����������� � ���������� ���������
	BuffText[dwNumbOfBytes] = '\0';
	//LPCSTR result = BuffText;
	//SetDlgItemText(hEdit, IDC_EDIT_BLOCK, BuffText); //�������������� ������
	SendMessage(hEdit, WM_SETTEXT, NULL, (LPARAM)BuffText); //������ ������ ���������� ����� ��������

	return true;
}

bool OpenDlgFile(HWND hWnd)
{
	//��������� ��������� ���������� ���� "������� ����"

	OPENFILENAME ofn;	// ��������� ��� common dialog box
	TCHAR lpszFileSpec[260] = { 0 };	// ������ ��� ����� �����
	HANDLE hFile;	// ���������� �����


					//������������ OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;  // ���������� ��������������
	ofn.lpstrFile = lpszFileSpec;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(lpszFileSpec);
	fileName[0] = '\0';

	ofn.lpstrFilter = _TEXT("All\0*.*\0Text\0*.TXT\0"); 
	ofn.nFilterIndex = 1; // ������ ��� �������� ������� �������
	ofn.lpstrFileTitle = NULL; // ��� ���������
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL; //���� � ���������� �������� ��� �������������� ����� ���������� ��������
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; //������������ ����������� ������� ������ ���������� ����� ������ � ����

													   // ����������� ����������� ����
	if (!GetOpenFileName(&ofn))
		return false;//������ � �������

	wsprintf(path, TEXT("%s"), lpszFileSpec); //���������� ���� � �����
	wsprintf(fileName, TEXT("C:\\Windows\\Notepad.exe %s"), lpszFileSpec); //��������� ��������� ������
	CmdParam[4] = fileName;
	return true;
}