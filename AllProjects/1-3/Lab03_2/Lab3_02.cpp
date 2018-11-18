#include <Windows.h> 
#include <tchar.h>
#include "resource.h"

static HMENU hMenu;

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

LRESULT CALLBACK pr1_WndProc(HWND, UINT, WPARAM, LPARAM);
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
		600, 300, 500,300,
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
LRESULT CALLBACK pr1_WndProc(HWND hWnd, UINT Msg_id, WPARAM wParam, LPARAM lParam) {
	HDC hDC;					//������� ��������� ��������� ����������
	int wmId, wmEvent;			//���������� ��� ������2 �.9;
	HMENU hFile; //���������� ������� "����"
	HMENU hEdit; //���������� ������� "������"
	HMENU hFloatMenu;
	DWORD xyPos;
	WORD xPos, yPos;
	MENUITEMINFO mItem, mItem0, mItem1; //���������, �������� ���������� � ������� ������������ ����
	switch (Msg_id) {
	case WM_DESTROY:
		//MessageBox(NULL, g_lpszDestroyMessage, TEXT("����������"), MB_OK);
		PostQuitMessage(5);
		break;
		//��������� ��������� WM_LBUTTONDOWN(� ������� ������� ����, DrawText) ��� ����������� DrawOut ��������!!!!!!!!!!!!!
	case WM_LBUTTONDOWN:
		//��������� ����������� ����
		hDC = GetDC(hWnd);
		//���������� ��������� ���� �� ��������� lParam
		POINTS position;
		position = MAKEPOINTS(lParam);
		//������������ ������� ������
		RECT area;
		area.left = position.x;
		area.top = position.y;
		area.right = position.x + 250;
		area.bottom = position.y + 100;
		LPRECT lpArea;
		lpArea = &area;
		//����� ������ � ������� �������
		DrawText(hDC, g_lpszLMBClickMessage, -1, lpArea, DT_LEFT | DT_WORDBREAK);
		break;

		//��������� ��������� WM_PAINT(����� ������ � ������� TextOut)   ��� ����������� ����� �� ��������!!!!!!!!!!!!!
	case WM_PAINT:
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps);
		//TextOut(hDC, 20, 100, g_lpszPaintMessage, lstrlen(g_lpszPaintMessage));
		EndPaint(hWnd, &ps);
		break;

		//������� ������ ���� ����������� �����			//========= �������3.1
	case WM_CREATE:
		hFile = GetSubMenu(hMenu, 0); //��������� ����������� ������� "����"
		hEdit = GetSubMenu(hMenu, 1); //��������� ����������� ������� "������"
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
		break;

		//�������� � ����� ������������ ����			//========= �������3.2
	case WM_RBUTTONDOWN:
		//�������� ������ � ��������� ��������������� ������� � ���� "������"
		MENUITEMINFO mSelect, mCopy; //���������� ��� �������� ��������� ��������������� ������� ��������� ����
		mSelect.cbSize = mCopy.cbSize = sizeof(MENUITEMINFO); //������ ������ ��������� ����
		mSelect.fMask = mCopy.fMask = MIIM_STATE; //������ ����� ��� ���������� ��������
		GetMenuItemInfo(hMenu, IDM_EDIT_SELECT, false, &mSelect); //�������� ��������� ������ "��������" � �������� ����
		GetMenuItemInfo(hMenu, IDM_EDIT_COPY, false, &mCopy); //�������� ��������� ������ "����������" � �������� ����

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
		mItem0.wID = IDM_EDIT_SELECT; //�������������� ������� ������������ ���� ��������� � ���������������� �������� � �������� ����
		mItem1.wID = IDM_EDIT_COPY;
		mItem0.hSubMenu = mItem1.hSubMenu = NULL;

		//��������� ������ �� ����������� ����
		InsertMenuItem(hFloatMenu, 0, true, &mItem0);
		InsertMenuItem(hFloatMenu, 1, true, &mItem1);
		//���������� ����������� ���� �� ������
		TrackPopupMenu(hFloatMenu, TPM_LEFTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN, xPos, yPos, 0, hWnd, NULL);
		break;

		//����� ��������� ��� ��������� ������� �� ����� ����		//========= �������2.11
	case WM_MENUSELECT:
		HDC hdc1;
		TCHAR Buf[300];
		HINSTANCE hInst;
		hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE); //��� �������� GWL_HINSTANCE ������� ���������� ���������� ���������� ����������
		int size;
		size = LoadString(hInst, LOWORD(wParam), Buf, 300); //���������� ���������� ����������, ������ ������� (������ ������� ������, ����������� � ����), ������ ���������� ������ ��� ������, ���������� ������������ ��������
		hdc1 = GetDC(hWnd); //�������� ���������� ������� ������� ����
		RECT rc; //��������� ���������
		GetClientRect(hWnd, &rc); //��������� ��������� ������� ������� ����
		TextOut(hdc1, rc.left, rc.bottom - 30, g_lpszMsgSpace, lstrlen(g_lpszMsgSpace)); //������� � ���� ��������� ������ � ���������
		TextOut(hdc1, rc.left, rc.bottom - 30, Buf, lstrlen(Buf)); //����� ������ ��� ��������� ������� ������ (����� ������� ������ �� ����������� ��������)
		ReleaseDC(hWnd, hdc1); //������������ ��������� ����������
		break;

		//��������� ��������� ����
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam); // ������� ����� ���������� ����� ����
		wmEvent = HIWORD(wParam); //������� ����� ���������� ������������ ������� - ����
		TCHAR lpszFinalString[500] = { 0 };
		TCHAR lpszString[500] = { 0 };

		switch (wmId)
		{
		case IDM_FILE_NEW:
			//��������� ������ ���� "��������"
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

		case IDM_INFO_PROGRAMM:
			ShowMessageAboutMenuItem(hWnd, IDM_INFO_PROGRAMM, lpszString, lpszFinalString);
			break;

		case IDM_FILE_EXIT:
			MessageBox(NULL, g_lpszDestroyMessage, _TEXT("����������"), MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
			break;

			//��������� ������ ���� "����������"
		case IDM_EDIT_SELECT: //��������� ������ "��������"
			hEdit = GetSubMenu(hMenu, 1);
			EnableMenuItem(hEdit, IDM_EDIT_COPY, MFS_ENABLED | MF_BYCOMMAND); //�������� ����� "����������"
			ShowMessageAboutMenuItem(hWnd, IDM_EDIT_SELECT, lpszString, lpszFinalString);
			break;

			//���������� ������� ���� "��������" � "����������"
		case IDM_CLOSE: //��������� ������ "������� ��������"
			hEdit = GetSubMenu(hMenu, 1);
			EnableMenuItem(hEdit, IDM_EDIT_SELECT, MFS_GRAYED | MF_BYCOMMAND); //������ �����, ����� ��� ������ ������������
			EnableMenuItem(hEdit, IDM_EDIT_COPY, MFS_GRAYED | MF_BYCOMMAND);
			ShowMessageAboutMenuItem(hWnd, IDM_CLOSE, lpszString, lpszFinalString);
			break;

		default:
			wsprintf(lpszFinalString, _TEXT("������� � ��������������� \"%d\" �� �����������."), wmId);
			MessageBox(hWnd, lpszFinalString, _TEXT("��������� ��� ������� �� ����� ����"), MB_OK);
			break;
		}
	}



	default: return(DefWindowProc(hWnd, Msg_id, wParam, lParam));
	}
	return FALSE;
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