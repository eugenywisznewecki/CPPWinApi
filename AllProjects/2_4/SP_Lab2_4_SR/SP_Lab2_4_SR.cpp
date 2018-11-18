#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"
#include <winuser.h>
#include "YUSH_SR_DLL.h"

//Глобальные переменные

#define g_lpszClassName _TEXT("sp_pr_class")
#define g_lpszAplicationTitle _TEXT("Поиск распределения слов в тексте (явное подключение)")
#define g_lpszAdress _TEXT("SP_Lab2_4_SR_DLL.dll")

//для подключения библиотеки
HMODULE hDLL;
//указатели на функции
bool(*pOpenDlgFile)(HWND); //позволяет выбрать файл при помощи стандартного диалогового окна
bool(*pLoadFile)(); //открывает файл
bool(*pReadTextFromFile)(); //считывает содержимое файла и закрывает его
UINT(*pSearchingLengthWords)(); //находит распределение слов и возвращает максимальную длину
bool(*pFinalizeReport)(); //создает отчет и записывает в конец файла
LPTSTR(*pGetFilePath)(); //возвращает указатель на файл

HINSTANCE hInst; //программа
HMENU hMenu; //меню программы
HWND hWindow; //идентификатор главного окна


//Объявление прототипов функций
			  //функция главного окна
LRESULT CALLBACK Pr6_WndProc(HWND, UINT, WPARAM, LPARAM);
//обработчики сообщений главного окна
bool WndProc_OnCreate(HWND, LPCREATESTRUCT);
void WndProc_OnPaint(HWND);
void WndProc_OnDestroy(HWND);
void WndProc_OnCommand(HWND, int, HWND, UINT);
//обработчик файла, использующий функции DLL
bool ParseTheFile(HWND);
//открывает блокнот с измененным файлом
void OpenNoteApp(HWND);

//оконная процедура модального окна "О программе..."
BOOL CALLBACK DbWndProc_About(HWND, UINT, WPARAM, LPARAM);
//функции для Handle Message из оконной процедуры окна "О программе"
BOOL DbWndProc_ABOUT_OnInitDialog(HWND, HWND, LPARAM);
void DbWndProc_ABOUT_OnCommand(HWND, int, HWND, UINT);
void DbWndProc_OnClose(HWND);


//Главная функция
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
		MessageBox(NULL, _TEXT("Ошибка регистрации класса окна!"), _TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
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

	hMenu = GetMenu(hWnd); //получаем хэндл меню

	if (!hWnd)
	{
		MessageBox(NULL, _TEXT("Не удалось создать окно!"), _TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
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


//Оконная процедура главного окна
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


//Создание и уничтожение главного окна, инициализация необходимых компонентов
bool WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT ipCreateStruct)
{
	//записываем идентификатор окна в глобальную переменную
	hWindow = hWnd;
	//открываем библиотеку
	if ((hDLL = LoadLibrary(g_lpszAdress)) == NULL)
	{
		MessageBox(NULL, _TEXT("Не удалось загрузить библиотеку!"), _TEXT("ОШИБКА!"), MB_OK);
		DestroyWindow(hWnd);
	}
	else
	{
		//инициализация указателей для работы с библиотекой
		pOpenDlgFile = (bool(*)(HWND))GetProcAddress(hDLL, "OpenDlgFile");
		pLoadFile = (bool(*)())GetProcAddress(hDLL, "LoadFile");
		pReadTextFromFile = (bool(*)())GetProcAddress(hDLL, "ReadTextFromFile");
		pSearchingLengthWords = (UINT(*)())GetProcAddress(hDLL, "SearchingLengthWords");
		pFinalizeReport = (bool(*)())GetProcAddress(hDLL, "FinalizeReport");
		pGetFilePath = (LPTSTR(*)())GetProcAddress(hDLL, "GetFilePath");
	}

	return true;
}

//вывод результатов
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


//Команды меню
void WndProc_OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify)
{
	switch (id)
	{
		//обработка текстового файла
	case PARSE_FILE:
		if (!ParseTheFile(hWnd))
		{
			MessageBox(hWnd, _TEXT("Не удалось обработать файл"), _TEXT("ОШИБКА!"), MB_OK);
			return;
		}

		else
			OpenNoteApp(hWnd);

		break;

		//о приложении
	case ABOUT_APP:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, DbWndProc_About);
		break;

		//выход из программы
	case IDM_FILE_EXIT:
		FreeLibrary(hDLL); //отключаемся от библиотеки перед выходом
		DestroyWindow(hWnd);

	default:

		break;
	}
}



