#pragma once
#include <windows.h>

#ifdef DLL_EXPORTS
#define EXPORT extern "C" __declspec(dllexport) 
#else
#define EXPORT extern "C" __declspec(dllimport) 
#endif

//���������� ������������ ������ ������ ������
#define MAX_LOADSTRING 10000
//���������� ������������ ������ ������ ����������
#define MAX_INFOSTRING 500

//������������ ����� ����� � ������� �����
#define MAX_LENGTH_WORD 55

/////////////////////////
//�������������� �������
////////////////////////
EXPORT bool OpenDlgFile(HWND hWnd); //��������� ������� ���� ��� ������ ������������ ����������� ����
EXPORT bool LoadFile();				//��������� ����
EXPORT bool ReadTextFromFile();		//��������� ���������� ����� � ��������� ���
EXPORT UINT SearchingLengthWords(); //������� ������������� ���� � ���������� ������������ �����
EXPORT bool FinalizeReport();		//������� ����� � ���������� � ����� �����
EXPORT LPTSTR GetFilePath();		//����������� ���� � �����

////////////////////
//��������� �������
///////////////////
INT FindNextWord(INT);				//������� ��������� ����� ��� ���������
UINT CountLengthWord(INT, INT*);	//������������ � ���������� ����� �����
void WriteDataLength(UINT);			//��������� ������ � ����� ��������������� ����� � �������
bool CreateReport();				//������� ����� ��� ������ � ����� �����
bool UpdateFile();					//���������� ����� � ����� �����