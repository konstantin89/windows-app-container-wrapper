#include "AppContainerTests.h"
#include <iostream>

#define LOG_MESSAGE(msg) do {std::cout << "[ ] " << msg << std::endl; } while(0)
#define LOG_ERROR(msg)   do {std::cout << "[X] " << msg << std::endl; } while(0)
#define LOG_SUCCESS(msg) do {std::cout << "[V] " << msg << std::endl; } while(0)

DWORD runAppContainer(std::string aProcToRun)
{
	AppContainerProfile lProfile;
	PROCESS_INFORMATION lContainerProcessInfo;

	auto lRetVal =
		AppContainerWrapper::CreateAppContainerProfile(L"test_name", lProfile);

	if (lRetVal != ERROR_SUCCESS)
	{
		LOG_ERROR("CreateProfile failed");
		return lRetVal;
	}

	lRetVal = AppContainerWrapper::StartProcInContainer(
		aProcToRun,
		lProfile,
		&lContainerProcessInfo);

	if (lRetVal != ERROR_SUCCESS)
	{
		LOG_ERROR("StartProcInContainer failed");
	}

	WaitForSingleObject(lContainerProcessInfo.hProcess, INFINITE);
	LOG_SUCCESS("AppContainer process terminated");

	return ERROR_SUCCESS;
}

DWORD runContainerTests()
{
	std::vector<std::wstring> lVisibleProcesses;
	auto lRetVal = AppContainerTests::ListVisibleProcessNames(lVisibleProcesses);

	if (lRetVal != ERROR_SUCCESS)
	{
		LOG_ERROR("ListVisibleProcessNames failed");
	}
	else
	{
		LOG_MESSAGE("List of visible processes");

		for (auto lProcName : lVisibleProcesses)
		{
			std::wcout << "    " << lProcName.c_str() << std::endl;
		}
	}

	AppContainerTests::EnvVarAndValueVec lEnvVarList;
	lRetVal = AppContainerTests::GetEnviormentVariables(lEnvVarList);

	if (lRetVal != ERROR_SUCCESS)
	{
		LOG_ERROR("GetEnviormentVariables failed");
	}
	else
	{
		LOG_MESSAGE("List of enviorment variables in processes");

		for (auto lEnvVarPair : lEnvVarList)
		{
			auto lVarName = lEnvVarPair.first.c_str();
			auto lVarValue = lEnvVarPair.second.c_str();

			std::wcout << "    " << lVarName << " = " << lVarValue << std::endl;
		}
	}


	return ERROR_SUCCESS;
}

int main(int argc, char* argv[])
{
	bool lIsInContainer = false;

	auto lRetVal = AppContainerWrapper::IsCurrentProcessInContainer(lIsInContainer);

	if (lRetVal != ERROR_SUCCESS)
	{
		LOG_ERROR("IsCurrentProcessInContainer failed");
	}

	if (lIsInContainer)
	{
		LOG_SUCCESS("Running in Windows AppContainer process");
		runContainerTests();
	}
	else
	{
		LOG_MESSAGE("Starting Windows AppContainer process");
		lRetVal = runAppContainer(argv[0]);
	}

	return lRetVal;
}