#include "pch.h"
#include "AppContainerWrapper.h"
#include <iostream>

#define LOG_MESSAGE(msg) do {std::cout << "[ ] " << msg << std::endl; } while(0)
#define LOG_ERROR(msg)   do {std::cout << "[X] " << msg << std::endl; } while(0)
#define LOG_SUCCESS(msg) do {std::cout << "[V] " << msg << std::endl; } while(0)

DWORD runAppContainerExample(std::string aProcToRun)
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

	return 0;
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
	}
	else
	{
		LOG_MESSAGE("Starting Windows AppContainer process");
		lRetVal = runAppContainerExample(argv[0]);
	}

	return lRetVal;
}