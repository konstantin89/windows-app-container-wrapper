#include "AppContainerTests.h"
#include <iostream>


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
	
	AppContainerTests::VisibleProcessesTest();

	AppContainerTests::EnviormentVariablesTest();

	AppContainerTests::FilePermissionsTest();


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