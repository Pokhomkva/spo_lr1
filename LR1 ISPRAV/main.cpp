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


void Counter()
{
    unsigned long freq = 0;
    LARGE_INTEGER t0;
    LARGE_INTEGER t;
    __int64 CounterStart = 0;

    QueryPerformanceCounter(&t0);
    CounterStart = t0.QuadPart;
    if (QueryPerformanceFrequency(&t)== 0)
        cout << "Function QueryPerformanceFrequency() failed!\n";
    freq = long(t.QuadPart);
    cout << "\nCPU frequency: " << freq << " Hz\n";

    if (QueryPerformanceCounter(&t)== 0)
    {
        cout << "Function QueryPerformanceCounter() failed!\n";
    }
    else
    {
        double result = (1000000 * (t.QuadPart - CounterStart)) / freq;
        cout << "Number of cpu cycles: " << result  << " us \n";
    }
}


/*void StartCounter()
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
}*/

int main()
{
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

    printf("VERSION OS:  %d.%d.%d\n",
           dwMajorVersion,
           dwMinorVersion,
           dwBuild);

    //Определение системного каталога
    GetSystemDirectory(infoBuf, INFO_BUFFER_SIZE);
    printf("\nSystem Directory: %s\n", infoBuf);

    //Определение имени пользователя
    GetUserName(infoBuf, &bufCharCount);
    printf("\nUserName:  %s\n", infoBuf);

    //Определение названия компьютера
    bufCharCount = INFO_BUFFER_SIZE;
    GetComputerName(infoBuf, &bufCharCount);
    printf("\nComputerName: %s\n", infoBuf);

    //Для томов
    TCHAR buffer[MAX_PATH];
    TCHAR name[MAX_PATH];
    DWORD  BufferLength = MAX_PATH;
    ULARGE_INTEGER freespac;
    ULARGE_INTEGER vol;

    HANDLE search = FindFirstVolume(buffer, sizeof(buffer));
    do
    {
        GetVolumePathNamesForVolumeName(name, buffer, BufferLength, &BufferLength);
        printf("\n\n%s", name);
        printf("\n%s", buffer);
        if (GetDiskFreeSpaceEx(name, &freespac, &vol, NULL)!= 0)
        {
            cout << "\nVolume: " << vol.QuadPart << "\nFree space: " << freespac.QuadPart;
        }
        else
        {
            printf("\nNo information.");
        }
    }
    while (FindNextVolume(search, name, sizeof(buffer)));
    FindVolumeClose(search);



    //Вывод списка программ, запускаемых при старте системы
    DWORD dwSize;
    TCHAR szName[MAX_KEY_LENGTH];
    HKEY hKey;
    DWORD dwIndex = 0;
    DWORD retCode;
    DWORD BufferSize = 8192;
    DWORD cbData;

    TCHAR PerfData[INFO_BUFFER_SIZE];
    cbData = BufferSize;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
    {
        printf("\nRegOpenKeyEx() FALSE\n");
    }
    else printf("\nRegOpenKeyEx() OK\n");

    while (1)   // бесконечный цикл, будет выполняться, пока не оборвется break-ом
    {
        dwSize = sizeof(szName);
        retCode = RegEnumValue(hKey, dwIndex, szName, &dwSize, NULL, NULL, NULL, NULL);
        if (retCode == ERROR_SUCCESS)
        {
            RegQueryValueEx(hKey, szName, NULL, NULL,LPBYTE(PerfData), &cbData);
            printf("\n(%d) %s %s\n", dwIndex + 1, szName, PerfData);
            dwIndex++;
        }
        else break;
    }

    RegCloseKey(hKey);

    Counter();
    // <<"\nИзмерение частоты ЦП в подпункте 2.1 занимает:  "<< Counter()*1000000<<" мкс\n";
    return 0;
}
