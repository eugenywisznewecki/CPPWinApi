#include "YUSH_SR_DLL.h" //подключение заголовочного файла
#include <windows.h> //для поддержи функционала DLL
#include <tchar.h>
#include <atlconv.h>

////////////////////////
//глобальные переменные
///////////////////////

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

//главная функция библиотеки
BOOL APIENTRY DllMain(HMODULE hModule, DWORD fgwReason, LPVOID lpReserved)
{
	switch (fgwReason)
	{
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, L"Загружается библиотека SP_Lab2_4_SR_DLL", L"ОПОВЕЩЕНИЕ", MB_OK);
		break;
	case DLL_THREAD_ATTACH:

		break;
	case DLL_THREAD_DETACH:

		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, L"Библиотека SP_Lab2_4_SR_DLL отключается", L"ОПОВЕЩЕНИЕ", MB_OK);
		break;
	}
	return TRUE;
}

//1. Открывает модальное окно для выбора файла, ИСТИНА в случае успеха
bool OpenDlgFile(HWND hWnd)
{
	OPENFILENAME ofn;	// структура для common dialog box
	TCHAR lpszFileSpec[260] = { 0 };	// массив для имени файла
	HANDLE hFile;	// дескриптор файла

					//заполнение OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;  // дескриптор окна–влвдельца
	ofn.lpstrFile = lpszFileSpec;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(lpszFileSpec);
	fileName[0] = '\0';

	ofn.lpstrFilter = _TEXT("All\0*.*\0Text\0*.txt\0");
	ofn.nFilterIndex = 1; // Индекс для текущего шаблона фильтра
	ofn.lpstrFileTitle = NULL; // Без заголовка
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL; //путь в начальному каталогу или автоматический выбор начального каталога
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; //пользователю разрешается вводить только допустимые имена файлов и пути

													   // Отображение диалогового окна
	if (!GetOpenFileName(&ofn))
		return false;//ошибка в диалоге

	wsprintf(fileName, TEXT("%s"), lpszFileSpec); //запоминаем путь к файлу

	return true;
}

//2. Открывает файл, сохраняя его HANDLE, ИСТИНА в случае успеха
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

//3. Читает содержимое файла в буфер, ИСТИНА в случае успеха
bool ReadTextFromFile()
{
	CHAR TempText[MAX_LOADSTRING] = { 0 }; //буфер для перекодировки текста

	if (txtFile != NULL)
	{
		if (ReadFile(txtFile, TempText, MAX_LOADSTRING + MAX_INFOSTRING, &dwNumbOfBytes, NULL))
		{
			CloseHandle(txtFile);

			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, TempText, -1, text, dwNumbOfBytes); //конвертация в правильную кодировку
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
		if (text[start] == '\0') //если подошло к концу строки, возвращаем -1
			return -1;
		if (IsCharAlpha(text[start])) //если нашло символ алфавита, возвращаем его индекс
			return start;
		else //иначе увеличивает значение индекса на 1 и продолжает цикл
			start++;
	}
}

//6. Подсчитывает длину слова (количество символов), возвращает количество символов
UINT CountLengthWord(INT start, INT* end)
{
	UINT length = 0; //задаем начальное значение длины слова

	while (IsCharAlpha(text[start]) || text[start] == '-') //пока символ является буквой или дефисом
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

	txtFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

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
	return fileName;
}