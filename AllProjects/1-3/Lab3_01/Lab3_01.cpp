#include <Windows.h> 
#include <tchar.h>
#include "resource.h"

LPCTSTR g_lpszClassName = TEXT("sp_pr2_class");
LPCTSTR g_lpszAplicationTitle = TEXT("������� ���� ����������. �����������: <�������, �������>");
LPCTSTR g_lpszDestroyMessage = TEXT("��������� ��������� WM_DESTROY, �� ����������� �������� � �������� ������ �����. ��������� ��������� � ����� � ����������� ���� ����������.");
LPCTSTR g_lpszLMBClickMessage = TEXT("��������� ��������� WM_LBUTTONDOWN, ������� ���������� � ���� ��� ������ ����� ������ ����");
LPCTSTR g_lpszPaintMessage = TEXT("��������� ��������� WM_PAINT");
LPCTSTR g_lpszMessageCreate = TEXT("����������� ��������� WM_CREATE");

LRESULT CALLBACK pr1_WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow) {
	HWND hWnd;				// ������������� �������� ���� ����������
	MSG Msg;
	WNDCLASSEX WndClass;

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

	if (!RegisterClassEx(&WndClass)) {
		MessageBox(NULL, TEXT("������ �����������!"), TEXT("������!"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//��������� CreateWindowEx
	//	HWND CreateWindowEx
	//	(
	//		DWORD dwExStyle,			// ���������� ����� ����
	//		LPCTSTR lpClassName,		// ��������� �� ������������������ ��� ������
	//		LPCTSTR lpWindowName,		// ��������� �� ��� ����
	//		DWORD dwStyle,				// ����� ����
	//		int x,						// �������������� ������� ����
	//		int y,						// ������������ ������� ����
	//		int nWidth,					// ������ ����
	//		int nHeight,				// ������ ����
	//		HWND hWndParent,			// ���������� ������������� ��� ���� ������������
	//		HMENU hMenu,				// ���������� ���� ��� ������������� ��������� ����
	//		HINSTANCE hInstance,		// ���������� ���������� ���������� ���������	
	//		LPVOID lpParam 				// ��������� �� ������ �������� ����
	//	);

	hWnd = CreateWindowEx(NULL, WndClass.lpszClassName, g_lpszAplicationTitle, WS_OVERLAPPEDWINDOW,
		600, 300, 500, 300, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		MessageBox(NULL, TEXT("���� �� �������!"), TEXT("������!"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Msg, NULL, 0, 0)) {
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
LRESULT CALLBACK pr1_WndProc(HWND hWnd, UINT Msg_id, WPARAM wParam, LPARAM lParam) {
	HDC hDC;					//������� ��������� ��������� ����������
	switch (Msg_id) {
	case WM_DESTROY:
		//MessageBox(NULL, g_lpszDestroyMessage, TEXT("����������"), MB_OK);
		PostQuitMessage(5);
		break;

		////��������� ��������� WM_LBUTTONDOWN (�������� ����������) ��� ����������� TextOut ��������!!!!!!!!!!!!!
		//	case WM_LBUTTONDOWN:
		//		//��������� ����������� ����
		//		hDC = GetDC(hWnd);
		//		//����� ������ � �������� ������� x=50, y=200
		//		TextOut(hDC, 50, 200, g_lpszLMBClickMessage, lstrlen(g_lpszLMBClickMessage));

		////��������� ��������� WM_LBUTTONDOWN (� ������� ������� ����, TextOut) ��� ����������� TextOut ��������!!!!!!!!!!!!!
		//	case WM_LBUTTONDOWN:
		//		//��������� ����������� ����
		//		hDC = GetDC(hWnd);
		//		POINTS position;		//���������� ���������� �����
		//		//���������� ��������� ���� �� ��������� lParam
		//		position = MAKEPOINTS(lParam); //������ ����������� ��������, ���������� � � � ���������� ����� � POINTS ���������
		//		//����� ������ � ������� �������
		//		TextOut(hDC, position.x, position.y, g_lpszLMBClickMessage, lstrlen(g_lpszLMBClickMessage));
		//		break;

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
		TextOut(hDC, 20, 100, g_lpszPaintMessage, lstrlen(g_lpszPaintMessage));
		EndPaint(hWnd, &ps);
		break;

		//��������� ��������� WM_CREATE � ������� ����������
	case WM_CREATE:
		//MessageBox(NULL, g_lpszMessageCreate, TEXT("����������"), MB_OK | MB_ICONINFORMATION);
		return 0;			// ��� �������� -1, ���� �� ��������, � ����� �� ����������� WM_DESTROY
		break;



	default: return(DefWindowProc(hWnd, Msg_id, wParam, lParam));
	}
	return FALSE;
}
