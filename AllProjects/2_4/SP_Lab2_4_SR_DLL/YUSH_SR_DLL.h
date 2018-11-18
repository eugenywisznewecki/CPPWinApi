#pragma once
#include <windows.h>

#ifdef DLL_EXPORTS
#define EXPORT extern "C" __declspec(dllexport) 
#else
#define EXPORT extern "C" __declspec(dllimport) 
#endif

//определяем максимальный размер буфера строки
#define MAX_LOADSTRING 10000
//определяем максимальный размер буфера результата
#define MAX_INFOSTRING 500

//максимальная длина слова в русском языке
#define MAX_LENGTH_WORD 55

/////////////////////////
//экспортируемые функции
////////////////////////
EXPORT bool OpenDlgFile(HWND hWnd); //позволяет выбрать файл при помощи стандартного диалогового окна
EXPORT bool LoadFile();				//открывает файл
EXPORT bool ReadTextFromFile();		//считывает содержимое файла и закрывает его
EXPORT UINT SearchingLengthWords(); //находит распределение слов и возвращает максимальную длину
EXPORT bool FinalizeReport();		//создает отчет и записывает в конец файла
EXPORT LPTSTR GetFilePath();		//возвращаеть путь к файлу

////////////////////
//служебные функции
///////////////////
INT FindNextWord(INT);				//находит очередное слово для обработки
UINT CountLengthWord(INT, INT*);	//подсчитывает и возвращает длину слова
void WriteDataLength(UINT);			//фиксирует данные о слове соответствующей длины в массиве
bool CreateReport();				//создает отчет для записи в конец файла
bool UpdateFile();					//записывает отчет в конец файла