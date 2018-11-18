#include <Windows.h>
#include <tchar.h>
#include <atlconv.h>
#include <windowsx.h>
#include "resource.h"
#include <winuser.h>

//Глобальные переменные

#define g_lpszClassName _TEXT("sp_pr_class")
#define g_lpszAplicationTitle _TEXT("2-5. Обработка текстового файла (проецирование файла данных)")
#define MAX_LOADSTRING 100
#define FILENAME TEXT("FILEREV.DAT")
#define lpszCmdLineT TEXT("Test_SR.txt")

//ДЛЯ ПРЕОБРАЗОВАНИЯ ТЕКСТА
//определяем максимальный размер буфера строки
#define MAX_LOADSTRING 10000
//определяем максимальный размер буфера результата
#define MAX_INFOSTRING 500
//максимальная длина слова в русском языке
#define MAX_LENGTH_WORD 55
//хэндл текстового файла
HANDLE txtFile;
//получает число байтов, прочитанных из файла
DWORD dwNumbOfBytes;
//хранит размер самого длинного из найденных слов (последний индекс в массиве)
UINT maxLength;
//массив для хранения количества слов, индекс обозначает количество букв, а в ячейке - содержится количество слов
UINT results[MAX_LENGTH_WORD];
//рабочий буфер
TCHAR text[MAX_LOADSTRING + MAX_INFOSTRING];

//путь к файлу
TCHAR fileName[260];

//отчет
TCHAR reportTXT[MAX_INFOSTRING];

// Global Variables:
HANDLE hFile;        // Для дескриптора объекта "файл"
HANDLE hFileMap;     // Для дескриптора объекта 'проецируемый файл'
LPVOID lpvFile;      // Для адреса региона в адресном пространстве
					 // куда будет отображаться файл
LPSTR  lpchANSI;     // Указатель на ANSI строку

DWORD  dwFileSize;   // Для значения размера файла 
					 //LPTSTR lpszCmdLineT = "Test.txt\0"; // Указатель на командную строку 
					 // в ANSI или UNICODE

STARTUPINFO si = { 0 };    // Структуры для функции
PROCESS_INFORMATION pi;// CreateProcess 
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			

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
BOOL ViewToAddress(HWND);
BOOL CloseProektToAddress();
BOOL CloseHandleKernel();

//преобразование текста
/////////////////////////
//основные функции
////////////////////////
bool ParseTheFile(HWND); //обработчик файла
						 //bool OpenDlgFile(HWND hWnd); //позволяет выбрать файл при помощи стандартного диалогового окна
						 //bool LoadFile(); //открывает файл
						 //bool ReadTextFromFile(); //считывает содержимое файла и закрывает его
UINT SearchingLengthWords(); //находит распределение слов и возвращает максимальную длину
bool FinalizeReport(); //создает отчет и записывает в конец файла
LPTSTR GetFilePath(); //возвращаеть путь к файлу

					  ////////////////////
					  //вспомогательные функции
					  ///////////////////
INT FindNextWord(INT); //находит очередное слово для обработки
UINT CountLengthWord(INT, INT*); //подсчитывает и возвращает длину слова
void WriteDataLength(UINT); //фиксирует данные о слове соответствующей длины в массиве
bool CreateReport(); //создает отчет для записи в конец файла
bool UpdateFile(); //записывает отчет в конец файла

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
		//
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

		//
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

		//
	case PROJECTION:
		if (!ViewToAddress(hWnd))
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

		//
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

		//
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
			wsprintf(buff, TEXT("%s %s"), TEXT("C:\\Windows\\Notepad.exe"), lpszCmdLineT);

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

