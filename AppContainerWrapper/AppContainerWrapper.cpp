#include "pch.h"
#include "AppContainerWrapper.h"

#include <strsafe.h>
#include <Sddl.h>
#include <Userenv.h>
#include <AccCtrl.h>
#include <Aclapi.h>

#pragma comment(lib, "Userenv.lib")

#define EMPTY_DESCRIPTION_WSTR L""

DWORD AppContainerWrapper::CreateProfile(
	std::wstring aProfileName,
	AppContainerProfile& aProfile)
{
	std::vector<SID_AND_ATTRIBUTES> lEmptyCababilitiesVec;

	return createProfile(
		aProfileName, 
		aProfileName, 
		EMPTY_DESCRIPTION_WSTR,
		lEmptyCababilitiesVec, 
		aProfile);
}

static DWORD createProfile(
	std::wstring aProfileName,
	std::wstring aDisplayName,
	std::wstring aDescription,
	std::vector<SID_AND_ATTRIBUTES> aCapabilities,
	AppContainerProfile& aProfile)
{
	PSID lSid = NULL;

	auto lRetVal = ::CreateAppContainerProfile(
		aProfileName.c_str(),
		aDisplayName.c_str(),
		aDescription.c_str(),
		&aCapabilities.front(),
		aCapabilities.size(),
		&lSid);

	if (lRetVal != S_OK)
	{
		return lRetVal;
	}

	aProfile.mName = aProfileName;
	aProfile.mDescription = aDescription;
	aProfile.mSid = lSid;
	aProfile.mCapabilitiesVec = aCapabilities;

	return S_OK;
}


DWORD AppContainerWrapper::DeleteAppContainerProfile(std::wstring aProfileName)
{
	return DeleteAppContainerProfile(aProfileName.c_str());
}

DWORD AppContainerWrapper::DeleteAppContainerProfile(AppContainerProfile& aProfileName)
{
	return DeleteAppContainerProfile(aProfileName.mName);
}

DWORD AppContainerWrapper::IsCurrentProcessInContainer(bool& aIsRunningInContainer)
{
	HANDLE lProcToken;
	BOOL lIsContainer = 0;
	DWORD lRetVal = 0;
	DWORD lRetLen;

	lRetVal = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &lProcToken);

	if (!lRetVal)
	{
		return GetLastError();
	}

	lRetVal = GetTokenInformation(
		lProcToken, 
		TokenIsAppContainer, 
		&lIsContainer, 
		sizeof(lIsContainer), 
		&lRetLen);

	if (!lRetVal)
	{
		CloseHandle(lProcToken);
		return GetLastError();
	}

	aIsRunningInContainer = lIsContainer;

	CloseHandle(lProcToken);
	return ERROR_SUCCESS;
}

DWORD AppContainerWrapper::GrantAccessToFiles(
	AppContainerProfile& aProfile,
	std::vector<std::wstring>& aAllowedFiles)
{

}

DWORD AppContainerWrapper::startProcInContainer(
	std::string aPeFilePath,
	AppContainerProfile& aProfile,
	std::vector<std::wstring>& aFilesWithGrantedAccess,
	int aProcCreationParams,
	LPPROCESS_INFORMATION aProcessInfo)
{

}