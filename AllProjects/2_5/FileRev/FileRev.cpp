#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"
#include <winuser.h>

//Глобальные переменные

#define g_lpszClassName _TEXT("sp_pr_class")
#define g_lpszAplicationTitle _TEXT("2-5. FileRev с графическим интерфейсом")

#define MAX_LOADSTRING 100
#define FILENAME TEXT("FILEREV.DAT")
#define lpszCmdLineT TEXT("Test_FileRev.txt")
// Global Variables:
HANDLE hFile;        // Для дескриптора объекта "файл"
HANDLE hFileMap;     // Для дескриптора объекта 'проецируемый файл'
LPVOID lpvFile;      // Для адреса региона в адресном пространстве
					 // куда будет отображаться файл
LPSTR  lpchANSI;     // Указатель на ANSI строку

DWORD  dwFileSize;   // Для значения размера файла 
					 //LPTSTR lpszCmdLineT = "Test_FileRev.txt\0"; // Указатель на командную строку 
					 // в ANSI или UNICODE

STARTUPINFO si = { 0 };    // Структуры для функции
PROCESS_INFORMATION pi;// CreateProcess 
TCHAR szTitle[MAX_LOADSTRING];					// Текст заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			//имя класса главного окна

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

//работа с проецируемым файлом
BOOL CreateFileObj();
BOOL CreateFileProekt();
BOOL ViewToAddress();
BOOL CloseProektToAddress();
BOOL CloseHandleKernel();

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
		//создает файл
	case CREATE_FILE:
		if (!CreateFileObj())
			MessageBox(hWnd, TEXT("Ошибка создания файла!"), TEXT("ОШИБКА!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("Объект файла успешно создан!"), TEXT("ОПОВЕЩЕНИЕ"), MB_OK);
			EnableMenuItem(hMenu, CREATE_PR_FILE, MF_ENABLED);
			EnableMenuItem(hMenu, CREATE_FILE, MF_GRAYED);
		}
		break;

		//создает проецируемый файл
	case CREATE_PR_FILE:
		if (!CreateFileProekt())
			MessageBox(hWnd, TEXT("Ошибка создания проецируемого файла!"), TEXT("ОШИБКА!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("Проецируемый файл успешно создан!"), TEXT("ОПОВЕЩЕНИЕ"), MB_OK);
			EnableMenuItem(hMenu, CREATE_PR_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, CREATE_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, PROJECTION, MF_ENABLED);
		}
		break;

		//выполняет проецирование
	case PROJECTION:
		if (!ViewToAddress())
			MessageBox(hWnd, TEXT("Ошибка проецирования файла!"), TEXT("ОШИБКА!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("Проецирование файла прошло успешно!"), TEXT("ОПОВЕЩЕНИЕ"), MB_OK);
			EnableMenuItem(hMenu, CREATE_PR_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, CREATE_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, PROJECTION, MF_GRAYED);
			EnableMenuItem(hMenu, CLOSE_PROJECTION, MF_ENABLED);
		}
		break;

		//закрывает проекцию файла
	case CLOSE_PROJECTION:
		if (!CloseProektToAddress())
			MessageBox(hWnd, TEXT("Ошибка закрытия проецируемого!"), TEXT("ОШИБКА!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("Закрытие проецируемого файла прошло успешно!"), TEXT("ОПОВЕЩЕНИЕ"), MB_OK);
			EnableMenuItem(hMenu, CREATE_PR_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, CREATE_FILE, MF_GRAYED);
			EnableMenuItem(hMenu, PROJECTION, MF_GRAYED);
			EnableMenuItem(hMenu, CLOSE_PROJECTION, MF_GRAYED);
			EnableMenuItem(hMenu, CLOSE_HANDLE, MF_ENABLED);
		}
		break;

		//закрывает хэндлы открывает блокнот с файлом
	case CLOSE_HANDLE:
		if (!CloseProektToAddress())
			MessageBox(hWnd, TEXT("Ошибка закрытия хэндлов!"), TEXT("ОШИБКА!"), MB_OK);

		else
		{
			MessageBox(hWnd, TEXT("Закрытие хэндлов прошло успешно!\nНажмите ОК, чтобы увидеть результат"), TEXT("ОПОВЕЩЕНИЕ"), MB_OK);

			// Запускаем NOTEPAD и загружаем в него созданный файл,
			// чтобы увидеть плоды своих трудов
			si.cb = sizeof(si);// Заполняем поле размера структуры si
			si.wShowWindow = SW_SHOW;// Задаем режим показа окна NOTEPAD
			si.dwFlags = STARTF_USESHOWWINDOW;// Устанавливаем флаг - учитывать

			TCHAR buff[266] = { 0 };
			wsprintf(buff, TEXT("%s %s"), TEXT("C:\\Windows\\Notepad.exe"), FILENAME);

			// значение поля wShowWindow
			if (CreateProcess(TEXT("C:\\Windows\\Notepad.exe"), buff,
				NULL, NULL, FALSE, 0,
				NULL, NULL, &si, &pi))
			{
				// Если процесс создан, освобождаем 
				// дескрипторы потока и процесса	
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			DestroyWindow(hWnd);
		}
		break;

		//о приложении
	case ABOUT_APP:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, DbWndProc_About);
		break;

		//выход из программы
	case IDM_FILE_EXIT:
		DestroyWindow(hWnd);

	default:

		break;
	}
}


//Проецируемый файл
//создание копии файла
BOOL CreateFileObj()
{
	// Чтобы не испортить входной файл, копируем его в новый файл,
	// имя которого указываем через определенную выше константу FILENAME.

	if (!CopyFile(lpszCmdLineT, FILENAME, FALSE))
	{
		// Копирование не удалось
		MessageBox(NULL, TEXT("Новый файл не может быть создан!"),
			TEXT("ОШИБКА"), MB_OK);
		return FALSE;
	}

	// Открываем файл для чтения и записи. Для этого создаем объект
	// ядра "Файл". В зависимости от указанных параметров функция 
	// CreateFile либо открывает существующий файл, либо создает новый.
	// Только эта функция может использоваться для открытия файла,
	// проецируемого в память. Функция возвращает дескриптор открытого
	// объекта ядра, или код ошибки INVALID_HANDLE_VALUE.

	hFile = CreateFile(
		FILENAME,  // Указатель на строку с имененм файла
		GENERIC_WRITE | GENERIC_READ, // Требуемый режим доступа
		0,   //  Режим разделения,0 - безраздельный доступ к файлу.       
		NULL,// LPSECURITY_ATTRIBUTES=0 - объект не наследуемый.
		OPEN_EXISTING,//Если файл не существует, то возвратить ошибку.
		FILE_ATTRIBUTE_NORMAL,//Оставить атрибуты как есть 
		NULL //Не давать имя объекту ядра "Файл"
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{  // Открыть файл не удалось
		MessageBox(NULL, TEXT("Не удалось открыть файл!"),
			TEXT("ОШИБКА"), MB_OK);
		return FALSE;
	}

	// Узнаем размер файла. Второй параметр NULL, так как предполагается,
	// что файл меньше 4 Гб.
	dwFileSize = GetFileSize(hFile, NULL);

	return TRUE;
}

//создание проецируемого файла
BOOL CreateFileProekt()
{
	// Создаем объект "проецируемый файл". Он - на 1 байт больше, чем
	// размер файла, чтобы можно было записать в конец файла нулевой 
	// символ  и работать с файлом как с нуль-терминированной строкой.
	// Поскольку пока еще неизвестно содержит файл ANSI - или Unicode
	// - символы, увеличиваем файл на величину размера символа WCHAR

	hFileMap = CreateFileMapping(
		hFile,  // Дескриптор проецирумого объекта "Файл" 
		NULL,   // 
		PAGE_READWRITE, // Атрибуты защиты страниц 
		0,  // LPSECURITY_ATTRIBUTES
		dwFileSize + sizeof(WCHAR),   //Младшие 32 разряда
		NULL  // и старшие 32 разряда размера файла.
	);
	if (hFileMap == NULL)
	{	// Открыть объект "проецируемый файл" не удалось
		MessageBox(NULL, TEXT("Не удалось открыть проецируемый файл!"),
			TEXT("ОШИБКА!"), MB_OK);
		CloseHandle(hFile);// Перед выходом закрываем открытые объекты
		return FALSE;
	}

	return TRUE;
}

BOOL ViewToAddress()
{
	lpvFile = MapViewOfFile(
		hFileMap, // Дескриптор объекта "Проецируемый файл" 
		FILE_MAP_WRITE, // Режим доступа
		0, // Старшие 32 разряда смещения от начала файла, 
		0, // младшие 32 разряда смещения от начала файла
		0  // и количество отображаемых байт. 0 - весь файл.
	);

	if (lpvFile == NULL)
	{// Спроецировать оконное представление файла не удалось
		MessageBox(NULL, TEXT("Не удалось спроецировать файл на адресное пространство!"),
			TEXT("ОШИБКА!"), MB_OK);
		CloseHandle(hFileMap);// Перед выходом закрываем открытые объекты
		CloseHandle(hFile);
		return FALSE;
	}


	// Записываем в конец файла нулевой символ
	lpchANSI = (LPSTR)lpvFile;
	lpchANSI[dwFileSize] = 0;

	// "Переворачиваем" содержимое файла наоборот
	_strrev(lpchANSI);

	// Преобразуем все комбинации управляющих символов "\n\r" обратно в "\r\n",
	// чтобы сохранить нормальную последовательность кодов "возврат каретки"
	// "перевод строки", завершающих строки в текстовом файле

	lpchANSI = strchr(lpchANSI, (int)'\n');   // Находим адрес первого '\n'
	while (lpchANSI != NULL) //Пока не найдены все символы '\n'
	{
		*lpchANSI = (int)'\r'; // Заменяем '\n' на '\r'
		lpchANSI++;           // Увеличиваем указатель
		*lpchANSI = (int)'\n';// Заменяем '\r' на '\n' и увеличиваем указатель
		lpchANSI++;
		lpchANSI = strchr(lpchANSI, (int)'\n');  // Ищем дальше
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
	// Уменьшаем счетчик ссылок на объект ядра "Проецируемый файл"
	CloseHandle(hFileMap);

	// Удаляем добавленный ранее концевой нулевой байт.Для этого
	// перемещаем указатель файла в конец на нулевой байт,
	// а затем даем команду установить в этом месте конец файла

	SetFilePointer(hFile, dwFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	// SetEndOfFlle нужно вызывать после закрытия дескриптора объекта
	// ядра "Проецируемый файл"

	CloseHandle(hFile);// Уменьшаем счетчик ссылок на объект ядра "Файл"
	return TRUE;
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
