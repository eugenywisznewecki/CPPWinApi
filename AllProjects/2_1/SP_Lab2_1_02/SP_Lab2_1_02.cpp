#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>
#include "resource.h"

#define	IDC_EDIT_BLOCK	105 //для текстового поля
#define MAX_BYTES  100000

HINSTANCE hInst;
UINT exitCode = 1; //код завершения процесса "блокнот", равный порядковому номеру в группе
TCHAR fileName[260] = { 0 };
TCHAR path[260] = { 0 };
static HWND hEdit; //для текстового поля
HANDLE hFile;
WCHAR BuffText[MAX_BYTES]; // буфер для текста
						   //переменные для установки параметров TextProc
DWORD dwX, dwY, dwXSize, dwYSize;
WORD wShowWindow;
//для работы с ComboBox
LPWSTR ComboBoxStrings[4] = { TEXT("SW_SHOW"), TEXT("SW_SHOWDEFAULT"), TEXT("SW_SHOWMAXIMIZED"), TEXT("SW_SHOWMINIMIZED") };
WORD ComboBoxValues[4] = { SW_SHOW, SW_SHOWDEFAULT, SW_SHOWMAXIMIZED, SW_SHOWMINIMIZED };
bool show = false;

// Глобальные массивы
HANDLE ProcHandle[5]; //для дескрипторов процессов;
DWORD ProcId[5]; //для идентификаторов процессов;
HANDLE ThreadHandle[5]; //для дескрипторов потоков;
DWORD ThreadId[5];// для.идентификаторов потоков;
LPTSTR ProcImage[6] = { 0,	TEXT("C:\\Windows\\Notepad.exe"),
TEXT("C:\\Windows\\Notepad.exe"),
TEXT("C:\\Windows\\System32\\Calc.exe"),
TEXT("C:\\Windows\\Notepad.exe"),
TEXT("TestProc.exe") };//для указателей строк, идентифицирущих файлы запускаемых программ;
LPTSTR CmdParam[5] = { 0, 0, TEXT("C:\\Windows\\Notepad.exe SP_Lab2_1_02.cpp"), 0, TEXT("") };; //для строк c параметрами запускаемых программ.


																								// Объявление прототипов функций
																								//главная оконная функция
LRESULT CALLBACK Pr2_1_WndProc(HWND, UINT, WPARAM, LPARAM);
//обработка сообщений
bool WndProc_OnCreate(HWND, LPCREATESTRUCT);
void WndProc_OnPaint(HWND);
void WndProc_OnDestroy(HWND);
void WndProc_OnCommand(HWND, int, HWND, UINT);
//процедура диалогового окна
INT_PTR CALLBACK DlgBoxInfo(HWND, UINT, WPARAM, LPARAM);
//сохраняет данные о процессе в массивы
void SavingInfoAboutProc(HANDLE, DWORD, HANDLE, DWORD, int);
//открывает пустой блокнот или блокнот с файлом
void OpenNoteApp(HWND, bool, int);
//позволяет выбрать файл для блокнота через стандартное окно диалога
bool OpenDlgFile(HWND);
//выводит содержимое текстового файла в поле Edit
bool PrintTxtInEdit();
//оконная процедура модального окна "Выбрать параметры"
BOOL CALLBACK DbWndProc_GetParam(HWND, UINT, WPARAM, LPARAM);
//функции для Handle Message из оконной процедуры окна "Показать текст"
BOOL DbWndProc_GETPARAM_OnInitDialog(HWND, HWND, LPARAM);
void DbWndProc_GETPARAM_OnCommand(HWND, int, HWND, UINT);
void DbWndProc_GETPARAM_OnClose(HWND);

