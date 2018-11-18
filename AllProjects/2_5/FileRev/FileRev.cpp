#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"
#include <winuser.h>

//���������� ����������

#define g_lpszClassName _TEXT("sp_pr_class")
#define g_lpszAplicationTitle _TEXT("2-5. FileRev � ����������� �����������")

#define MAX_LOADSTRING 100
#define FILENAME TEXT("FILEREV.DAT")
#define lpszCmdLineT TEXT("Test_FileRev.txt")
// Global Variables:
HANDLE hFile;        // ��� ����������� ������� "����"
HANDLE hFileMap;     // ��� ����������� ������� '������������ ����'
LPVOID lpvFile;      // ��� ������ ������� � �������� ������������
					 // ���� ����� ������������ ����
LPSTR  lpchANSI;     // ��������� �� ANSI ������

DWORD  dwFileSize;   // ��� �������� ������� ����� 
					 //LPTSTR lpszCmdLineT = "Test_FileRev.txt\0"; // ��������� �� ��������� ������ 
					 // � ANSI ��� UNICODE

STARTUPINFO si = { 0 };    // ��������� ��� �������
PROCESS_INFORMATION pi;// CreateProcess 
TCHAR szTitle[MAX_LOADSTRING];					// ����� ���������
TCHAR szWindowClass[MAX_LOADSTRING];			//��� ������ �������� ����

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

