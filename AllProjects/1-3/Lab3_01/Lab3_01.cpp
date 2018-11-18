#include <Windows.h> 
#include <tchar.h>
#include "resource.h"

LPCTSTR g_lpszClassName = TEXT("sp_pr2_class");
LPCTSTR g_lpszAplicationTitle = TEXT("Главное окно приложения. Программист: <Юшкевич, Евгений>");
LPCTSTR g_lpszDestroyMessage = TEXT("Поступило сообщение WM_DESTROY, из обработчика которого и выполнен данный вывод. Сообщение поступило в связи с разрушением окна приложения.");
LPCTSTR g_lpszLMBClickMessage = TEXT("Обработка сообщения WM_LBUTTONDOWN, которое посылается в окно при щелчке левой кнопки мыши");
LPCTSTR g_lpszPaintMessage = TEXT("Обработка сообщения WM_PAINT");
LPCTSTR g_lpszMessageCreate = TEXT("Выполняется обработка WM_CREATE");

LRESULT CALLBACK pr1_WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow) {
	HWND hWnd;				// идентификатор главного окна приложения
	MSG Msg;
	WNDCLASSEX WndClass;

	memset(&WndClass, 0, sizeof(WNDCLASSEX));
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.lpszClassName = g_lpszClassName;
	WndClass.lpfnWndProc = pr1_WndProc;
	WndClass.style = CS_VREDRAW | CS_HREDRAW;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));			//подгрузка созданного значка приложения
	WndClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));		//функция формирует меню для подключения его к программе через функцию создания окна
	WndClass.hbrBackground = CreateSolidBrush(RGB(192, 192, 192));
	WndClass.lpszMenuName = NULL;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;

	if (!RegisterClassEx(&WndClass)) {
		MessageBox(NULL, TEXT("Ошибка регистрации!"), TEXT("Ошибка!"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//Синтаксис CreateWindowEx
	//	HWND CreateWindowEx
	//	(
	//		DWORD dwExStyle,			// улучшенный стиль окна
	//		LPCTSTR lpClassName,		// указатель на зарегистрированное имя класса
	//		LPCTSTR lpWindowName,		// указатель на имя окна
	//		DWORD dwStyle,				// стиль окна
	//		int x,						// горизонтальная позиция окна
	//		int y,						// вертикальная позиция окна
	//		int nWidth,					// ширина окна
	//		int nHeight,				// высота окна
	//		HWND hWndParent,			// дескриптор родительского или окна собственника
	//		HMENU hMenu,				// дескриптор меню или идентификатор дочернего окна
	//		HINSTANCE hInstance,		// дескриптор экземпляра прикладной программы	
	//		LPVOID lpParam 				// указатель на данные создания окна
	//	);

	hWnd = CreateWindowEx(NULL, WndClass.lpszClassName, g_lpszAplicationTitle, WS_OVERLAPPEDWINDOW,
		600, 300, 500, 300, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		MessageBox(NULL, TEXT("Окно не создано!"), TEXT("Ошибка!"), MB_OK | MB_ICONERROR);
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
	HDC hDC;					//создадём экземпляр контекста устройства
	switch (Msg_id) {
	case WM_DESTROY:
		//MessageBox(NULL, g_lpszDestroyMessage, TEXT("Оповещение"), MB_OK);
		PostQuitMessage(5);
		break;

		////Обработка сообщения WM_LBUTTONDOWN (заданные координаты) при перерисовке TextOut Исчезает!!!!!!!!!!!!!
		//	case WM_LBUTTONDOWN:
		//		//получение дескриптора окна
		//		hDC = GetDC(hWnd);
		//		//вывод текста в заданной позиции x=50, y=200
		//		TextOut(hDC, 50, 200, g_lpszLMBClickMessage, lstrlen(g_lpszLMBClickMessage));

		////Обработка сообщения WM_LBUTTONDOWN (в позиции курсора мыши, TextOut) при перерисовке TextOut Исчезает!!!!!!!!!!!!!
		//	case WM_LBUTTONDOWN:
		//		//получение дескриптора окна
		//		hDC = GetDC(hWnd);
		//		POINTS position;		//определяет координаты точки
		//		//извлечение координат мыши из параметра lParam
		//		position = MAKEPOINTS(lParam); //макрос преобразует значение, содержащее х и у координаты точки в POINTS структуру
		//		//вывод текста в позиции курсора
		//		TextOut(hDC, position.x, position.y, g_lpszLMBClickMessage, lstrlen(g_lpszLMBClickMessage));
		//		break;

		//Обработка сообщения WM_LBUTTONDOWN(в позиции курсора мыши, DrawText) при перерисовке DrawOut Исчезает!!!!!!!!!!!!!
	case WM_LBUTTONDOWN:
		//получение дескриптора окна
		hDC = GetDC(hWnd);
		//извлечение координат мыши из параметра lParam
		POINTS position;
		position = MAKEPOINTS(lParam);
		//формирование области вывода
		RECT area;
		area.left = position.x;
		area.top = position.y;
		area.right = position.x + 250;
		area.bottom = position.y + 100;
		LPRECT lpArea;
		lpArea = &area;
		//вывод текста в позиции курсора
		DrawText(hDC, g_lpszLMBClickMessage, -1, lpArea, DT_LEFT | DT_WORDBREAK);
		break;

		//Обработка сообщения WM_PAINT(вывод текста с помощью TextOut)   при перерисовке текст НЕ ИСЧЕЗАЕТ!!!!!!!!!!!!!
	case WM_PAINT:
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps);
		TextOut(hDC, 20, 100, g_lpszPaintMessage, lstrlen(g_lpszPaintMessage));
		EndPaint(hWnd, &ps);
		break;

		//Обработка сообщения WM_CREATE с выводом оповещения
	case WM_CREATE:
		//MessageBox(NULL, g_lpszMessageCreate, TEXT("Оповещение"), MB_OK | MB_ICONINFORMATION);
		return 0;			// при возврате -1, окно не создаётся, а сразу же выскакивает WM_DESTROY
		break;



	default: return(DefWindowProc(hWnd, Msg_id, wParam, lParam));
	}
	return FALSE;
}
