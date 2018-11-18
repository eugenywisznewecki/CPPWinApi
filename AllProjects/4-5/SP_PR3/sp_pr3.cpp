#include <windows.h> 
#include <windowsx.h> // HANDLE_MSG
#include <tchar.h>	
#include "resource.h"
#include "sp_pr3.h"
#include <Strsafe.h> // Для 'StringCbPrintf'

HMENU hMenu, hEditMenu, hFloatMenu; 

HWND hWnd;					  // дескриптор окна–владельца
HINSTANCE hinstance;		  // Лаб 5, Для "Текст" и "Элементы управления" 

//Элементы управления 1.3.3
static HWND hEdit, hListBox;  // Лаб 5, Для "Элементы управления" -> Указатель на Массив для сохранения данных из диалогового окна
//static TCHAR gpszTextBuff[100];
static TCHAR EditBuff[20]; 

static TCHAR gpszTextBuff[10][100];	//
static int cnt;						//

//Элементы управления 1.3.4
OPENFILENAME ofn;   // структура для common dialog box
HANDLE hFile;       // дескриптор файла
DWORD dwNumbOfBytes = MAX_BYTES;	// буфер для текста  
TCHAR Buffer[MAX_BYTES];			// буфер для текста 
BOOL fRet;				   // для 'Диалоговое окно'
TCHAR lpszFileSpec[100];   // массив для имени файла

//Прототипы функций обработки сообщений с пользовательским названием
LRESULT CALLBACK Pr2_WndProc(HWND, UINT, WPARAM, LPARAM);					// <Функция>
BOOL CALLBACK Text(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam); // Текст
BOOL CALLBACK ElementyUpravlenija(HWND, UINT, WPARAM, LPARAM);				// Элементы управления
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);							// О программе...

// HANDLE_MSG 
BOOL Cls_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
void Cls_OnPaint(HWND hwnd);
void Cls_OnDestoy(HWND);																		 
void Cls_OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);

BOOL CreateMenuItem(HMENU hMenu, LPWSTR str, UINT u, UINT uc, HMENU hSub, BOOL flag, UINT type); // <<<=== Плавающее Меню



//Мейн
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{
	//HWND hWnd;
	MSG msg;
	WNDCLASSEX WndClass;
	HACCEL hAccel; // <<<=== Акселератор
	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)); // 8.Подключение меню: б) <<<=== Меню

	memset(&WndClass, 0, sizeof(WNDCLASSEX));
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.hInstance = hInstance;
	WndClass.lpszClassName = TEXT("SimpleClassName1_5");
	WndClass.lpfnWndProc = Pr2_WndProc;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); // <<<=== Иконка
	WndClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_POINTER)); // <<<=== Курсор
	WndClass.lpszMenuName = NULL; //(LPCTSTR)IDR_MENU1;  // 8.Подключение меню: а) <<<=== Меню
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = CreateSolidBrush(RGB(192, 192, 192));
	WndClass.hIconSm = NULL;

	if (!RegisterClassEx(&WndClass))
	{
		MessageBox(NULL, TEXT("Ошибка регистрации класса окна!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hWnd = CreateWindowEx(NULL,
		TEXT("SimpleClassName1_5"), 
		TEXT("Главное окно приложения. Программист: <Юшкевич, Евгений>"),
		WS_OVERLAPPEDWINDOW,
		600, 300,		//вместо координат Х и У можно использовать CW_USEDEFAULT - значение по умолчанию 
		600, 450,
		HWND_DESKTOP,
		LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)), // 8.Подключение меню: б) <<<=== Меню
		hInstance,
		NULL);
	if (!hWnd)
	{
		MessageBox(NULL, TEXT("Нельзя создать окно!"),
			TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// АКСЕЛЕРАТОР 
	hAccel = LoadAccelerators(hInstance,
		MAKEINTRESOURCE(IDR_ACCELERATOR1));
	// + цикл обработки сообщений
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


//Функция
LRESULT CALLBACK Pr2_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	hMenu = GetMenu(hWnd);	// Задание 3.1; вставка программным путем команды “Закрыть документ” в меню “Файл”  <<<=== Меню	
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hWnd, WM_PAINT, Cls_OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, Cls_OnDestoy); // Переход по значению (HANDLE_MSG)
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
// Элементы управления 1.3.3 //создание списка
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

// Элементы управления 1.3.4
	//Иницализация OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;														
	ofn.lpstrFile = lpszFileSpec;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(lpszFileSpec);
	// Формирование массива строк шаблонов фильтра
	ofn.lpstrFilter = L"Все файлы\0*.*\0Текстовые документы (*.txt)\0*.TXT\0";
	ofn.nFilterIndex = 1;		// Индекс для текущего шаблона фильтра
	ofn.lpstrFileTitle = NULL;	// Без заголовка
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

HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);								// Для "DialogBox" (О программе...)
switch (wmId) 
{
	// Меню -> Файл
case IDM_FILE_EXIT:
	SendMessage(hWnd, WM_DESTROY, 0, 0);
	break;
	// Меню -> Просмотр
case IDM_VIEW_TEXT: // Текст 
	DialogBox(hinstance, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, Text);								// Для "Текст"
	break;

case IDM_VIEW_CTL:																				

	//DialogBox(hinstance, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, ElementyUpravlenija);				// Для "Элементы управления"
	
	DialogBox((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG2), hWnd, (DLGPROC)ElementyUpravlenija);
	ListBoxContent(cnt);	//
	break;

	// Меню -> Сппавка
case IDM_INFO_ABOUT: // О программе...
	{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)About);
	}
	break;
	//-------------------------------------------------------------------
