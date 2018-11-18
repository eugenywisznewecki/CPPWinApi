//#define _WIN32_WINDOWS 0x0410 
//#define WINVER 0x0400

#include <windows.h>
#include <stdio.h>
#include <locale.h>

DWORD WINAPI OpenNoteApp(LPVOID);

//количество интервалов по 100 нс в 1 секунде
const int nTimerUnitsPerSecond = 10000000;
//строка, указывающая на блокнот
LPTSTR ProcImage = TEXT("C:\\Windows\\Notepad.exe");
//глобальный таймер для уведомления главного потока
HANDLE hTimerThread;


void main(void)
{
	//для поддержки русского языка
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_CTYPE, "Russian");

	//количество срабатываний таймера
	int cAbrasionCount = 0;
	//для вывода времени выполнения операций
	SYSTEMTIME st;

	//структура для арифметики с FILETIME
	LARGE_INTEGER li;

	//создание таймеров
	//главного потока
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	//глобального таймера
	hTimerThread = CreateWaitableTimer(NULL, FALSE, NULL);

	printf("\nЖдущий таймер (T4).\nПрограммист - Юшкевич Е.М.\nГруппа - 70231-2");
	printf("\n\nЧерез 15 секунд после запуска открывает Notepad.exe,\nотображая в окне блокнота номер срабатывания таймера.\nЧерез 10 секунд блокнот закрывается.\n\n");

	//выводим текущее время
	GetLocalTime(&st);
	printf("Время запуска программы: \t\t%.2d:%.2d:%.2d\n\n", st.wHour, st.wMinute, st.wSecond, cAbrasionCount);

	///чтобы использовать относительное время, а не абсолютное, делаем значение отрицательным
	li.QuadPart = -(15 * nTimerUnitsPerSecond);

	//установка таймера
	//передаем хэндл созданного таймера,
	//время первого срабатывания - через 15 секунд,
	//интервалы последующий срабатываний - через каждые 15 секунд
	if (SetWaitableTimer(hTimer, &li, 15 * 1000, NULL, NULL, FALSE))
	{
		while (TRUE) {
			WaitForSingleObject(hTimer, INFINITE);
			GetLocalTime(&st);
			cAbrasionCount++;
			printf("Время открытия блокнота (1-й таймер): \t\t%.2d:%.2d:%.2d\nЗапуск № %.2d\n", st.wHour, st.wMinute, st.wSecond, cAbrasionCount);
			//создаем поток, отправляя ему количество срабатываний таймера
			DWORD dwSecThreadId = 0;
			HANDLE hSecThread = CreateThread(NULL, 0, OpenNoteApp, (LPVOID)cAbrasionCount, 0, &dwSecThreadId);
			WaitForSingleObject(hTimerThread, INFINITE);
			GetLocalTime(&st);
			printf("Время закрытия блокнота (2-й таймер): \t\t%.2d:%.2d:%.2d\n\n", st.wHour, st.wMinute, st.wSecond);
		}
	}
}

//дочерний поток открывает блокнот
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
		Sleep(200);

		//находим окно блокнота
		HWND hwndNotepad = FindWindow(TEXT("Notepad"), NULL);

		//формируем строку для вывода
		TCHAR textForNPad[100] = { 0 };
		wsprintf(textForNPad, TEXT("Запуск № %.2d"), (int)indexTimer);

		//находим поле редактирования
		HWND hwndEditNotepad = FindWindowEx(hwndNotepad, 0, TEXT("Edit"), NULL);

		//посылаем текст в поле редактирования
		SendMessage(hwndEditNotepad, WM_SETTEXT, 0, (LPARAM)textForNPad);

		LARGE_INTEGER lit;
		//устанавливаем время срабатывания таймера на чуть менее 10 секунд (учет паузы на создание окна) 
		lit.QuadPart = -(98000000);

		//создаем таймер для главного потока
		SetWaitableTimer(hTimerThread, &lit, NULL, NULL, NULL, FALSE);

		//ждем чуть меньше 10 секунд (с учетом паузы на создание окна)
		Sleep(9800);

		//закрываем блокнот
		TerminateProcess(pi.hProcess, 10001);
	}

	//завершаем дочерний поток
	ExitThread(102);

	return 0;
}