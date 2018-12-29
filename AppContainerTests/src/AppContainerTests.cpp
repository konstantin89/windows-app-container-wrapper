#include "AppContainerTests.h"
#include "OsUtils.h"
#include <stdio.h>

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


DWORD AppContainerTests::GetEnviormentVariables(
	EnvVarAndValueVec& aEnvVariavles)
{
	DWORD lRetVal = ERROR_SUCCESS;
	std::string lVarValue;

	for (auto lEnvVarName : mWindowsEnvVariableList)
	{
		lRetVal = OsUtils::GetEnviormentVariable(lEnvVarName ,lVarValue);

		if (lRetVal == ERROR_SUCCESS)
		{
			aEnvVariavles.push_back(
				AppContainerTests::EnvVarAndValuePair(lEnvVarName, lVarValue));
		}
	}

	return ERROR_SUCCESS;
}


DWORD AppContainerTests::VisibleProcessesTest()
{
	std::vector<std::wstring> lVisibleProcesses;
	auto lRetVal = OsUtils::ListVisibleProcessNames(lVisibleProcesses);

	if (lRetVal != ERROR_SUCCESS)
	{
		LOG_ERROR("ListVisibleProcessNames failed");
		return lRetVal;
	}
	else
	{
		LOG_MESSAGE("List of visible processes");

		for (auto lProcName : lVisibleProcesses)
		{
			printf("visible process: %ws\n", lProcName.c_str());
		}
	}

	return ERROR_SUCCESS;
}


DWORD AppContainerTests::EnviormentVariablesTest()
{
	AppContainerTests::EnvVarAndValueVec lEnvVarList;
	auto lRetVal = AppContainerTests::GetEnviormentVariables(lEnvVarList);

	if (lRetVal != ERROR_SUCCESS)
	{
		LOG_ERROR("GetEnviormentVariables failed");
		return lRetVal;
	}
	else
	{
		LOG_MESSAGE("List of enviorment variables in processes");

		for (auto lEnvVarPair : lEnvVarList)
		{
			auto lVarName = lEnvVarPair.first.c_str();
			auto lVarValue = lEnvVarPair.second.c_str();

			printf("%s = %s \n", lVarName, lVarValue);
		}
	}

	return ERROR_SUCCESS;
}

DWORD AppContainerTests::FilePermissionsTest()
{
	std::string lCreatedFileName;

	LOG_MESSAGE("Trying to create files under enviorment paths");

	for (auto lEnvVar : mWindowsEnvVariableList)
	{
		auto lRetVal = OsUtils::CreateFileUnderEnvVar(
			lEnvVar, "tempfile.txt", lCreatedFileName);

		if (lRetVal == ERROR_SUCCESS)
		{
			printf("Created file under %s: %s \n", 
				lEnvVar.c_str(), lCreatedFileName.c_str());

			DeleteFileA(lCreatedFileName.c_str());
		}
		else
		{
			printf("Failed to create file under %s \n",
				lEnvVar.c_str());

		}
	}


	

	

	return ERROR_SUCCESS;
}
