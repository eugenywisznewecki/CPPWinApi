#include <Windows.h> 
#include <tchar.h>
#include "resource.h"

static HMENU hMenu;

LPCTSTR g_lpszClassName = TEXT("sp_pr2_class");
LPCTSTR g_lpszAplicationTitle = TEXT("Главное окно приложения. Программист: <Юшкевич, Евгений>");
LPCTSTR g_lpszDestroyMessage = TEXT("Поступило сообщение WM_DESTROY, из обработчика которого и выполнен данный вывод. Сообщение поступило в связи с разрушением окна приложения.");
LPCTSTR g_lpszLMBClickMessage = TEXT("Обработка сообщения WM_LBUTTONDOWN, которое посылается в окно при щелчке левой кнопки мыши");
LPCTSTR g_lpszPaintMessage = TEXT("Обработка сообщения WM_PAINT");
LPCTSTR g_lpszMessageCreate = TEXT("Выполняется обработка WM_CREATE");
LPCTSTR g_lpszMsgSpace = TEXT("                                                    ");
LPTSTR g_lpszTextMenu = _TEXT("Закрыть документ");
#define g_lpszCommandMessage _TEXT("Выбрана команда \"")
#define g_lpszCommandMessageDefault _TEXT("Команда с идентификатором \"")

#define	IDM_CLOSE	40021