case IDM_INFO_HELP:
	MessageBox(hWnd, _T("Выбран пункт 'HELP'"), _T("Меню Info"), MB_OK);
	break;

default:
	TCHAR buf_for_MB[100];				// 9.Сообщение об отсутствии кнопки в case WM_COMMAND: c вводом кода идентификатора <<<=== Меню	
	WORD CommandCaseId = LOWORD(id);
	StringCbPrintf(buf_for_MB, 101, L"Команда  с идентификатором '%d' не реализована!", CommandCaseId);
	MessageBox(hWnd, buf_for_MB, TEXT("Ошибка"), MB_OK);
	}
	return FORWARD_WM_COMMAND(hWnd, id, hwndCtl, codeNotify, DefWindowProc);
}

BOOL CALLBACK Text(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam) // Текст - Модальное окно
{
	LPCWSTR lpszInitEdit = TEXT("Шаг 1. Начало работы");

	HWND hEdit = NULL;
	hEdit = GetDlgItem(hDlg, IDC_EDIT1); // Извлекаем в дескрипторе [Окна] идентификатор [Элемента]

	HANDLE hFile = NULL;

	switch (uMessage)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), lpszInitEdit);	// Вывод "Шаг 1. Начало работы"
		return TRUE;
	case WM_COMMAND:
	{
		int wmId, wmEvent;
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDC_LOAD:								// При выборе 'Загрузить'
			lstrcat(Buffer, TEXT("Тест Шага 2"));	// Вывод "Тест Шага 2"
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

			lstrcat(Buffer, TEXT(""));				// Предварительная очистка буфера
			SetWindowText(hEdit, Buffer);			// Вывод текста в ""EditList"

			if (hFile) CloseHandle(hFile);			// Закрытие файла

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

BOOL CALLBACK ElementyUpravlenija(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam) // Элементы управления - Модальное окно
{
	static HWND hWndList, hWndEdit;
	
	LPCTSTR step1 = TEXT("Строка 1");

	switch (msg)
	{ 
	case WM_INITDIALOG:			// Инициализация диалоговой панели
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
			cch = SendMessage(hWndEdit, WM_GETTEXT, (WPARAM)100, (LPARAM)EditBuff);  //сообщение окну редактора, код сообщения, размер буфера, записываем в буфер текст

			if (cch == 0) //если запись не произошла (поле редактора пустое)
				MessageBox(hWndEdit, L"Вы не ввели текст в поле Edit", L"ОШИБКА!", MB_OK);//окно сообщения с просьбой ввести текст
			else
			{
				TCHAR Buff1[500] = { '\0' }; //создание строки текста
				SYSTEMTIME systime; //структура для системного времени
				GetLocalTime(&systime);			// получаем время из системы
				wsprintf(Buff1, _TEXT("Время : %d ч %d мин %d сек\n"),
					systime.wHour, systime.wMinute, systime.wSecond); //текущее время записывается в буфер
				lstrcat(Buff1, __TEXT("Текст в памяти: "));
				lstrcat(Buff1, EditBuff); //добавление строк
				MessageBox(hdlg, Buff1, _TEXT("Содержимое буфера"), MB_OK);//вывод финальной строки
			}
		}break;

		case IDC_ADDLIST:
		{
			if (EditBuff[0] == '\0') //если строка пустая
				MessageBox(hdlg, _TEXT("Нельзя добавить пустую строку!"), _TEXT("ОШИБКА!"), MB_OK);
			else
			{
				int ind;
				ind = SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)EditBuff); //отправка буферной строки в список

				if (ind == LB_ERR)
					MessageBox(hWnd, _TEXT("Ошибка в списке"), _TEXT("ОШИБКА!"), MB_OK); //если не удалось записать - ошибка
			}
		}break;
		// Сообщение от кнопки "OK"
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

			// Сообщение от кнопки "Крестик"
		case IDCANCEL:
			//ZeroMemory(gpszTextBuff, sizeof(gpszTextBuff));
			EndDialog(hdlg, TRUE);
			return true;
		}
	}
	}
	return FALSE;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) // О программе... - Модальное окно
{
	static HICON hIconDialog;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		SYSTEMTIME systime;
		GetLocalTime(&systime);			// получаем время из системы
		TCHAR Time[64] = TEXT("");
		TCHAR Date[64] = TEXT("");
		wsprintf(Date, TEXT("Текущая дата: %02d.%02d.%d"), systime.wDay, systime.wMonth, systime.wYear);
		wsprintf(Time, TEXT("Время:               %02d:%02d"), systime.wHour, systime.wMinute);

		SetDlgItemText(hDlg, IDC_STATIC1, TEXT("Лабораторная работа № 5"));
		SetDlgItemText(hDlg, IDC_STATIC2, TEXT("Диалоговые окна. Элементы управления в диалоговых окнах.\n\nРазработчик - Юшкевич Е."));
		SetDlgItemText(hDlg, IDC_STATIC3, Date);
		SetDlgItemText(hDlg, IDC_STATIC4, Time);
		hIconDialog = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIconDialog);
	}
	return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_OKd1 || LOWORD(wParam) == IDCANCEL) // используется только 'OK'
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Cls_OnDestoy(HWND) // Функция обработки сообщения WM_DESTROY: (HANDLE_MSG)
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