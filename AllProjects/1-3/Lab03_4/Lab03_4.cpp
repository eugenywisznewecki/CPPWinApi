#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include <windowsx.h>

static HMENU hMenu;
static HACCEL hAccel; //���������� ��� ������� ������������� (������� ������)

LPCTSTR g_lpszClassName = TEXT("sp_pr2_class");
LPCTSTR g_lpszAplicationTitle = TEXT("������� ���� ����������. �����������: <�������, �������>");
LPCTSTR g_lpszDestroyMessage = TEXT("��������� ��������� WM_DESTROY, �� ����������� �������� � �������� ������ �����. ��������� ��������� � ����� � ����������� ���� ����������.");
LPCTSTR g_lpszLMBClickMessage = TEXT("��������� ��������� WM_LBUTTONDOWN, ������� ���������� � ���� ��� ������ ����� ������ ����");
LPCTSTR g_lpszPaintMessage = TEXT("��������� ��������� WM_PAINT");
LPCTSTR g_lpszMessageCreate = TEXT("����������� ��������� WM_CREATE");
LPCTSTR g_lpszMsgSpace = TEXT("                                                    ");
LPTSTR g_lpszTextMenu = _TEXT("������� ��������");
#define g_lpszCommandMessage _TEXT("������� ������� \"")
#define g_lpszCommandMessageDefault _TEXT("������� � ��������������� \"")

#define	IDM_CLOSE	40021

//���������� �������� ������� �������
LRESULT CALLBACK pr1_WndProc(HWND, UINT, WPARAM, LPARAM);
//void ShowMessageAboutMenuItem(HWND, UINT, LPWSTR, LPWSTR);

//���������� ��������������� ������� ��� ��������� ��������� ����� HANDLE_MSG
bool pr1_WndProc_OnCreate(HWND, LPCREATESTRUCT);
void pr1_WndProc_OnPaint(HWND);
void pr1_WndProc_OnLButtonDown(HWND, BOOL, int, int, UINT);
void pr1_WndProc_OnRButtonDown(HWND, BOOL, int, int, UINT);
void pr1_WndProc_OnMenuSelect(HWND, HMENU, int, HMENU, UINT);
void pr1_WndProc_OnDestroy(HWND);
void pr1_WndProc_OnCommand(HWND, int, HWND, UINT);
void ShowMessageAboutMenuItem(HWND, UINT, LPWSTR, LPWSTR);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow) {
	WNDCLASSEX WndClass;
	HWND hWnd;				// ������������� �������� ���� ����������
	MSG Msg;
	HACCEL hAccel;			//���������� ��� ������� ������������� (������� ������)

	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));							//������� ��������� ���� ��� ����������� ��� � ��������� ����� ������� �������� ����
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR_YUSH)); //������� ��������� ������ ������������� � ���������� //========= �������2.10

	memset(&WndClass, 0, sizeof(WNDCLASSEX));
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.lpszClassName = g_lpszClassName;
	WndClass.lpfnWndProc = pr1_WndProc;
	WndClass.style = CS_VREDRAW | CS_HREDRAW;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));			//��������� ���������� ������ ����������
	WndClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));		//������� ��������� ���� ��� ����������� ��� � ��������� ����� ������� �������� ����
	WndClass.hbrBackground = CreateSolidBrush(RGB(192, 192, 192));
	WndClass.lpszMenuName = NULL;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	//========= �������2. 8.����������� ����.
	//a)		���������� ���� � ��������� ����� ��������� ������ ����.
	//WndClass.lpszMenuName = MAKEINTRESOURCE (IDR_MENU1);						//����������� ���� ����� ��������� ������ ����

	if (!RegisterClassEx(&WndClass)) {
		MessageBox(NULL, TEXT("������ �����������!"), TEXT("������!"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//========= �������2. 8.����������� ����.
	//�) ���������� ���� � ��������� ����� ������� �������� ���� CreateWindowEx : ���� ������� ����������� ��� ������ ������� LoadMenu :
	hWnd = CreateWindowEx(NULL, g_lpszClassName, g_lpszAplicationTitle, WS_OVERLAPPEDWINDOW - WS_MINIMIZEBOX,
		600, 300, 500, 300,
		NULL, hMenu,			//����������� ���� ����� ������� �������� ����
		hInstance, NULL);

	if (!hWnd) {
		MessageBox(NULL, TEXT("���� �� �������!"), TEXT("������!"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Msg, NULL, 0, 0))								//========= �������2.10 	
	{
		if (!hAccel || !TranslateAccelerator(hWnd, hAccel, &Msg))		//���� ���������� � ������������� �� ����� ���� � �������� ��������� �� ����������
																		//��� ������������� � ��������� ���� WM_COMMAND � ���������� �������� ���� ����� ������� ���������
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

	DestroyAcceleratorTable(hAccel); //���������� ������� ������-�����������. ������, ��� ���������� ���������, ��� ������ ������������ ��� �������, ����� ���������� ������ ������� ������-�����������, ������� �����������

	return Msg.wParam;
}

LRESULT CALLBACK pr1_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) //��������� HANDLE_MSG ������ ������� ������� ���������� � ���� �������������
{
	switch (Msg)
	{
		//���������� HANDLE_MSG
		HANDLE_MSG(hWnd, WM_PAINT, pr1_WndProc_OnPaint); //��������������� �� case WM_PAINT: return HANDLE_WM_PAINT(hWnd, WM_PAINT, lParam, pr1_WndProc_OnPaint)
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, pr1_WndProc_OnLButtonDown);
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, pr1_WndProc_OnRButtonDown);
		HANDLE_MSG(hWnd, WM_CREATE, pr1_WndProc_OnCreate);
		HANDLE_MSG(hWnd, WM_MENUSELECT, pr1_WndProc_OnMenuSelect);
		HANDLE_MSG(hWnd, WM_DESTROY, pr1_WndProc_OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, pr1_WndProc_OnCommand);

	default:
		return(DefWindowProc(hWnd, Msg, wParam, lParam));
	}

	return FALSE;
}

//������� ��� ������������� � HANDLE_MSG
void pr1_WndProc_OnPaint(HWND hWnd)
{
	HDC hDC;
	PAINTSTRUCT ps;
	hDC = BeginPaint(hWnd, &ps);
	TextOut(hDC, 20, 100, g_lpszPaintMessage, lstrlen(g_lpszPaintMessage));
	EndPaint(hWnd, &ps);
}

void pr1_WndProc_OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	HDC hDC = GetDC(hWnd);
	//POINTS position;
	//position = MAKEPOINTS(lParam);
	RECT area;
	area.left = 50;
	area.top = 200;
	area.right = 900;
	area.bottom = 300;
	LPRECT lpArea;
	lpArea = &area;
	//TextOut( hDC, 50, 200, lpszMessageMouse, lstrlen(lpszMessageMouse) );
	//TextOut( hDC, position.x, position.y, lpszMessageMouse, lstrlen(lpszMessageMouse) );
	DrawText(hDC, g_lpszLMBClickMessage, -1, lpArea, DT_CENTER);
}