//������ � ������������ ������
BOOL CreateFileObj();
BOOL CreateFileProekt();
BOOL ViewToAddress();
BOOL CloseProektToAddress();
BOOL CloseHandleKernel();

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
		//������� ����
	case CREATE_FILE:
		if (!CreateFileObj())
			MessageBox(hWnd, TEXT("������ �������� �����!"), TEXT("������!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("������ ����� ������� ������!"), TEXT("����������"), MB_OK);
			EnableMenuItem(hMenu, CREATE_PR_FILE, MF_ENABLED);
			EnableMenuItem(hMenu, CREATE_FILE, MF_GRAYED);
		}
		break;

		//������� ������������ ����
	case CREATE_PR_FILE:
		if (!CreateFileProekt())
			MessageBox(hWnd, TEXT("������ �������� ������������� �����!"), TEXT("������!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("������������ ���� ������� ������!"), TEXT("����������"), MB_OK);
			EnableMenuItem(hMenu, CREATE_PR_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, CREATE_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, PROJECTION, MF_ENABLED);
		}
		break;

		//��������� �������������
	case PROJECTION:
		if (!ViewToAddress())
			MessageBox(hWnd, TEXT("������ ������������� �����!"), TEXT("������!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("������������� ����� ������ �������!"), TEXT("����������"), MB_OK);
			EnableMenuItem(hMenu, CREATE_PR_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, CREATE_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, PROJECTION, MF_GRAYED);
			EnableMenuItem(hMenu, CLOSE_PROJECTION, MF_ENABLED);
		}
		break;

		//��������� �������� �����
	case CLOSE_PROJECTION:
		if (!CloseProektToAddress())
			MessageBox(hWnd, TEXT("������ �������� �������������!"), TEXT("������!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("�������� ������������� ����� ������ �������!"), TEXT("����������"), MB_OK);
			EnableMenuItem(hMenu, CREATE_PR_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, CREATE_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, PROJECTION, MF_GRAYED);
			EnableMenuItem(hMenu, CLOSE_PROJECTION, MF_GRAYED);
			EnableMenuItem(hMenu, CLOSE_HANDLE, MF_ENABLED);
		}
		break;

		//��������� ������ ��������� ������� � ������
	case CLOSE_HANDLE:
		if (!CloseProektToAddress())
			MessageBox(hWnd, TEXT("������ �������� �������!"), TEXT("������!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("�������� ������� ������ �������!\n������� ��, ����� ������� ���������"), TEXT("����������"), MB_OK);

			// ��������� NOTEPAD � ��������� � ���� ��������� ����,
			// ����� ������� ����� ����� ������
			si.cb = sizeof(si);// ��������� ���� ������� ��������� si
			si.wShowWindow = SW_SHOW;// ������ ����� ������ ���� NOTEPAD
			si.dwFlags = STARTF_USESHOWWINDOW;// ������������� ���� - ���������

			TCHAR buff[266] = { 0 };
			wsprintf(buff, TEXT("%s %s"), TEXT("C:\\Windows\\Notepad.exe"), FILENAME);

			// �������� ���� wShowWindow
			if (CreateProcess(TEXT("C:\\Windows\\Notepad.exe"), buff,
				NULL, NULL, FALSE, 0,
				NULL, NULL, &si, &pi))
			{
				// ���� ������� ������, ����������� 
				// ����������� ������ � ��������	
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			DestroyWindow(hWnd);
		}
		break;

		//� ����������
	case ABOUT_APP:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, DbWndProc_About);
		break;

		//����� �� ���������
	case IDM_FILE_EXIT:
		DestroyWindow(hWnd);

	default:

		break;
	}
}


//������������ ����
//�������� ����� �����
BOOL CreateFileObj()
{
	// ����� �� ��������� ������� ����, �������� ��� � ����� ����,
	// ��� �������� ��������� ����� ������������ ���� ��������� FILENAME.

	if (!CopyFile(lpszCmdLineT, FILENAME, FALSE))
	{
		// ����������� �� �������
		MessageBox(NULL, TEXT("����� ���� �� ����� ���� ������!"),
			TEXT("������"), MB_OK);
		return FALSE;
	}

	// ��������� ���� ��� ������ � ������. ��� ����� ������� ������
	// ���� "����". � ����������� �� ��������� ���������� ������� 
	// CreateFile ���� ��������� ������������ ����, ���� ������� �����.
	// ������ ��� ������� ����� �������������� ��� �������� �����,
	// ������������� � ������. ������� ���������� ���������� ���������
	// ������� ����, ��� ��� ������ INVALID_HANDLE_VALUE.

	hFile = CreateFile(
		FILENAME,  // ��������� �� ������ � ������� �����
		GENERIC_WRITE | GENERIC_READ, // ��������� ����� �������
		0,   //  ����� ����������,0 - ������������� ������ � �����.       
		NULL,// LPSECURITY_ATTRIBUTES=0 - ������ �� �����������.
		OPEN_EXISTING,//���� ���� �� ����������, �� ���������� ������.
		FILE_ATTRIBUTE_NORMAL,//�������� �������� ��� ���� 
		NULL //�� ������ ��� ������� ���� "����"
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{  // ������� ���� �� �������
		MessageBox(NULL, TEXT("�� ������� ������� ����!"),
			TEXT("������"), MB_OK);
		return FALSE;
	}

	// ������ ������ �����. ������ �������� NULL, ��� ��� ��������������,
	// ��� ���� ������ 4 ��.
	dwFileSize = GetFileSize(hFile, NULL);

	return TRUE;
}

//�������� ������������� �����
BOOL CreateFileProekt()
{
	// ������� ������ "������������ ����". �� - �� 1 ���� ������, ���
	// ������ �����, ����� ����� ���� �������� � ����� ����� ������� 
	// ������  � �������� � ������ ��� � ����-��������������� �������.
	// ��������� ���� ��� ���������� �������� ���� ANSI - ��� Unicode
	// - �������, ����������� ���� �� �������� ������� ������� WCHAR

	hFileMap = CreateFileMapping(
		hFile,  // ���������� ������������ ������� "����" 
		NULL,   // 
		PAGE_READWRITE, // �������� ������ ������� 
		0,  // LPSECURITY_ATTRIBUTES
		dwFileSize + sizeof(WCHAR),   //������� 32 �������
		NULL  // � ������� 32 ������� ������� �����.
	);
	if (hFileMap == NULL)
	{	// ������� ������ "������������ ����" �� �������
		MessageBox(NULL, TEXT("�� ������� ������� ������������ ����!"),
			TEXT("������!"), MB_OK);
		CloseHandle(hFile);// ����� ������� ��������� �������� �������
		return FALSE;
	}

	return TRUE;
}

BOOL ViewToAddress()
{
	lpvFile = MapViewOfFile(
		hFileMap, // ���������� ������� "������������ ����" 
		FILE_MAP_WRITE, // ����� �������
		0, // ������� 32 ������� �������� �� ������ �����, 
		0, // ������� 32 ������� �������� �� ������ �����
		0  // � ���������� ������������ ����. 0 - ���� ����.
	);

	if (lpvFile == NULL)
	{// ������������� ������� ������������� ����� �� �������
		MessageBox(NULL, TEXT("�� ������� ������������� ���� �� �������� ������������!"),
			TEXT("������!"), MB_OK);
		CloseHandle(hFileMap);// ����� ������� ��������� �������� �������
		CloseHandle(hFile);
		return FALSE;
	}


	// ���������� � ����� ����� ������� ������
	lpchANSI = (LPSTR)lpvFile;
	lpchANSI[dwFileSize] = 0;

	// "��������������" ���������� ����� ��������
	_strrev(lpchANSI);

	// ����������� ��� ���������� ����������� �������� "\n\r" ������� � "\r\n",
	// ����� ��������� ���������� ������������������ ����� "������� �������"
	// "������� ������", ����������� ������ � ��������� �����

	lpchANSI = strchr(lpchANSI, (int)'\n');   // ������� ����� ������� '\n'
	while (lpchANSI != NULL) //���� �� ������� ��� ������� '\n'
	{
		*lpchANSI = (int)'\r'; // �������� '\n' �� '\r'
		lpchANSI++;           // ����������� ���������
		*lpchANSI = (int)'\n';// �������� '\r' �� '\n' � ����������� ���������
		lpchANSI++;
		lpchANSI = strchr(lpchANSI, (int)'\n');  // ���� ������
	}

	return TRUE;
}

BOOL CloseProektToAddress()
{
	UnmapViewOfFile(lpvFile);
	return TRUE;
}

BOOL CloseHandleKernel()
{
	// ��������� ������� ������ �� ������ ���� "������������ ����"
	CloseHandle(hFileMap);

	// ������� ����������� ����� �������� ������� ����.��� �����
	// ���������� ��������� ����� � ����� �� ������� ����,
	// � ����� ���� ������� ���������� � ���� ����� ����� �����

	SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	// SetEndOfFlle ����� �������� ����� �������� ����������� �������
	// ���� "������������ ����"

	CloseHandle(hFile);// ��������� ������� ������ �� ������ ���� "����"
	return TRUE;
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
