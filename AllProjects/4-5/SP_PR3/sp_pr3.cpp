#include <windows.h> 
#include <windowsx.h> // HANDLE_MSG
#include <tchar.h>	
#include "resource.h"
#include "sp_pr3.h"
#include <Strsafe.h> // ��� 'StringCbPrintf'

HMENU hMenu, hEditMenu, hFloatMenu; 

HWND hWnd;					  // ���������� ��������������
HINSTANCE hinstance;		  // ��� 5, ��� "�����" � "�������� ����������" 

//�������� ���������� 1.3.3
static HWND hEdit, hListBox;  // ��� 5, ��� "�������� ����������" -> ��������� �� ������ ��� ���������� ������ �� ����������� ����
//static TCHAR gpszTextBuff[100];
static TCHAR EditBuff[20]; 

static TCHAR gpszTextBuff[10][100];	//
static int cnt;						//

//�������� ���������� 1.3.4
OPENFILENAME ofn;   // ��������� ��� common dialog box
HANDLE hFile;       // ���������� �����
DWORD dwNumbOfBytes = MAX_BYTES;	// ����� ��� ������  
TCHAR Buffer[MAX_BYTES];			// ����� ��� ������ 
BOOL fRet;				   // ��� '���������� ����'
TCHAR lpszFileSpec[100];   // ������ ��� ����� �����

//��������� ������� ��������� ��������� � ���������������� ���������
LRESULT CALLBACK Pr2_WndProc(HWND, UINT, WPARAM, LPARAM);					// <�������>
BOOL CALLBACK Text(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam); // �����
BOOL CALLBACK ElementyUpravlenija(HWND, UINT, WPARAM, LPARAM);				// �������� ����������
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);							// � ���������...

// HANDLE_MSG 
BOOL Cls_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void Cls_OnPaint(HWND hwnd);
void Cls_OnDestoy(HWND);																		 
void Cls_OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);

BOOL CreateMenuItem(HMENU hMenu, LPWSTR str, UINT u, UINT uc, HMENU hSub, BOOL flag, UINT type); // <<<=== ��������� ����



//����
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{
	//HWND hWnd;
	MSG msg;
	WNDCLASSEX WndClass;
	HACCEL hAccel; // <<<=== �����������
	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)); // 8.����������� ����: �) <<<=== ����

	memset(&WndClass, 0, sizeof(WNDCLASSEX));
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.hInstance = hInstance;
	WndClass.lpszClassName = TEXT("SimpleClassName1_5");
	WndClass.lpfnWndProc = Pr2_WndProc;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // <<<=== ������
	WndClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_POINTER)); // <<<=== ������
	WndClass.lpszMenuName = NULL; //(LPCTSTR)IDR_MENU1;  // 8.����������� ����: �) <<<=== ����
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = CreateSolidBrush(RGB(192, 192, 192));
	WndClass.hIconSm = NULL;

	if (!RegisterClassEx(&WndClass))
	{
		MessageBox(NULL, TEXT("������ ����������� ������ ����!"),
			TEXT("������"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hWnd = CreateWindowEx(NULL,
		TEXT("SimpleClassName1_5"), 
		TEXT("������� ���� ����������. �����������: <�������, �������>"),
		WS_OVERLAPPEDWINDOW,
		600, 300,		//������ ��������� � � � ����� ������������ CW_USEDEFAULT - �������� �� ��������� 
		600, 450,
		HWND_DESKTOP,
		LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)), // 8.����������� ����: �) <<<=== ����
		hInstance,
		NULL);
	if (!hWnd)
	{
		MessageBox(NULL, TEXT("������ ������� ����!"),
			TEXT("������"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// ����������� 
	hAccel = LoadAccelerators(hInstance,
		MAKEINTRESOURCE(IDR_ACCELERATOR1));
	// + ���� ��������� ���������
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}


//�������
LRESULT CALLBACK Pr2_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	hMenu = GetMenu(hWnd);	// ������� 3.1; ������� ����������� ����� ������� �������� �������� � ���� �����  <<<=== ����	
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hWnd, WM_PAINT, Cls_OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, Cls_OnDestoy); // ������� �� �������� (HANDLE_MSG)
		HANDLE_MSG(hWnd, WM_COMMAND, Cls_OnCommand);

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0; //return FALSE;
}


BOOL Cls_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	HMENU addSubMenuToFileMenu;
	addSubMenuToFileMenu = GetSubMenu(hMenu, 0);
// �������� ���������� 1.3.3 //�������� ������
	if (!(hListBox = CreateWindow(
		//0,	
		L"ListBox",
		NULL, //L"Edit"
		WS_CHILD | WS_VISIBLE | WS_DISABLED,
		10, 10,
		160, 380,
		hWnd,
		NULL,//(HMENU)(IDC_OKd3),
		hinstance,
		NULL
	)))
		return (-1);

// �������� ���������� 1.3.4
	//������������ OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;														
	ofn.lpstrFile = lpszFileSpec;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(lpszFileSpec);
	// ������������ ������� ����� �������� �������
	ofn.lpstrFilter = L"��� �����\0*.*\0��������� ��������� (*.txt)\0*.TXT\0";
	ofn.nFilterIndex = 1;		// ������ ��� �������� ������� �������
	ofn.lpstrFileTitle = NULL;	// ��� ���������
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	return TRUE;
}