void pr1_WndProc_OnRButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	//�������� ������ � ��������� ��������������� ������� � ���� "������"
	HMENU hFloatMenu;
	DWORD xyPos;
	WORD xPos, yPos;
	MENUITEMINFO mItem0, mItem1; //���������, �������� ���������� � ������� ������������ ����
	MENUITEMINFO mSelect, mCopy;

	mSelect.cbSize = mCopy.cbSize = sizeof(MENUITEMINFO); //������ ������ ��������� ����
	mSelect.fMask = mCopy.fMask = MIIM_STATE; //������ ����� ��� ���������� ��������
	GetMenuItemInfo(hMenu, IDM_EDIT_SELECT, false, &mSelect); //�������� ��������� ������ "��������"
	GetMenuItemInfo(hMenu, IDM_EDIT_COPY, false, &mCopy); //�������� ��������� ������ "����������"

													 //�������� �������� ���������� ������� ������ ������� ������������ ������ �������� ���� ����
	xyPos = GetMessagePos();
	xPos = LOWORD(xyPos);
	yPos = HIWORD(xyPos);
	hFloatMenu = CreatePopupMenu(); //������� ����������� ����

									//���������� �������� ��� ������� ������ ������������ ����
	mItem0.cbSize = mItem1.cbSize = sizeof(MENUITEMINFO);
	mItem0.fMask = mItem1.fMask = MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
	mItem0.fType = mItem1.fType = MFT_STRING;
	mItem0.fState = mSelect.fState; //����������� ��������� ������� ������ ������������ ����
	mItem1.fState = mCopy.fState; //����������� ��������� ������� ������ ������������ ����
	mItem0.dwTypeData = _TEXT("��������");
	mItem1.dwTypeData = _TEXT("����������");
	mItem0.cch = sizeof(_TEXT("��������"));
	mItem1.cch = sizeof(_TEXT("����������"));
	mItem0.wID = IDM_EDIT_SELECT;
	mItem1.wID = IDM_EDIT_COPY;
	mItem0.hSubMenu = mItem1.hSubMenu = NULL;

	//��������� ������ �� ����������� ����
	InsertMenuItem(hFloatMenu, 0, true, &mItem0);
	InsertMenuItem(hFloatMenu, 1, true, &mItem1);
	//���������� ����������� ���� �� ������
	TrackPopupMenu(hFloatMenu, TPM_LEFTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN, xPos, yPos, 0, hWnd, NULL);
}

bool pr1_WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT ipCreateStruct)
{
	HMENU hFile; //��� ����������� ������� "����"
	HMENU hEdit; //��� ����������� ������� "������"
	MENUITEMINFO mItem;

	hFile = GetSubMenu(hMenu, 0); //��������� ����������� ������� "����"
	hEdit = GetSubMenu(hMenu, 1);

	mItem.cbSize = sizeof(MENUITEMINFO);
	mItem.fMask = MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
	mItem.fType = MFT_STRING;
	mItem.fState = MFS_ENABLED;
	mItem.dwTypeData = g_lpszTextMenu;
	mItem.cch = sizeof(g_lpszTextMenu);
	mItem.wID = IDM_CLOSE;
	mItem.hSubMenu = NULL;

	InsertMenuItem(hFile, 2, true, &mItem); //������� ����� ������ �������

	MessageBox(NULL, g_lpszMessageCreate, _TEXT("����������"), MB_OK | MB_ICONINFORMATION);

	return 1;
}

