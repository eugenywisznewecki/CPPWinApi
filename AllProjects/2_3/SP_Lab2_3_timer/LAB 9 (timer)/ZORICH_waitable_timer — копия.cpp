#define _WIN32_WINDOWS 0x0410 
#define WINVER 0x0400

#include <windows.h>
#include <stdio.h>
#include <locale.h>

DWORD WINAPI OpenNoteApp(LPVOID);

//количество интервалов по 100 нс в 1 секунде
const int nTimerUnitsPerSecond = 10000000;
HANDLE ProcHandle; //для дескрипторов процессов;
DWORD ProcId; //для идентификаторов процессов;
HANDLE ThreadHandle; //для дескрипторов потоков;
DWORD ThreadId;// для идентификаторов потоков;
LPTSTR ProcImage = TEXT("C:\\Windows\\Notepad.exe"); //для указателей строк, идентифицирущих файлы запускаемых программ;
LPTSTR CmdParam = TEXT("C:\\Windows\\Notepad.exe"); //для строк c параметрами запускаемых программ.
//для отдельного потока, запускающего блокнот
//HANDLE hSecThread = NULL;
DWORD dwSecThreadId = 0;
//HWND hwndNotepad;
//HWND hwndEditNotepad;
HANDLE hTimerThread;


void main(void)
{
	//для поддержки русского языка
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_CTYPE, "Russian");
	
	//количество срабатываний таймера
	int cAbrasionCount = 0; 
	SYSTEMTIME st;

	//структура для арифметики с FILETIME
	LARGE_INTEGER li;

	//создание таймера
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	hTimerThread = CreateWaitableTimer(NULL, FALSE, NULL);

	printf("\nЖдущий таймер (T4).\nПрограммист - Зорич Д. А.\nГруппа - 50235-2");
	printf("\n\nЧерез 15 секунд после запуска открывает Notepad.exe,\nотображая в окне блокнота номер срабатывания таймера.\nЧерез 10 секунд блокнот закрывается.\n\n");

	///чтобы использовать относительное время, а не абсолютное, делаем значение отрицательным
	li.QuadPart = -(15 * nTimerUnitsPerSecond);

	//установка таймера
	//передаем хэндл созданного таймера,
	//время первого срабатывания - через 15 секунд,
	//интервалы последующий срабатываний - через каждые 15 секунд
	if (SetWaitableTimer(hTimer, &li, 15 * 1000, NULL, NULL, FALSE))
	{
		while (TRUE){
			WaitForSingleObject(hTimer, INFINITE);
			GetLocalTime(&st);
			cAbrasionCount++;
			printf("Время открытия блокнота (1-й таймер): \t\t%.2d:%.2d:%.2d\nЗапуск № %.2d\n", st.wHour, st.wMinute, st.wSecond, cAbrasionCount);
			//создаем поток, отправляя ему количество срабатываний таймера
			HANDLE hSecThread = CreateThread(NULL, 0, OpenNoteApp, (LPVOID)cAbrasionCount, 0, &dwSecThreadId);
			WaitForSingleObject(hTimerThread, INFINITE);
			GetLocalTime(&st);
			printf("Время закрытия блокнота (2-й таймер): \t\t%.2d:%.2d:%.2d\n\n", st.wHour, st.wMinute, st.wSecond);
			//DWORD dw = WaitForSingleObject(hSecThread, INFINITE);
			//Sleep(3000);
			//bool how = TerminateThread(hSecThread, 101);
		}
	}
}

//открывает блокнот
DWORD WINAPI OpenNoteApp(LPVOID indexTimer)
{
	STARTUPINFO si; //структура для создания процессов
	ZeroMemory(&si, sizeof(STARTUPINFO)); //очистка содержимого структуры от мусора
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi; //структура, куда запишется информация о процессе
	SECURITY_ATTRIBUTES sp, st; //атрибуты защиты для процесса и потока
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&sp, sizeof(SECURITY_ATTRIBUTES));
	ZeroMemory(&st, sizeof(SECURITY_ATTRIBUTES));
	sp.nLength = sizeof(SECURITY_ATTRIBUTES);
	sp.lpSecurityDescriptor = NULL;
	sp.bInheritHandle = FALSE; //запрещаем наследование описателей главного процесса
	st.nLength = sizeof(SECURITY_ATTRIBUTES);
	st.lpSecurityDescriptor = NULL;
	st.bInheritHandle = FALSE; ////запрещаем наследование описателей потока главного процесса

	if (!CreateProcess(ProcImage, NULL,
		&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(NULL, TEXT("Ошибка запуска процесса"), NULL, MB_OK);
		return 0;
	}
	else
	{

		//пауза, чтобы система успела создать и показать окно
		//в противном случае текст не будет установлен в поле
		Sleep(100);

		//находим окно блокнота
		HWND hwndNotepad = FindWindow(TEXT("Notepad"), NULL);
		
		//формируем строку для вывода
		TCHAR textForNPad[100] = {0};
		wsprintf(textForNPad, TEXT("Запуск № %.2d"), (int)indexTimer);

		//находим поле редактирования
		HWND hwndEditNotepad = FindWindowEx(hwndNotepad, 0, TEXT("Edit"), NULL);
		//устанавливаем сообщение
		//bool result = SetWindowText(hwndEditNotepad, textForNPad);
		SendMessage(hwndEditNotepad, WM_SETTEXT, 0, (LPARAM)textForNPad);
		//ожидаем 10 секунд
		//Sleep(10000);
		LARGE_INTEGER lit;
		//устанавливаем время срабатывания таймера на чуть менее 10 секунд (учет 
		lit.QuadPart = -(99000000);
		SetWaitableTimer(hTimerThread, &lit, NULL, NULL, NULL, FALSE);
		//закрываем блокнот
		//WaitForSingleObject(hTimerThread, INFINITE);
		Sleep(10000);
		//result = SendMessage(hwndNotepad, WM_QUIT, 0, 0);
		TerminateProcess(pi.hProcess, 10001);
		//Sleep(100);
	}
	ExitThread(102);

	return 0;
}