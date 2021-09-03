#include <iostream>
#include <windows.h>
#include <inttypes.h>
#define MAX_DATA_LENGTH 16383

using namespace std;
#define WINVER 0x0502
#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH
#define INFO_BUFFER_SIZE 32767
#define MAX_KEY_LENGTH 255

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
	//Замер рабочей частоты f ЦП
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency))
		cout << "\nQueryPerformanceFrequency не удалось выполнить!\n";

	PCFreq = double(frequency.QuadPart);
	printf("\nРабочая частота ЦП, Hz: %u\n", frequency);
	QueryPerformanceCounter(&frequency);
	CounterStart = frequency.QuadPart;
}

//Подсчет количества тактов ЦП
double GetCounter()
{
	LARGE_INTEGER frequency;
	QueryPerformanceCounter(&frequency);
	return double(frequency.QuadPart - CounterStart) / PCFreq;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	DWORD  dwVersion = 0;
	DWORD  dwMajorVersion = 0;
	DWORD  dwMinorVersion = 0;
	DWORD  dwBuild = 0;
	TCHAR  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;
	HANDLE hFindVol;
	BOOL   hNextVol, hCloseVol;
	char   buf[MAX_PATH];
	char   FileSysNameBuf[FILESYSNAMEBUFSIZE];
	DWORD  VSNumber;
	DWORD  MCLength;
	DWORD  FileSF;
	BOOL   test;

	// Определение версии Windows
	dwVersion = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// Получение номера сборки
	dwBuild = (DWORD)(HIWORD(dwVersion));

	printf("Версия ОС:  %d.%d.%d\n",
		dwMajorVersion,
		dwMinorVersion,
		dwBuild);

	//Определение системного каталога
	GetSystemDirectory(infoBuf, INFO_BUFFER_SIZE);
	printf("\nСистемный каталог: %s\n", infoBuf);

	//Определение имени пользователя
	GetUserName(infoBuf, &bufCharCount);
	printf("\nИмя пользователя:  %s\n", infoBuf);

	//Определение названия компьютера
	bufCharCount = INFO_BUFFER_SIZE;
	GetComputerName(infoBuf, &bufCharCount);
	printf("\nИмя компьютера: %s\n", infoBuf);

	//Для томов
    DWORD  CharCount = MAX_PATH;
    char Names[MAX_PATH];
    CHAR VolumeName[MAX_PATH];

	//Имя, путь, объем и кол-во свободного места для первого тома
    hFindVol = FindFirstVolume(VolumeName, BUFSIZE);
    printf("\nИмя первого тома: ");
    printf(VolumeName);
	__int64 total, available, free;
    GetVolumePathNamesForVolumeName(VolumeName, Names, CharCount, &CharCount);
    printf("\nПервый путь в томе: ");
    printf(Names);
    GetDiskFreeSpaceEx((LPCSTR)Names, (PULARGE_INTEGER)&available, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free);
	cout << "\nОбъем тома: " << total << " байт, " << "  количество свободного места: " << available << " байт\n";

	//Имя, путь, объем и кол-во свободного места для последующего тома
	while (1) {                   // бесконечный цикл, будет выполняться, пока не оборвется break-ом
		hNextVol = FindNextVolume(hFindVol, VolumeName, BUFSIZE);
		if (GetLastError() != 18) // код ошибки "больше нет файлов"
		{
			printf("\nСледующие имя тома: ");
			printf(VolumeName);
			GetVolumePathNamesForVolumeName(VolumeName, Names, CharCount, &CharCount);
			printf("\nПервый путь в томе: ");
			printf(Names);
			GetDiskFreeSpaceEx((LPCSTR)Names, (PULARGE_INTEGER)&available, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free);
			cout << "\nОбъем тома: " << total << " байт, " << "  количество свободного места: " << available << " байт\n";
		}
		else {
			hCloseVol = FindVolumeClose(hFindVol); //Окончание поиска томов
			if (hCloseVol != 0)
			{
				printf("\n");
				printf("Поиск томов окончен\n");
				break;
			}
		}
	}

	//Вывод списка программ, запускаемых при старте системы
    DWORD dwSize;
    TCHAR szName[MAX_KEY_LENGTH];
    HKEY hKey;
    DWORD dwIndex = 0;
    DWORD retCode;
    DWORD BufferSize = 8192;
    DWORD cbData;

    PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK)malloc(BufferSize);
    cbData = BufferSize;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
    {
        printf("\nRegOpenKeyEx() не удалось выполнить\n");
    }
    else printf("\nRegOpenKeyEx() успеешно выполнен\n");

    while (1) { // бесконечный цикл, будет выполняться, пока не оборвется break-ом
        dwSize = sizeof(szName);
        retCode = RegEnumValue(hKey, dwIndex, szName, &dwSize, NULL, NULL, NULL, NULL);
        if (retCode == ERROR_SUCCESS)
        {
            RegQueryValueEx(hKey, szName, NULL, NULL, (LPBYTE)PerfData, &cbData);
            printf("\n(%d) %s %s\n", dwIndex + 1, szName, PerfData);
            dwIndex++;
        }
        else break;
    }

    RegCloseKey(hKey);

    StartCounter();
    cout <<"\nИзмерение частоты ЦП в подпункте 2.1 занимает:  "<< GetCounter()*1000000<<" мкс\n";
    return 0;
}