//Диалоговое окно "О программе"
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

//для показа текущей даты и времени в модальном окне "О программе"
BOOL DbWndProc_ABOUT_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	LPTSTR lpszRunTime = new TCHAR[50];
	wsprintf(lpszRunTime, TEXT("Текущие дата и время:\n%02d.%02d.%04d %02d:%02d\0"), time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute);
	SetDlgItemText(hDlg, IDC_INFO_DATE, lpszRunTime);
	return TRUE;
}

//для обработки нажатия клавиши в модальном окне "О программе"
void DbWndProc_ABOUT_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
		//Сохраняем данные, введенные в диалоговом окне
		EndDialog(hDlg, TRUE);
	}
}

//для закрытия модального диалога по стандартной клавише
void DbWndProc_OnClose(HWND hDlg)
{
	EndDialog(hDlg, TRUE);
}


//Обработка текстового файла
//обработчик файла, использующий функции DLL
bool ParseTheFile(HWND hWnd)
{
	if (!pOpenDlgFile(hWnd)) //если не удалось выбрать файл
		return false;

	if (!pLoadFile()) //если не удалось открыть файл
		return false;

	if (!pReadTextFromFile()) //если не удалось прочитать текст из файла
		return false;

	if (pSearchingLengthWords() == 0) //если в файле нет буквенных слов
		return false;

	if (!pFinalizeReport()) //если не удалось создать отчет
		return false;

	return true;
}

//открывает блокнот с текстом либо без него
void OpenNoteApp(HWND hWnd)
{
	STARTUPINFO si; //структура для создания процессов
	ZeroMemory(&si, sizeof(STARTUPINFO)); //очистка содержимого структуры от мусора
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi; //структура, куда запишется информация о процессе
	SECURITY_ATTRIBUTES sp, st; //атрибуты защиты для процесса и потока
	sp.nLength = sizeof(SECURITY_ATTRIBUTES);
	sp.lpSecurityDescriptor = NULL;
	sp.bInheritHandle = FALSE; //запрещаем наследование описателей главного процесса
	st.nLength = sizeof(SECURITY_ATTRIBUTES);
	st.lpSecurityDescriptor = NULL;
	st.bInheritHandle = FALSE; ////запрещаем наследование описателей потока главного процесса

							   //ПРИМЕЧАНИЕ
							   //CreateProcess возвращает TRUE до окончательной инициали-
							   //зации процесса.Это означает, что на данном этапе загрузчик операционной
							   //системы еще не искал все необходимые DLL.Если он не сможет найти хотя
							   //бы одну из DLL или корректно провести инициализацию, процесс завер-
							   //шится.Но, поскольку CreateProcess уже вернула TRUE, родительский про-
							   //цесс ничего не узнает об этих проблемах.

	TCHAR cmdParam[300] = { 0 };
	wsprintf(cmdParam, _TEXT("C:\\Windows\\Notepad.exe "));
	lstrcat(cmdParam, pGetFilePath());

	if (!CreateProcess(NULL, cmdParam,
		&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(hWnd, TEXT("Ошибка запуска процесса"), NULL, MB_OK);
		return;
	}
	else
	{
		CloseHandle(pi.hProcess); //отвязывает процесс от нашего приложения
		CloseHandle(pi.hThread);
	}
}
 