// Главная оконная функция
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow)
{

	WNDCLASSEX MainWin;
	MSG message;
	HWND hWnd;

	hInst = hInstance;

	memset(&MainWin, 0, sizeof(WNDCLASSEX));
	MainWin.cbSize = sizeof(WNDCLASSEX);
	MainWin.lpszClassName = TEXT("SimpleClassName2_1");
	MainWin.lpfnWndProc = Pr2_1_WndProc;
	MainWin.style = CS_VREDRAW | CS_HREDRAW;
	MainWin.hInstance = hInstance;
	MainWin.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	MainWin.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	MainWin.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	MainWin.lpszMenuName = MAKEINTRESOURCE(THIS_MENU);
	MainWin.cbClsExtra = 0;
	MainWin.cbWndExtra = 0;

	if (!RegisterClassEx(&MainWin))
	{
		MessageBox(NULL, _TEXT("Ошибка регистрации класса окна!"), _TEXT("ОШИБКА"), MB_OK | MB_ICONERROR);
		return false;
	}


	hWnd = CreateWindowEx(NULL,
		TEXT("SimpleClassName2_1"),
		TEXT("SP-LB2-1 Юшкевич Е.М. Гр. 70321-2"),
		WS_OVERLAPPEDWINDOW,
		600, 200,
		620, 460,
		NULL, NULL,
		hInstance, NULL);

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

//Обработка сообщений
LRESULT CALLBACK Pr2_1_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

bool WndProc_OnCreate(HWND hWnd, LPCREATESTRUCT ipCreateStruct)
{
	//записываем дескрипторы и идентификаторы текущего процесса и потока
	ProcHandle[0] = GetCurrentProcess();
	ProcId[0] = GetCurrentProcessId();
	ThreadHandle[0] = GetCurrentThread();
	ThreadId[0] = GetCurrentThreadId();

	if (!(hEdit = CreateWindowEx(0L, _TEXT("Edit"), _TEXT(""),
		WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE | WS_DISABLED,
		0, 0, 600, 400, hWnd, (HMENU)(IDC_EDIT_BLOCK),
		NULL, NULL)))
		return (false);

	return true;
}

void WndProc_OnPaint(HWND hWnd)
{

}

void WndProc_OnDestroy(HWND hWnd)
{

	PostQuitMessage(0);
}

// Обработка сообщений меню
void WndProc_OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sp, st;

	switch (id)
	{
	case OPEN_NOTE: //блокнот
		OpenNoteApp(hWnd, true, 1); //передаем хэндл окна, флаг закрытия дескрипторов и индекс
		break;

	case OPEN_NOTE_TXT: //блокнот с файлом
		OpenNoteApp(hWnd, false, 2); //передаем хэндл окна, флаг закрытия дескрипторов и индекс
		break;

	case OPEN_CALC: //калькулятор
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES sp, st;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);

		sp.nLength = sizeof(SECURITY_ATTRIBUTES);
		sp.lpSecurityDescriptor = NULL;
		sp.bInheritHandle = FALSE;
		st.nLength = sizeof(SECURITY_ATTRIBUTES);
		st.lpSecurityDescriptor = NULL;
		st.bInheritHandle = FALSE;

		if (!CreateProcess(ProcImage[3], NULL,
			&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
		{
			MessageBox(hWnd, TEXT("Ошибка запуска процесса"), NULL, MB_OK);
			break;
		}
		else
			SavingInfoAboutProc(pi.hProcess, pi.dwProcessId, pi.hThread, pi.dwThreadId, 3);
		break;
	}

	case CLOSE_NOTE: //закрытие блокнот
		//TerminateProcess(ProcHandle[1], exitCode);
		PostThreadMessage(ThreadId[1], WM_QUIT, 0, 0);
		break;

	case OPEN_NOTE_ANY_TXT: //открытие блокнота с выбором текста
		OpenDlgFile(hWnd); //для выбора файла
		OpenNoteApp(hWnd, false, 4); //для открытия блокнота
		break;

	case OPEN_TESTPROC:
	{
		//установка выбранных пользователем параметров окна
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_GETPARAM), hWnd, DbWndProc_GetParam, 0);

		if (!show)
			break;

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES sp, st;

		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);

		sp.nLength = sizeof(SECURITY_ATTRIBUTES);
		sp.lpSecurityDescriptor = NULL;
		sp.bInheritHandle = FALSE;
		st.nLength = sizeof(SECURITY_ATTRIBUTES);
		st.lpSecurityDescriptor = NULL;
		st.bInheritHandle = FALSE;


		//чтобы можно было назначать положение, режим отображения и размер
		si.dwFlags = STARTF_USEPOSITION | STARTF_USESHOWWINDOW | STARTF_USESIZE;
		//положение
		si.dwX = dwX;
		si.dwY = dwY;
		//размер
		si.dwXSize = dwXSize;
		si.dwYSize = dwYSize;
		//режим отображения
		si.wShowWindow = wShowWindow;

		if (!CreateProcess(ProcImage[5], NULL,
			&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
		{
			MessageBox(hWnd, TEXT("Ошибка запуска процесса"), NULL, MB_OK);
			break;
		}
		break;
	}

	case OPEN_NOTE_PRINT_TXT:
		OpenDlgFile(hWnd); //для выбора файла
		OpenNoteApp(hWnd, false, 4); //для открытия блокнота
		if (!PrintTxtInEdit())
			MessageBox(hWnd, TEXT("Не удалось прочитать содержимое файла."), TEXT("ОШИБКА!"), MB_OK);
		break;

	case THIS_PROCESS:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 0); //передаем индекс процесса последним параметром (принимается процедурой диалогового окна как lParam)
		break;

	case INFO_NOTE:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 1);
		break;

	case INFO_NOTE_TXT:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 2);
		break;

	case INFO_CALC:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 3);
		break;

	case INFO_NOTE_ANY_TXT:
		DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			MAKEINTRESOURCE(IDD_INFO_PROC), hWnd, DlgBoxInfo, 4);
		break;

	default:

		break;
	}
}