LRESULT CALLBACK pr1_WndProc(HWND, UINT, WPARAM, LPARAM);
void ShowMessageAboutMenuItem(HWND, UINT, LPWSTR, LPWSTR);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow) {
	WNDCLASSEX WndClass;
	HWND hWnd;				// идентификатор главного окна приложения
	MSG Msg;
	HACCEL hAccel;			//переменная для таблицы акселераторов (горячих клавиш)

	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));							//функция формирует меню для подключения его к программе через функцию создания окна
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR_YUSH)); //функция загружает список акселераторов в переменную //========= Задание2.10

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
//========= Задание2. 8.Подключение меню.
		//a)		Подключите меню к программе через структуру класса окна.
	//WndClass.lpszMenuName = MAKEINTRESOURCE (IDR_MENU1);						//подключение меню через структуру класса окна

	if (!RegisterClassEx(&WndClass)) {
		MessageBox(NULL, TEXT("Ошибка регистрации!"), TEXT("Ошибка!"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

//========= Задание2. 8.Подключение меню.
	//б) Подключите меню к программе через функцию создания окна CreateWindowEx : меню сначала загружается при помощи функции LoadMenu :
	hWnd = CreateWindowEx(NULL, g_lpszClassName, g_lpszAplicationTitle, WS_OVERLAPPEDWINDOW - WS_MINIMIZEBOX,
		600, 300, 500,300,
		NULL, hMenu,			//подключение меню через функцию создания окна
		hInstance, NULL);

	if (!hWnd) {
		MessageBox(NULL, TEXT("Окно не создано!"), TEXT("Ошибка!"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Msg, NULL, 0, 0))								//========= Задание2.10 	
	{
		if (!hAccel || !TranslateAccelerator(hWnd, hAccel, &Msg))		//если переменная с акселератором не равна нулю и получено сообщение от клавиатуры
																		//оно преобразуется в сообщение типа WM_COMMAND и посылается главному окну минуя очередь сообщений
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

	DestroyAcceleratorTable(hAccel); //уничтожает таблицу клавиш-ускорителей. Прежде, чем приложение закроется, оно должно использовать эту функцию, чтобы уничтожить каждую таблицу клавиш-ускорителей, которая создавалась

	return Msg.wParam;
}
LRESULT CALLBACK pr1_WndProc(HWND hWnd, UINT Msg_id, WPARAM wParam, LPARAM lParam) {
	HDC hDC;					//создадём экземпляр контекста устройства
	int wmId, wmEvent;			//переменные для Задачи2 п.9;
	HMENU hFile; //дескриптор подменю "Файл"
	HMENU hEdit; //дескриптор подменю "Правка"
	HMENU hFloatMenu;
	DWORD xyPos;
	WORD xPos, yPos;
	MENUITEMINFO mItem, mItem0, mItem1; //структуры, хранящие информацию о пунктах всплывающего меню
	switch (Msg_id) {
	case WM_DESTROY:
		//MessageBox(NULL, g_lpszDestroyMessage, TEXT("Оповещение"), MB_OK);
		PostQuitMessage(5);
		break;
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
		//TextOut(hDC, 20, 100, g_lpszPaintMessage, lstrlen(g_lpszPaintMessage));
		EndPaint(hWnd, &ps);
		break;

		//Вставка пункта меню программным путем			//========= Задание3.1
	case WM_CREATE:
		hFile = GetSubMenu(hMenu, 0); //получение дескриптора подменю "Файл"
		hEdit = GetSubMenu(hMenu, 1); //получение дескриптора подменю "Правка"
		mItem.cbSize = sizeof(MENUITEMINFO);
		mItem.fMask = MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
		mItem.fType = MFT_STRING;
		mItem.fState = MFS_ENABLED;
		mItem.dwTypeData = g_lpszTextMenu;
		mItem.cch = sizeof(g_lpszTextMenu);
		mItem.wID = IDM_CLOSE;
		mItem.hSubMenu = NULL;
		InsertMenuItem(hFile, 2, true, &mItem); //вставка новой строки подменю
		MessageBox(NULL, g_lpszMessageCreate, _TEXT("ОПОВЕЩЕНИЕ"), MB_OK | MB_ICONINFORMATION);
		break;

		//Создание и вывод контекстного меню			//========= Задание3.2
	case WM_RBUTTONDOWN:
		//получаем данные о состоянии соответствующих пунктов в меню "Правка"
		MENUITEMINFO mSelect, mCopy; //переменные для хранения состояния соответствующих пунктов основного меню
		mSelect.cbSize = mCopy.cbSize = sizeof(MENUITEMINFO); //задаем размер структуры меню
		mSelect.fMask = mCopy.fMask = MIIM_STATE; //задаем маску для извлечения значения
		GetMenuItemInfo(hMenu, IDM_EDIT_SELECT, false, &mSelect); //получаем состояние пункта "Выделить" в основном меню
		GetMenuItemInfo(hMenu, IDM_EDIT_COPY, false, &mCopy); //получаем состояние пункта "Копировать" в основном меню

		//получаем экранные координаты курсора вместо позиции относительно левого верхнего угла окна
		xyPos = GetMessagePos();
		xPos = LOWORD(xyPos);
		yPos = HIWORD(xyPos);
		hFloatMenu = CreatePopupMenu(); //создаем всплывающее меню

		//заполнение структур для каждого пункта всплывающего меню
		mItem0.cbSize = mItem1.cbSize = sizeof(MENUITEMINFO);
		mItem0.fMask = mItem1.fMask = MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
		mItem0.fType = mItem1.fType = MFT_STRING;
		mItem0.fState = mSelect.fState; //присваиваем состояние первому пункту всплывающего меню
		mItem1.fState = mCopy.fState; //присваиваем состояние второму пункту всплывающего меню
		mItem0.dwTypeData = _TEXT("Выделить");
		mItem1.dwTypeData = _TEXT("Копировать");
		mItem0.cch = sizeof(_TEXT("Выделить"));
		mItem1.cch = sizeof(_TEXT("Копировать"));
		mItem0.wID = IDM_EDIT_SELECT; //идентификаторы пунктов контекстного меню совпадают с соответствующими пунктами в основном меню
		mItem1.wID = IDM_EDIT_COPY;
		mItem0.hSubMenu = mItem1.hSubMenu = NULL;

		//вставляем пункты во всплывающее меню
		InsertMenuItem(hFloatMenu, 0, true, &mItem0);
		InsertMenuItem(hFloatMenu, 1, true, &mItem1);
		//отображаем всплывающее меню на экране
		TrackPopupMenu(hFloatMenu, TPM_LEFTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN, xPos, yPos, 0, hWnd, NULL);
		break;

		//Вывод подсказок при наведении курсора на пункт меню		//========= Задание2.11
	case WM_MENUSELECT:
		HDC hdc1;
		TCHAR Buf[300];
		HINSTANCE hInst;
		hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE); //при смещении GWL_HINSTANCE функция возвращает дескриптор экземпляра приложения
		int size;
		size = LoadString(hInst, LOWORD(wParam), Buf, 300); //отправляем дескриптор приложения, индекс подменю (откуда берется строка, привязанная к меню), размер текстового буфера для записи, количество записываемых символов
		hdc1 = GetDC(hWnd); //получаем дескриптор рабочей области окна
		RECT rc; //структура координат
		GetClientRect(hWnd, &rc); //получение координат рабочей области окна
		TextOut(hdc1, rc.left, rc.bottom - 30, g_lpszMsgSpace, lstrlen(g_lpszMsgSpace)); //сначала в поле выводится строка с пробелами
		TextOut(hdc1, rc.left, rc.bottom - 30, Buf, lstrlen(Buf)); //потом поверх нее выводится искомая строка (чтобы длинные строки не перекрывали короткие)
		ReleaseDC(hWnd, hdc1); //освобождение контекста устройства
		break;

		//Обработка сообщений меню
	case WM_COMMAND:
	{
		wmId = LOWORD(wParam); // младшее слово определяет пункт меню
		wmEvent = HIWORD(wParam); //старшее слово определяет произошедшее событие - клик
		TCHAR lpszFinalString[500] = { 0 };
		TCHAR lpszString[500] = { 0 };

		switch (wmId)
		{
		case IDM_FILE_NEW:
			//Включение пункта меню "Выделить"
			hEdit = GetSubMenu(hMenu, 1); //получаем дескриптор подменю "Правка"
			EnableMenuItem(hEdit, IDM_EDIT_SELECT, MFS_ENABLED | MF_BYCOMMAND); //включаем пункт "Выделить"
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
			MessageBox(NULL, g_lpszDestroyMessage, _TEXT("ОПОВЕЩЕНИЕ"), MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
			break;

			//Включение пункта меню "Копировать"
		case IDM_EDIT_SELECT: //обработка пункта "Выделить"
			hEdit = GetSubMenu(hMenu, 1);
			EnableMenuItem(hEdit, IDM_EDIT_COPY, MFS_ENABLED | MF_BYCOMMAND); //включаем пункт "Копировать"
			ShowMessageAboutMenuItem(hWnd, IDM_EDIT_SELECT, lpszString, lpszFinalString);
			break;

			//Выключение пунктов меню "Выделить" и "Копировать"
		case IDM_CLOSE: //обработка пункта "Закрыть документ"
			hEdit = GetSubMenu(hMenu, 1);
			EnableMenuItem(hEdit, IDM_EDIT_SELECT, MFS_GRAYED | MF_BYCOMMAND); //меняем флаги, делая два пункта недоступными
			EnableMenuItem(hEdit, IDM_EDIT_COPY, MFS_GRAYED | MF_BYCOMMAND);
			ShowMessageAboutMenuItem(hWnd, IDM_CLOSE, lpszString, lpszFinalString);
			break;

		default:
			wsprintf(lpszFinalString, _TEXT("Команда с идентификатором \"%d\" не реализована."), wmId);
			MessageBox(hWnd, lpszFinalString, _TEXT("Сообщение при нажатии на пункт меню"), MB_OK);
			break;
		}
	}



	default: return(DefWindowProc(hWnd, Msg_id, wParam, lParam));
	}
	return FALSE;
}
//Функция для вывода подсказок о назначении команд меню
void ShowMessageAboutMenuItem(HWND hWnd, UINT IDM_MENU_ITEM, LPWSTR lpszString, LPWSTR lpszFinalString)
{
	GetMenuString(hMenu, IDM_MENU_ITEM, lpszString, 50, MF_BYCOMMAND);
	lstrcat(lpszFinalString, g_lpszCommandMessage);
	lstrcat(lpszFinalString, lpszString);
	lstrcat(lpszFinalString, _TEXT("\"."));
	MessageBox(hWnd, lpszFinalString, _TEXT("Сообщение при нажатии на пункт меню"), MB_OK);
}