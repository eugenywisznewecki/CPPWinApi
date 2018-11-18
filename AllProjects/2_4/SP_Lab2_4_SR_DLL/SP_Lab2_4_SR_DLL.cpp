#include "YUSH_SR_DLL.h" //����������� ������������� �����
#include <windows.h> //��� �������� ����������� DLL
#include <tchar.h>
#include <atlconv.h>

////////////////////////
//���������� ����������
///////////////////////

//����� ���������� �����
HANDLE txtFile;

//�������� ����� ������, ����������� �� �����
DWORD dwNumbOfBytes;

//������ ������ ������ �������� �� ��������� ���� (��������� ������ � �������)
UINT maxLength;

//������ ��� �������� ���������� ����, ������ ���������� ���������� ����, � � ������ - ���������� ���������� ����
UINT results[MAX_LENGTH_WORD];

//������� �����
TCHAR text[MAX_LOADSTRING + MAX_INFOSTRING];

//���� � �����
TCHAR fileName[260];

//�����
TCHAR reportTXT[MAX_INFOSTRING];

//������� ������� ����������
BOOL APIENTRY DllMain(HMODULE hModule, DWORD fgwReason, LPVOID lpReserved)
{
	switch (fgwReason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, L"����������� ���������� SP_Lab2_4_SR_DLL", L"����������", MB_OK);
		break;
	case DLL_THREAD_ATTACH:

		break;
	case DLL_THREAD_DETACH:

		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, L"���������� SP_Lab2_4_SR_DLL �����������", L"����������", MB_OK);
		break;
	}
	return TRUE;
}

//1. ��������� ��������� ���� ��� ������ �����, ������ � ������ ������
bool OpenDlgFile(HWND hWnd)
{
	OPENFILENAME ofn;	// ��������� ��� common dialog box
	TCHAR lpszFileSpec[260] = { 0 };	// ������ ��� ����� �����
	HANDLE hFile;	// ���������� �����

					//���������� OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;  // ���������� ��������������
	ofn.lpstrFile = lpszFileSpec;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(lpszFileSpec);
	fileName[0] = '\0';

	ofn.lpstrFilter = _TEXT("All\0*.*\0Text\0*.txt\0");
	ofn.nFilterIndex = 1; // ������ ��� �������� ������� �������
	ofn.lpstrFileTitle = NULL; // ��� ���������
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL; //���� � ���������� �������� ��� �������������� ����� ���������� ��������
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; //������������ ����������� ������� ������ ���������� ����� ������ � ����

													   // ����������� ����������� ����
	if (!GetOpenFileName(&ofn))
		return false;//������ � �������

	wsprintf(fileName, TEXT("%s"), lpszFileSpec); //���������� ���� � �����

	return true;
}

//2. ��������� ����, �������� ��� HANDLE, ������ � ������ ������
bool LoadFile()
{
	txtFile = NULL;
	txtFile = CreateFile(fileName, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

	if (txtFile == INVALID_HANDLE_VALUE)
	{
		txtFile = NULL;
		return false;
	}

	else
		return true;
}

//3. ������ ���������� ����� � �����, ������ � ������ ������
bool ReadTextFromFile()
{
	CHAR TempText[MAX_LOADSTRING] = { 0 }; //����� ��� ������������� ������

	if (txtFile != NULL)
	{
		if (ReadFile(txtFile, TempText, MAX_LOADSTRING + MAX_INFOSTRING, &dwNumbOfBytes, NULL))
		{
			CloseHandle(txtFile);

			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, TempText, -1, text, dwNumbOfBytes); //����������� � ���������� ���������
			text[dwNumbOfBytes] = '\0';

			return true;
		}

		else
			return false;
	}

	else
	{
		return false;
	}
}

//4. ����� �������: ������� ������������� ���� �� ����� � ���������� ������������ �����
UINT SearchingLengthWords()
{
	maxLength = 0;
	UINT length = 0;

	INT start = FindNextWord(0); //�������� �������� ������ � ������ - ���� ������ �����

	while (start != -1) //���� ������� ��������� �����
	{
		length = CountLengthWord(start, &start); //�������� �� ������ ����� ���������� ����� � ������ ��� ������ ����������

		if (maxLength < length) //����������, �������� ����� ����� ������������
			maxLength = length;

		WriteDataLength(length); //���������� ������ � ��������� �����

		start = FindNextWord(start); //�������� ������ ���������� ����� ��� 0, ���� ��� �� �������
	}

	return maxLength; //���������� ����� ������������� �����
}

//5. ������� ��������� ����� (��� ������-�������� �� ������), ���� ����� �� �������, ���������� 0
INT FindNextWord(INT start)
{
	while (true)
	{
		if (text[start] == '\0') //���� ������� � ����� ������, ���������� -1
			return -1;
		if (IsCharAlpha(text[start])) //���� ����� ������ ��������, ���������� ��� ������
			return start;
		else //����� ����������� �������� ������� �� 1 � ���������� ����
			start++;
	}
}

//6. ������������ ����� ����� (���������� ��������), ���������� ���������� ��������
UINT CountLengthWord(INT start, INT* end)
{
	UINT length = 0; //������ ��������� �������� ����� �����

	while (IsCharAlpha(text[start]) || text[start] == '-') //���� ������ �������� ������ ��� �������
	{
		length++; //����������� �������� �����
		start++; //��������� � ���������� �������
	}

	*end = start; //���������� ������ ���������� �� ������ �������

	return length; //���������� ����� �����
}

//7. ���������� ������ � ��������� ����� � ���������� ������
void WriteDataLength(UINT length)
{
	results[length] += 1;
}

//8. ��������� ����� � �������� ������ � ���������� ����� � �������� ����
bool FinalizeReport()
{
	if (CreateReport()) //���� ����� �� ����
	{
		if (UpdateFile()) //���� ������� ��������
			return true;

		else
			return false;
	}

	else
		return false;
}

//9. ������� ����� ��� ������ � ����� �����
bool CreateReport()
{
	TCHAR buff[100]; //����� ��� ������������ ������ �� ������ �������
	buff[0] = '\0';
	reportTXT[0] = '\0'; //���������� ���������� ������ ������
	bool IsResults = false; //���� �� � ������� ��������

	lstrcpy(reportTXT, _TEXT("\r\n������������� ����:\r\n"));

	for (int i = 1; i <= maxLength; i++) //�������� �� ��������������� �������
	{
		if (results[i] != 0) //���� � ������ ����������� ����� � ������, ������ �������
		{
			wsprintf(buff, _TEXT("�����: %d | ���������� ����: %d\r\n"), i, results[i]); //��������� ������ �� ���������� �����
			lstrcat(reportTXT, buff); //��������� � ������
			IsResults = true; //����� �������
		}
	}

	return IsResults;
}

//10. �������������� ����
bool UpdateFile()
{
	if (fileName == NULL || (lstrcmp(reportTXT, _TEXT("")) == 0))
		return false;

	txtFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (txtFile == NULL)
		return false;

	else
	{
		DWORD dwBytesToWrite = lstrlen(reportTXT);
		DWORD dwBytesWritten;

		//������������ � ANSI
		CHAR* inFile;
		USES_CONVERSION;
		inFile = W2A(reportTXT);

		SetFilePointer(txtFile, 0, 0, FILE_END); //������������� ��������� � �����
		WriteFile(txtFile, inFile, strlen(inFile), &dwBytesWritten, NULL); //���������� ����� � ����
		CloseHandle(txtFile); //��������� ����

		return true;
	}
}

//11. ���������� ���� � �����
LPTSTR GetFilePath()
{
	return fileName;
}