void Cls_OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC;
	hDC = BeginPaint(hWnd, &ps);

	EndPaint(hWnd, &ps);
}

void Cls_OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
int wmId;
wmId = LOWORD(id);

HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);								// ��� "DialogBox" (� ���������...)
switch (wmId) 
{
	// ���� -> ����
case IDM_FILE_EXIT:
	SendMessage(hWnd, WM_DESTROY, 0, 0);
	break;
	// ���� -> ��������
case IDM_VIEW_TEXT: // ����� 
	DialogBox(hinstance, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, Text);								// ��� "�����"
	break;

case IDM_VIEW_CTL:																				

	//DialogBox(hinstance, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, ElementyUpravlenija);				// ��� "�������� ����������"
	
	DialogBox((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG2), hWnd, (DLGPROC)ElementyUpravlenija);
	ListBoxContent(cnt);	//
	break;

	// ���� -> �������
case IDM_INFO_ABOUT: // � ���������...
	{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)About);
	}
	break;
	//-------------------------------------------------------------------
case IDM_INFO_HELP:
	MessageBox(hWnd, _T("������ ����� 'HELP'"), _T("���� Info"), MB_OK);
	break;

default:
	TCHAR buf_for_MB[100];				// 9.��������� �� ���������� ������ � case WM_COMMAND: c ������ ���� �������������� <<<=== ����	
	WORD CommandCaseId = LOWORD(id);
	StringCbPrintf(buf_for_MB, 101, L"�������  � ��������������� '%d' �� �����������!", CommandCaseId);
	MessageBox(hWnd, buf_for_MB, TEXT("������"), MB_OK);
	}
	return FORWARD_WM_COMMAND(hWnd, id, hwndCtl, codeNotify, DefWindowProc);
}

BOOL CALLBACK Text(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam) // ����� - ��������� ����
{
	LPCWSTR lpszInitEdit = TEXT("��� 1. ������ ������");

	HWND hEdit = NULL;
	hEdit = GetDlgItem(hDlg, IDC_EDIT1); // ��������� � ����������� [����] ������������� [��������]

	HANDLE hFile = NULL;

	switch (uMessage)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), lpszInitEdit);	// ����� "��� 1. ������ ������"
		return TRUE;
	case WM_COMMAND:
	{
		int wmId, wmEvent;
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDC_LOAD:								// ��� ������ '���������'
			lstrcat(Buffer, TEXT("���� ���� 2"));	// ����� "���� ���� 2"
			SetWindowText(hEdit, Buffer);

			/* Open file */
			fRet = GetOpenFileName(&ofn);
			if (fRet == FALSE) return 1;
			hFile = CreateFile(ofn.lpstrFile, GENERIC_READ,
				0, (LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
				(HANDLE)NULL);
			if (hFile == INVALID_HANDLE_VALUE) return (-1);
			ReadFile(hFile, Buffer, MAX_BYTES, &dwNumbOfBytes, NULL);

			lstrcat(Buffer, TEXT(""));				// ��������������� ������� ������
			SetWindowText(hEdit, Buffer);			// ����� ������ � ""EditList"

			if (hFile) CloseHandle(hFile);			// �������� �����

			return TRUE;

		case IDC_OKd3:
			EndDialog(hDlg, TRUE);
			return true;

		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return TRUE;
	
		default:
			return true;
		}
	}
	break;
	default:
		return(FALSE);
	}
	return true;
}