// Диалоговое окно
INT_PTR CALLBACK DlgBoxInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD PriorityCode;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		//инициализация элементов формы
	case WM_INITDIALOG:
		//структуры для хранения временных характеристик
		FILETIME ftKernelTime;
		FILETIME ftUserTime;
		FILETIME ftStart, ftEnd, ftCurrent;
		FILETIME ftNow;
		FILETIME ftTemp;
		//буфер для строки вывода в поле формы
		TCHAR Buff[200];
		//структура для арифметических вычислений над файловым времени
		ULARGE_INTEGER uiTempTime, uiTempTimeNow, uiCurrent;
		//код завершения
		DWORD dwExitCode;
		//время для вывода в поле формы
		SYSTEMTIME stCurrent;

		//файл программы и параметры командной строки
		SetDlgItemText(hDlg, IDC_NAME_PROG, ProcImage[lParam]);
		SetDlgItemText(hDlg, IDC_INFO_COMMAND, CmdParam[lParam]);

		//дескрипторы процесса и потока
		wsprintf(Buff, TEXT("0x%08X"), ProcHandle[lParam]);
		SetDlgItemText(hDlg, IDC_HANDLE_PROC, Buff);
		wsprintf(Buff, TEXT("0x%08X"), ThreadHandle[lParam]);
		SetDlgItemText(hDlg, IDC_HANDLE_THREAD, Buff);

		//идентификаторы процесса и потока
		wsprintf(Buff, TEXT("%d"), ThreadId[lParam]);
		SetDlgItemText(hDlg, IDC_ID_THREAD, Buff);
		wsprintf(Buff, TEXT("%d"), ProcId[lParam]);
		SetDlgItemText(hDlg, IDC_ID_PROC, Buff);

		// получаем время создания процесса, время окончания работы процесса, время работы в режиме ядра, время работы в режиме пользователя
		GetProcessTimes(ProcHandle[lParam], &ftStart, &ftEnd, &ftKernelTime, &ftUserTime);

		//код завершения процесса
		if (!GetExitCodeProcess(ProcHandle[lParam], &dwExitCode))
		{
			DWORD ErrCode = GetLastError();
			wsprintf(Buff, TEXT("Код ошибки: %d"), ErrCode);
		}
		else if (dwExitCode == STILL_ACTIVE)
		{
			wsprintf(Buff, TEXT("Процесс активен!"));
			//получаем текущее время, чтобы узнать время жизни процесса к настоящему моменту
			GetSystemTimeAsFileTime(&ftNow);
			uiTempTimeNow.HighPart = ftNow.dwHighDateTime;
			uiTempTimeNow.LowPart = ftNow.dwLowDateTime;
		}
		else
		{
			wsprintf(Buff, TEXT("%d"), dwExitCode);
			uiTempTimeNow.HighPart = ftEnd.dwHighDateTime;
			uiTempTimeNow.LowPart = ftEnd.dwLowDateTime;
		}


		SetDlgItemText(hDlg, IDC_EXIT_CODE, Buff);

		//код завершения потока
		if (!GetExitCodeThread(ThreadHandle[lParam], &dwExitCode))
		{
			DWORD ErrCode = GetLastError();
			wsprintf(Buff, TEXT("Код ошибки: %d"), ErrCode);
		}
		else if (dwExitCode == STILL_ACTIVE)
			wsprintf(Buff, TEXT("Процесс активен!"));
		else
			wsprintf(Buff, TEXT("%d"), dwExitCode);

		SetDlgItemText(hDlg, IDC_EXIT_CODE_THREAD, Buff);

		//идентификатор класса приоритета процесса
		PriorityCode = GetPriorityClass(ProcHandle[lParam]);
		wsprintf(Buff, TEXT("%d"), PriorityCode);
		SetDlgItemText(hDlg, IDC_CLASS_PRIORITY, Buff);

		//временные характеристики

		//время жизни приложения
		//заполняем поля ULARGE_INTEGER
		uiTempTime.HighPart = ftStart.dwHighDateTime;
		uiTempTime.LowPart = ftStart.dwLowDateTime;
		//получаем разницу
		uiCurrent.QuadPart = uiTempTimeNow.QuadPart - uiTempTime.QuadPart;
		//записываем в итоговую структуру FILETIME
		ftCurrent.dwHighDateTime = uiCurrent.HighPart;
		ftCurrent.dwLowDateTime = uiCurrent.LowPart;
		//переводим в системное время
		FileTimeToSystemTime(&ftCurrent, &stCurrent);
		//создаем строку
		wsprintf(Buff, TEXT("%d:%d:%d.%d"), stCurrent.wHour, stCurrent.wMinute, stCurrent.wSecond, stCurrent.wMilliseconds);
		SetDlgItemText(hDlg, IDC_TIMELIFE, Buff);

		//время простоя
		uiTempTime.HighPart = ftUserTime.dwHighDateTime;
		uiTempTime.LowPart = ftUserTime.dwLowDateTime;
		uiTempTimeNow.HighPart = ftKernelTime.dwHighDateTime;
		uiTempTimeNow.LowPart = ftKernelTime.dwLowDateTime;

		uiCurrent.QuadPart = uiCurrent.QuadPart - uiTempTime.QuadPart - uiTempTimeNow.QuadPart;
		ftCurrent.dwHighDateTime = uiCurrent.HighPart;
		ftCurrent.dwLowDateTime = uiCurrent.LowPart;
		FileTimeToSystemTime(&ftCurrent, &stCurrent);
		wsprintf(Buff, TEXT("%d:%d:%d.%d"), stCurrent.wHour, stCurrent.wMinute, stCurrent.wSecond, stCurrent.wMilliseconds);
		SetDlgItemText(hDlg, IDC_TIMEPAUSE, Buff);

		//время выполнения в режиме пользователя
		FileTimeToSystemTime(&ftUserTime, &stCurrent);
		wsprintf(Buff, TEXT("%d:%d:%d.%d"), stCurrent.wHour, stCurrent.wMinute, stCurrent.wSecond, stCurrent.wMilliseconds);
		SetDlgItemText(hDlg, IDC_TIMEUSER, Buff);

		//время выполнения в режиме ядра
		FileTimeToSystemTime(&ftKernelTime, &stCurrent);
		wsprintf(Buff, TEXT("%d:%d:%d.%d"), stCurrent.wHour, stCurrent.wMinute, stCurrent.wSecond, stCurrent.wMilliseconds);
		SetDlgItemText(hDlg, IDC_TIMECORE, Buff);

		return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	}
	return (INT_PTR)FALSE;
}

