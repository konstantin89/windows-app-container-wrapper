#include "OsUtils.h"
#include <TlHelp32.h>

#pragma comment(lib, "Iphlpapi.lib")

DWORD OsUtils::GetEnviormentVariable(
	std::string aVarName,
	std::string& aVarValue)
{
	char lVariableValue[MAX_PATH];

	auto lRetVal = ExpandEnvironmentStringsA(
		aVarName.c_str(), lVariableValue, MAX_PATH - 1);

	if (!lRetVal)
	{
		return GetLastError();
	}

	aVarValue.assign(lVariableValue, lRetVal-1);

	return ERROR_SUCCESS;
}

DWORD OsUtils::CreateFileUnderEnvVar(
	std::string aEnvVar,
	std::string aFileName,
	std::string& aFileFullPath)
{
	std::string lEnvVarValue;
	
	auto lRetVal = GetEnviormentVariable(aEnvVar, lEnvVarValue);
	if (lRetVal != ERROR_SUCCESS)
	{
		return GetLastError();
	}

	std::string lFileFullPath = lEnvVarValue + "\\" + aFileName;

	HANDLE hFile = CreateFileA(lFileFullPath.c_str(), 
		GENERIC_WRITE, 
		0,             
		0,             
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		0);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	CloseHandle(hFile);
	aFileFullPath.assign(lFileFullPath);
	return ERROR_SUCCESS;
}

DWORD OsUtils::ListVisibleProcessNames(
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
	} 
	while (Process32Next(hSnapshot, &lProcEntry));

	CloseHandle(hSnapshot);

	return ERROR_SUCCESS;
}