BOOL ViewToAddress(HWND hWnd)
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

	lpchANSI = (LPSTR)lpvFile;
	lpchANSI[dwFileSize] = 0;

	if (!ParseTheFile(hWnd))
	{
		MessageBox(hWnd, _TEXT("Не удалось обработать файл"), _TEXT("ОШИБКА!"), MB_OK);
		return FALSE;
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


//Обработка файла
//проверяет обработку на всех этапах
bool ParseTheFile(HWND hWnd)
{
	if (SearchingLengthWords() == 0) //если в файле нет буквенных слов
		return false;

	if (!FinalizeReport()) //если не удалось создать отчет
		return false;

	return true;
}

//4. Общая функция: находит распределение слов по длине и возвращает максимальную длину
UINT SearchingLengthWords()
{
	maxLength = 0;
	UINT length = 0;

	INT start = FindNextWord(0); //начинаем проверку текста с начала - ищем первое слово

	while (start != -1) //пока находит следующее слово
	{
		length = CountLengthWord(start, &start); //получаем на выходе длину найденного слова и индекс для поиска следующего

		if (maxLength < length) //определяем, является длина слова максимальной
			maxLength = length;

		WriteDataLength(length); //записываем данные о найденном слове

		start = FindNextWord(start); //получаем индекс следующего слова или 0, если оно не найдено
	}

	return maxLength; //возвращаем длину максимального слова
}

//5. Находит очередное слово (его индекс-смещение от начала), если слово не найдено, возвращает 0
INT FindNextWord(INT start)
{
	while (true)
	{
		if (lpchANSI[start] == '\0') //если подошло к концу строки, возвращаем -1
			return -1;
		else if (IsCharAlpha(lpchANSI[start])) //если нашло символ алфавита, возвращаем его индекс
			return start;
		else //иначе увеличивает значение индекса на 1 и продолжает цикл
			start++;
	}
}

//6. Подсчитывает длину слова (количество символов), возвращает количество символов
UINT CountLengthWord(INT start, INT* end)
{
	UINT length = 0; //задаем начальное значение длины слова

	while (IsCharAlpha(lpchANSI[start]) || lpchANSI[start] == '-') //пока символ является буквой или дефисом
	{
		length++; //увеличиваем значение длины
		start++; //переходим к следующему символу
	}

	*end = start; //записываем индекс следующего за словом символа

	return length; //возвращаем длину слова
}

//7. Записывает данные о найденном слове в глобальный массив
void WriteDataLength(UINT length)
{
	results[length] += 1;
}

//8. Добавляет отчет к рабочему буферу и записывает текст в исходный файл
bool FinalizeReport()
{
	if (CreateReport()) //если отчет не пуст
	{
		if (UpdateFile()) //если удалось записать
			return true;

		else
			return false;
	}

	else
		return false;
}

//9. Создает отчет для записи в конец файла
bool CreateReport()
{
	TCHAR buff[100]; //буфер для формирования отчета по каждой позиции
	buff[0] = '\0';
	reportTXT[0] = '\0'; //сбрасываем содержимое строки отчета
	bool IsResults = false; //есть ли в массиве значения

	lstrcpy(reportTXT, _TEXT("\r\nРаспределение слов:\r\n"));

	for (int i = 1; i <= maxLength; i++) //проходим по результирующему массиму
	{
		if (results[i] != 0) //если в тексте встретились слова с длиной, равной индексу
		{
			wsprintf(buff, _TEXT("Длина: %d | Количество слов: %d\r\n"), i, results[i]); //формируем строку по конкретной длине
			lstrcat(reportTXT, buff); //добавляем к отчету
			IsResults = true; //слова найдены
		}
	}

	return IsResults;
}

//10. Перезаписывает файл
bool UpdateFile()
{
	if (fileName == NULL || (lstrcmp(reportTXT, _TEXT("")) == 0))
		return false;

	txtFile = CreateFile(lpszCmdLineT, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (txtFile == NULL)
		return false;

	else
	{
		DWORD dwBytesToWrite = lstrlen(reportTXT);
		DWORD dwBytesWritten;

		//конвертируем в ANSI
		CHAR* inFile;
		USES_CONVERSION;
		inFile = W2A(reportTXT);

		SetFilePointer(txtFile, 0, 0, FILE_END); //устанавливаем указатель в конец
		WriteFile(txtFile, inFile, strlen(inFile), &dwBytesWritten, NULL); //записываем отчет в файл
		CloseHandle(txtFile); //закрываем файл

		return true;
	}
}

//11. Возвращает путь к файлу
LPTSTR GetFilePath()
{
	return lpszCmdLineT;
}


 