#include "pch.h"
#include "AppContainerWrapper.h"
#include <iostream>

void logError(DWORD aErrCode, std::string aMethodName)
{
	std::cout << "[X] " << aMethodName.c_str()
		<< " failed with error code " << aErrCode 
		<< std::endl;
}

void logSuccess(std::string aMethodName)
{
	std::cout << "[V] " << aMethodName.c_str()
		<< " succeed" << std::endl;
}

DWORD runAppContainerExample(std::string aProcToRun)
{
	AppContainerProfile lProfile;
	PROCESS_INFORMATION lContainerProcessInfo;
	auto lRetVal = 
		AppContainerWrapper::CreateAppContainerProfile(L"test_name", lProfile);

	if (lRetVal != ERROR_SUCCESS)
	{
		logError(lRetVal, "CreateProfile");
		return lRetVal;
	}

	lRetVal = AppContainerWrapper::StartProcInContainer(
		aProcToRun,
		lProfile,
		&lContainerProcessInfo);

	if (lRetVal != ERROR_SUCCESS)
	{
		logError(lRetVal, "StartProcInContainer");
	}

	return lRetVal;
}

int main(int argc, char* argv[])
{
	auto lRetVal = runAppContainerExample(argv[0]);

	return lRetVal;
}