// Диалоговое окно "Выбрать параметры"
BOOL CALLBACK DbWndProc_GetParam(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, DbWndProc_GETPARAM_OnInitDialog);
		HANDLE_MSG(hDlg, WM_COMMAND, DbWndProc_GETPARAM_OnCommand);
		//HANDLE_MSG(hDlg, WM_CLOSE, DbWndProc_GETPARAM_OnClose);

	default:
		return FALSE;
	}

	return FALSE;
}

//при создании окна инициализирует комбобокс
BOOL DbWndProc_GETPARAM_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	HWND hCombo = GetDlgItem(hDlg, IDC_SHOW_STYLE);

	for (int i = 0; i < 4; i++)
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)ComboBoxStrings[i]);

	return 1;
}

void DbWndProc_GETPARAM_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDOK)
	{
		TCHAR buffTxt[100] = { 0 };

		GetDlgItemText(hDlg, IDC_X, buffTxt, 100);
		dwX = _wtoi(buffTxt);

		GetDlgItemText(hDlg, IDC_Y, buffTxt, 100);
		dwY = _wtoi(buffTxt);

		GetDlgItemText(hDlg, IDC_W, buffTxt, 100);
		dwXSize = _wtoi(buffTxt);

		GetDlgItemText(hDlg, IDC_H, buffTxt, 100);
		//strcpy(buffText, item);
		dwYSize = _wtoi(buffTxt);

		for (int i = 0; i < 4; i++)
		{
			GetDlgItemText(hDlg, IDC_SHOW_STYLE, buffTxt, 100);

			if (lstrcmp(buffTxt, ComboBoxStrings[i]) == 0)
			{
				wShowWindow = ComboBoxValues[i];
				break;
			}
		}

		show = true;

		EndDialog(hDlg, TRUE);
		//SendMessage(hDlg, WM_CLOSE, 0, 0);
	}

	else if (id == IDCANCEL)
	{
		show = false;
		EndDialog(hDlg, TRUE);
	}
}