void pr1_WndProc_OnMenuSelect(HWND hWnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags)
{
	HDC hDc;
	TCHAR Buf[300];
	HINSTANCE hInst;
	hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE); //��� �������� GWL_HINSTANCE ������� ���������� ���������� ���������� ����������
	int size;
	size = LoadString(hInst, item, Buf, 300); //���������� ���������� ����������, ������ ������� (������ ������� ������, ����������� � ����), ������ ���������� ������ ��� ������, ���������� ������������ ��������
	hDc = GetDC(hWnd); //�������� ���������� ������� ������� ����
	RECT rc; //��������� ���������
	GetClientRect(hWnd, &rc); //��������� ��������� ������� ������� ����
	TextOut(hDc, rc.left, rc.bottom - 30, g_lpszMsgSpace, lstrlen(g_lpszMsgSpace)); //������� � ���� ��������� ������ � ���������
	TextOut(hDc, rc.left, rc.bottom - 30, Buf, lstrlen(Buf)); //����� ������ ��� ��������� ������� ������ (����� ������� ������ �� ����������� ��������)
	ReleaseDC(hWnd, hDc); //������������ ��������� ����������
}

void pr1_WndProc_OnDestroy(HWND hWnd)
{
	MessageBox(NULL, g_lpszDestroyMessage, _TEXT("������"), MB_OK | MB_ICONERROR);
	FORWARD_WM_DESTROY(hWnd, DefWindowProc);
	PostQuitMessage(0);
}

void pr1_WndProc_OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify) //id - ����� ����, codeNotify - �������(��������, ����)
{
	HMENU hEdit;
	TCHAR lpszFinalString[500] = { 0 };
	TCHAR lpszString[500] = { 0 };

	switch (id)
	{
	case IDM_FILE_NEW:
		hEdit = GetSubMenu(hMenu, 1); //�������� ���������� ������� "������"
		EnableMenuItem(hEdit, IDM_EDIT_SELECT, MFS_ENABLED | MF_BYCOMMAND); //�������� ����� "��������"
		ShowMessageAboutMenuItem(hWnd, IDM_FILE_NEW, lpszString, lpszFinalString);
		break;

	case IDM_FILE_OPEN:
		ShowMessageAboutMenuItem(hWnd, IDM_FILE_OPEN, lpszString, lpszFinalString);
		break;

	case IDM_INFO_HELP:
		ShowMessageAboutMenuItem(hWnd, IDM_INFO_HELP, lpszString, lpszFinalString);
		break;

	case IDM_INFO_PROGRAMM: //���������������� ��� �������� default
		ShowMessageAboutMenuItem(hWnd, IDM_INFO_PROGRAMM, lpszString, lpszFinalString);
		break;

	case IDM_FILE_EXIT:
		MessageBox(NULL, g_lpszDestroyMessage, _TEXT("������"), MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		break;

	case IDM_EDIT_SELECT: //��������� ������ "��������"
		hEdit = GetSubMenu(hMenu, 1);
		EnableMenuItem(hEdit, IDM_EDIT_COPY, MFS_ENABLED | MF_BYCOMMAND); //�������� ����� "����������"
		ShowMessageAboutMenuItem(hWnd, IDM_EDIT_SELECT, lpszString, lpszFinalString);
		break;

	case IDM_CLOSE: //��������� ������ "������� ��������"
		hEdit = GetSubMenu(hMenu, 1);
		EnableMenuItem(hEdit, IDM_EDIT_SELECT, MFS_GRAYED | MF_BYCOMMAND); //������ �����, ����� ��� ������ ������������
		EnableMenuItem(hEdit, IDM_EDIT_COPY, MFS_GRAYED | MF_BYCOMMAND);
		ShowMessageAboutMenuItem(hWnd, IDM_CLOSE, lpszString, lpszFinalString);
		break;

	default:
		wsprintf(lpszFinalString, _TEXT("������� � ��������������� \"%d\" �� �����������."), id);
		MessageBox(hWnd, lpszFinalString, _TEXT("��������� ��� ������� �� ����� ����"), MB_OK);
		break;
	}
}
//������� ��� ������ ��������� � ���������� ������ ����
void ShowMessageAboutMenuItem(HWND hWnd, UINT IDM_MENU_ITEM, LPWSTR lpszString, LPWSTR lpszFinalString)
{
	GetMenuString(hMenu, IDM_MENU_ITEM, lpszString, 50, MF_BYCOMMAND);
	lstrcat(lpszFinalString, g_lpszCommandMessage);
	lstrcat(lpszFinalString, lpszString);
	lstrcat(lpszFinalString, _TEXT("\"."));
	MessageBox(hWnd, lpszFinalString, _TEXT("��������� ��� ������� �� ����� ����"), MB_OK);
}