BOOL CALLBACK ElementyUpravlenija(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) // �������� ���������� - ��������� ����
{
	static HWND hWndList, hWndEdit;
	
	LPCTSTR step1 = TEXT("������ 1");

	switch (msg)
	{ 
	case WM_INITDIALOG:			// ������������� ���������� ������
	{
		cnt = 0;	//
		ZeroMemory(EditBuff, sizeof(EditBuff));
		
		hWndEdit = GetDlgItem(hdlg, ID_EDITd2);
		hWndList = GetDlgItem(hdlg, ID_LISTBOXd2);

		SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)step1);

		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) //LOWORD(wParam)
		{
		case IDC_ADDBUFFER:
		{
			int cch;
			cch = SendMessage(hWndEdit, WM_GETTEXT, (WPARAM)100, (LPARAM)EditBuff);  //��������� ���� ���������, ��� ���������, ������ ������, ���������� � ����� �����

			if (cch == 0) //���� ������ �� ��������� (���� ��������� ������)
				MessageBox(hWndEdit, L"�� �� ����� ����� � ���� Edit", L"������!", MB_OK);//���� ��������� � �������� ������ �����
			else
			{
				TCHAR Buff1[500] = { '\0' }; //�������� ������ ������
				SYSTEMTIME systime; //��������� ��� ���������� �������
				GetLocalTime(&systime);			// �������� ����� �� �������
				wsprintf(Buff1, _TEXT("����� : %d � %d ��� %d ���\n"),
					systime.wHour, systime.wMinute, systime.wSecond); //������� ����� ������������ � �����
				lstrcat(Buff1, __TEXT("����� � ������: "));
				lstrcat(Buff1, EditBuff); //���������� �����
				MessageBox(hdlg, Buff1, _TEXT("���������� ������"), MB_OK);//����� ��������� ������
			}
		}break;

		case IDC_ADDLIST:
		{
			if (EditBuff[0] == '\0') //���� ������ ������
				MessageBox(hdlg, _TEXT("������ �������� ������ ������!"), _TEXT("������!"), MB_OK);
			else
			{
				int ind;
				ind = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)EditBuff); //�������� �������� ������ � ������

				if (ind == LB_ERR)
					MessageBox(hWnd, _TEXT("������ � ������"), _TEXT("������!"), MB_OK); //���� �� ������� �������� - ������
			}
		}break;
		// ��������� �� ������ "OK"
		case IDC_YES:

			cnt = SendMessage(hWndList, LB_GETCOUNT, 0, 0L);	//
			for (int i = 0; i < cnt; i++)						//
			{													//
				SendMessage(hWndList, LB_GETTEXT, i,			//
					(LPARAM)gpszTextBuff[i]);					//
			}													//
			EndDialog(hdlg, IDOK);								//
			
			/*memcpy(gpszTextBuff, EditBuff, sizeof(EditBuff));
			EndDialog(hdlg, IDC_YES);*/
			return TRUE;

		case IDC_CANCEL:
			EndDialog(hdlg, TRUE);
			return true;

			// ��������� �� ������ "�������"
		case IDCANCEL:
			//ZeroMemory(gpszTextBuff, sizeof(gpszTextBuff));
			EndDialog(hdlg, TRUE);
			return true;
		}
	}
	}
	return FALSE;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) // � ���������... - ��������� ����
{
	static HICON hIconDialog;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		SYSTEMTIME systime;
		GetLocalTime(&systime);			// �������� ����� �� �������
		TCHAR Time[64] = TEXT("");
		TCHAR Date[64] = TEXT("");
		wsprintf(Date, TEXT("������� ����: %02d.%02d.%d"), systime.wDay, systime.wMonth, systime.wYear);
		wsprintf(Time, TEXT("�����:               %02d:%02d"), systime.wHour, systime.wMinute);

		SetDlgItemText(hDlg, IDC_STATIC1, TEXT("������������ ������ � 5"));
		SetDlgItemText(hDlg, IDC_STATIC2, TEXT("���������� ����. �������� ���������� � ���������� �����.\n\n����������� - ������� �."));
		SetDlgItemText(hDlg, IDC_STATIC3, Date);
		SetDlgItemText(hDlg, IDC_STATIC4, Time);
		hIconDialog = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIconDialog);
	}
	return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_OKd1 || LOWORD(wParam) == IDCANCEL) // ������������ ������ 'OK'
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Cls_OnDestoy(HWND) // ������� ��������� ��������� WM_DESTROY: (HANDLE_MSG)
{
	PostQuitMessage(0);
}

void ListBoxContent(int count)												//
{																			//
	for (int i = 0; i < count; i++)											//
	{																		//
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(gpszTextBuff[i]));	//				
	}																		//
}																			//