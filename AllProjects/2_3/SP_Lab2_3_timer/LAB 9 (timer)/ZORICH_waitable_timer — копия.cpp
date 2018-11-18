#define _WIN32_WINDOWS 0x0410 
#define WINVER 0x0400

#include <windows.h>
#include <stdio.h>
#include <locale.h>

DWORD WINAPI OpenNoteApp(LPVOID);

//���������� ���������� �� 100 �� � 1 �������
const int nTimerUnitsPerSecond = 10000000;
HANDLE ProcHandle; //��� ������������ ���������;
DWORD ProcId; //��� ��������������� ���������;
HANDLE ThreadHandle; //��� ������������ �������;
DWORD ThreadId;// ��� ��������������� �������;
LPTSTR ProcImage = TEXT("C:\\Windows\\Notepad.exe"); //��� ���������� �����, ��������������� ����� ����������� ��������;
LPTSTR CmdParam = TEXT("C:\\Windows\\Notepad.exe"); //��� ����� c ����������� ����������� ��������.
//��� ���������� ������, ������������ �������
//HANDLE hSecThread = NULL;
DWORD dwSecThreadId = 0;
//HWND hwndNotepad;
//HWND hwndEditNotepad;
HANDLE hTimerThread;


void main(void)
{
	//��� ��������� �������� �����
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_CTYPE, "Russian");
	
	//���������� ������������ �������
	int cAbrasionCount = 0; 
	SYSTEMTIME st;

	//��������� ��� ���������� � FILETIME
	LARGE_INTEGER li;

	//�������� �������
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	hTimerThread = CreateWaitableTimer(NULL, FALSE, NULL);

	printf("\n������ ������ (T4).\n����������� - ����� �. �.\n������ - 50235-2");
	printf("\n\n����� 15 ������ ����� ������� ��������� Notepad.exe,\n��������� � ���� �������� ����� ������������ �������.\n����� 10 ������ ������� �����������.\n\n");

	///����� ������������ ������������� �����, � �� ����������, ������ �������� �������������
	li.QuadPart = -(15 * nTimerUnitsPerSecond);

	//��������� �������
	//�������� ����� ���������� �������,
	//����� ������� ������������ - ����� 15 ������,
	//��������� ����������� ������������ - ����� ������ 15 ������
	if (SetWaitableTimer(hTimer, &li, 15 * 1000, NULL, NULL, FALSE))
	{
		while (TRUE){
			WaitForSingleObject(hTimer, INFINITE);
			GetLocalTime(&st);
			cAbrasionCount++;
			printf("����� �������� �������� (1-� ������): \t\t%.2d:%.2d:%.2d\n������ � %.2d\n", st.wHour, st.wMinute, st.wSecond, cAbrasionCount);
			//������� �����, ��������� ��� ���������� ������������ �������
			HANDLE hSecThread = CreateThread(NULL, 0, OpenNoteApp, (LPVOID)cAbrasionCount, 0, &dwSecThreadId);
			WaitForSingleObject(hTimerThread, INFINITE);
			GetLocalTime(&st);
			printf("����� �������� �������� (2-� ������): \t\t%.2d:%.2d:%.2d\n\n", st.wHour, st.wMinute, st.wSecond);
			//DWORD dw = WaitForSingleObject(hSecThread, INFINITE);
			//Sleep(3000);
			//bool how = TerminateThread(hSecThread, 101);
		}
	}
}

//��������� �������
DWORD WINAPI OpenNoteApp(LPVOID indexTimer)
{
	STARTUPINFO si; //��������� ��� �������� ���������
	ZeroMemory(&si, sizeof(STARTUPINFO)); //������� ����������� ��������� �� ������
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi; //���������, ���� ��������� ���������� � ��������
	SECURITY_ATTRIBUTES sp, st; //�������� ������ ��� �������� � ������
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&sp, sizeof(SECURITY_ATTRIBUTES));
	ZeroMemory(&st, sizeof(SECURITY_ATTRIBUTES));
	sp.nLength = sizeof(SECURITY_ATTRIBUTES);
	sp.lpSecurityDescriptor = NULL;
	sp.bInheritHandle = FALSE; //��������� ������������ ���������� �������� ��������
	st.nLength = sizeof(SECURITY_ATTRIBUTES);
	st.lpSecurityDescriptor = NULL;
	st.bInheritHandle = FALSE; ////��������� ������������ ���������� ������ �������� ��������

	if (!CreateProcess(ProcImage, NULL,
		&sp, &st, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(NULL, TEXT("������ ������� ��������"), NULL, MB_OK);
		return 0;
	}
	else
	{

		//�����, ����� ������� ������ ������� � �������� ����
		//� ��������� ������ ����� �� ����� ���������� � ����
		Sleep(100);

		//������� ���� ��������
		HWND hwndNotepad = FindWindow(TEXT("Notepad"), NULL);
		
		//��������� ������ ��� ������
		TCHAR textForNPad[100] = {0};
		wsprintf(textForNPad, TEXT("������ � %.2d"), (int)indexTimer);

		//������� ���� ��������������
		HWND hwndEditNotepad = FindWindowEx(hwndNotepad, 0, TEXT("Edit"), NULL);
		//������������� ���������
		//bool result = SetWindowText(hwndEditNotepad, textForNPad);
		SendMessage(hwndEditNotepad, WM_SETTEXT, 0, (LPARAM)textForNPad);
		//������� 10 ������
		//Sleep(10000);
		LARGE_INTEGER lit;
		//������������� ����� ������������ ������� �� ���� ����� 10 ������ (���� 
		lit.QuadPart = -(99000000);
		SetWaitableTimer(hTimerThread, &lit, NULL, NULL, NULL, FALSE);
		//��������� �������
		//WaitForSingleObject(hTimerThread, INFINITE);
		Sleep(10000);
		//result = SendMessage(hwndNotepad, WM_QUIT, 0, 0);
		TerminateProcess(pi.hProcess, 10001);
		//Sleep(100);
	}
	ExitThread(102);

	return 0;
}