//при закрытии окна записывает введенные данные в глобальные переменные
//void DbWndProc_GETPARAM_OnClose(HWND hDlg)
//{


//}



// Вспомогательные функции
//сохраняет данные о процессе в массивы
void SavingInfoAboutProc(HANDLE pHandle, DWORD pId, HANDLE thHandle, DWORD thId, int index)
{
	ProcHandle[index] = pHandle;
	ProcId[index] = pId;
	ThreadHandle[index] = thHandle;
	ThreadId[index] = thId;
}

//открывает блокнот с текстом либо без него
void OpenNoteApp(HWND hWnd, bool close, int index)
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

	if (!CreateProcess(ProcImage[index], CmdParam[index],
		&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(hWnd, TEXT("Ошибка запуска процесса"), NULL, MB_OK);
		return;
	}
	else
	{
		//сохраняем значения дескрипторов и идентификаторов
		SavingInfoAboutProc(pi.hProcess, pi.dwProcessId, pi.hThread, pi.dwThreadId, index);

		if (close)
		{
			//закрываем дескрипторы процесса и потока, чтобы "отвязать" дочерний процесс от родительского приложения
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
}

bool PrintTxtInEdit()
{
	WaitForSingleObject(ProcHandle[4], INFINITE); //основной процесс ожидает, пока завершится дочерний
	hFile = CreateFile(path, GENERIC_READ,
		0, (LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	CHAR TempText[MAX_BYTES] = { 0 }; //буфер для перекодировки текста

									  //чтение из файла в буфер
	DWORD dwNumbOfBytes; //сохранит число прочитанных байтов
	ReadFile(hFile, TempText, MAX_BYTES, &dwNumbOfBytes, NULL); //во временную строку

																//закрываем файл
	if (hFile)
		CloseHandle(hFile);

	//записываем содержимое файла в поле редактирования
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, TempText, -1, BuffText, dwNumbOfBytes); //конвертация в правильную кодировку
	BuffText[dwNumbOfBytes] = '\0';
	//LPCSTR result = BuffText;
	//SetDlgItemText(hEdit, IDC_EDIT_BLOCK, BuffText); //альтернативный способ
	SendMessage(hEdit, WM_SETTEXT, NULL, (LPARAM)BuffText); //первый способ установить текст элемента

	return true;
}

bool OpenDlgFile(HWND hWnd)
{
	//заполняем структуру модального окна "Открыть файл"

	OPENFILENAME ofn;	// структура для common dialog box
	TCHAR lpszFileSpec[260] = { 0 };	// массив для имени файла
	HANDLE hFile;	// дескриптор файла


					//Иницализация OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;  // дескриптор окна–влвдельца
	ofn.lpstrFile = lpszFileSpec;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(lpszFileSpec);
	fileName[0] = '\0';

	ofn.lpstrFilter = _TEXT("All\0*.*\0Text\0*.TXT\0"); 
	ofn.nFilterIndex = 1; // Индекс для текущего шаблона фильтра
	ofn.lpstrFileTitle = NULL; // Без заголовка
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL; //путь в начальному каталогу или автоматический выбор начального каталога
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; //пользователю разрешается вводить только допустимые имена файлов и пути

													   // Отображение диалогового окна
	if (!GetOpenFileName(&ofn))
		return false;//ошибка в диалоге

	wsprintf(path, TEXT("%s"), lpszFileSpec); //запоминаем путь к файлу
	wsprintf(fileName, TEXT("C:\\Windows\\Notepad.exe %s"), lpszFileSpec); //формируем командную строку
	CmdParam[4] = fileName;
	return true;
}