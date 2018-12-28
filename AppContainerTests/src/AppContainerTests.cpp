#include "AppContainerTests.h"

#include <TlHelp32.h>

#pragma comment(lib, "Iphlpapi.lib")

 const std::vector<std::string> AppContainerTests::mWindowsEnvVariableList =
{
	"%ALLUSERSPROFILE%",
	"%APPDATA%",
	"%COMMONPROGRAMFILES%",
	"%COMMONPROGRAMFILES(x86)%",
	"%COMSPEC%",
	"%HOMEDRIVE%",
	"%HOMEPATH%",
	"%LOCALAPPDATA%",
	"%PROGRAMDATA%",
	"%PROGRAMFILES%",
	"%PROGRAMFILES(X86)%",
	"%PUBLIC%",
	"%SystemDrive%",
	"%SystemRoot%",
	"%TEMP% ",
	"%USERPROFILE%",
	"%WINDIR%"
};

DWORD AppContainerTests::ListVisibleProcessNames(
	std::vector<std::wstring>& aProcNamesVec)
{
	tagPROCESSENTRY32W lProcEntry;
	HANDLE hSnapshot;

	lProcEntry.dwSize = sizeof(lProcEntry);

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnapshot)
	{
		return GetLastError();
	}

	if (!Process32First(hSnapshot, &lProcEntry))
	{
		CloseHandle(hSnapshot);
		return GetLastError();
	}
	
	do
	{
		aProcNamesVec.push_back(lProcEntry.szExeFile);
	} while (Process32Next(hSnapshot, &lProcEntry));
	
	CloseHandle(hSnapshot);
	
	return ERROR_SUCCESS;
}

DWORD AppContainerTests::GetEnviormentVariable(
	std::string aVarName,
	std::string& aVarValue)
{
	char lVariableValue[MAX_PATH];

	auto lRetVal = ExpandEnvironmentStringsA(aVarName.c_str(), lVariableValue, MAX_PATH - 1);

	if (!lRetVal)
	{
		return GetLastError();
	}

	aVarValue.assign(lVariableValue, lRetVal);

	return ERROR_SUCCESS;
}

DWORD AppContainerTests::GetEnviormentVariables(
	EnvVarAndValueVec& aEnvVariavles)
{
	DWORD lRetVal = ERROR_SUCCESS;
	std::string lVarValue;

	for (auto lEnvVarName : mWindowsEnvVariableList)
	{
		lRetVal = GetEnviormentVariable(lEnvVarName ,lVarValue);

		if (lRetVal == ERROR_SUCCESS)
		{
			aEnvVariavles.push_back(
				AppContainerTests::EnvVarAndValuePair(lEnvVarName, lVarValue));
		}
	}

	return ERROR_SUCCESS;
}

