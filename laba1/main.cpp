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
	//����� ������� ������� f ��
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency))
		cout << "\nQueryPerformanceFrequency �� ������� ���������!\n";

	PCFreq = double(frequency.QuadPart);
	printf("\n������� ������� ��, Hz: %u\n", frequency);
	QueryPerformanceCounter(&frequency);
	CounterStart = frequency.QuadPart;
}

//������� ���������� ������ ��
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

	// ����������� ������ Windows
	dwVersion = GetVersion();
	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// ��������� ������ ������
	dwBuild = (DWORD)(HIWORD(dwVersion));

	printf("������ ��:  %d.%d.%d\n",
		dwMajorVersion,
		dwMinorVersion,
		dwBuild);

	//����������� ���������� ��������
	GetSystemDirectory(infoBuf, INFO_BUFFER_SIZE);
	printf("\n��������� �������: %s\n", infoBuf);

	//����������� ����� ������������
	GetUserName(infoBuf, &bufCharCount);
	printf("\n��� ������������:  %s\n", infoBuf);

	//����������� �������� ����������
	bufCharCount = INFO_BUFFER_SIZE;
	GetComputerName(infoBuf, &bufCharCount);
	printf("\n��� ����������: %s\n", infoBuf);

	//��� �����
    DWORD  CharCount = MAX_PATH;
    char Names[MAX_PATH];
    CHAR VolumeName[MAX_PATH];

	//���, ����, ����� � ���-�� ���������� ����� ��� ������� ����
    hFindVol = FindFirstVolume(VolumeName, BUFSIZE);
    printf("\n��� ������� ����: ");
    printf(VolumeName);
	__int64 total, available, free;
    GetVolumePathNamesForVolumeName(VolumeName, Names, CharCount, &CharCount);
    printf("\n������ ���� � ����: ");
    printf(Names);
    GetDiskFreeSpaceEx((LPCSTR)Names, (PULARGE_INTEGER)&available, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free);
	cout << "\n����� ����: " << total << " ����, " << "  ���������� ���������� �����: " << available << " ����\n";

	//���, ����, ����� � ���-�� ���������� ����� ��� ������������ ����
	while (1) {                   // ����������� ����, ����� �����������, ���� �� ��������� break-��
		hNextVol = FindNextVolume(hFindVol, VolumeName, BUFSIZE);
		if (GetLastError() != 18) // ��� ������ "������ ��� ������"
		{
			printf("\n��������� ��� ����: ");
			printf(VolumeName);
			GetVolumePathNamesForVolumeName(VolumeName, Names, CharCount, &CharCount);
			printf("\n������ ���� � ����: ");
			printf(Names);
			GetDiskFreeSpaceEx((LPCSTR)Names, (PULARGE_INTEGER)&available, (PULARGE_INTEGER)&total, (PULARGE_INTEGER)&free);
			cout << "\n����� ����: " << total << " ����, " << "  ���������� ���������� �����: " << available << " ����\n";
		}
		else {
			hCloseVol = FindVolumeClose(hFindVol); //��������� ������ �����
			if (hCloseVol != 0)
			{
				printf("\n");
				printf("����� ����� �������\n");
				break;
			}
		}
	}

	//����� ������ ��������, ����������� ��� ������ �������
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
        printf("\nRegOpenKeyEx() �� ������� ���������\n");
    }
    else printf("\nRegOpenKeyEx() �������� ��������\n");

    while (1) { // ����������� ����, ����� �����������, ���� �� ��������� break-��
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
    cout <<"\n��������� ������� �� � ��������� 2.1 ��������:  "<< GetCounter()*1000000<<" ���\n";
    return 0;
}
