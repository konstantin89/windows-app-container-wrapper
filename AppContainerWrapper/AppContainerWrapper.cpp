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

DWORD AppContainerWrapper::createProfile(
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

DWORD AppContainerWrapper::GrantAccessToObjects(
	AppContainerProfile& aProfile,
	std::vector<AccessibleObject>& aAllowedObjects)
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

DWORD AppContainerWrapper::grantAccessToObject(
	PSID aAppContainerSid,
	AccessibleObject aObjectToGrantAccessTo)
{
	EXPLICIT_ACCESS_A lExplicitAccess;
	PACL original_acl = NULL;
	PACL new_acl = NULL;
	DWORD lRetValue = 0;

	do
	{
		lExplicitAccess.grfAccessMode = GRANT_ACCESS;
		lExplicitAccess.grfAccessPermissions = aObjectToGrantAccessTo.accessMask;
		lExplicitAccess.grfInheritance = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;

		lExplicitAccess.Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
		lExplicitAccess.Trustee.pMultipleTrustee = NULL;
		lExplicitAccess.Trustee.ptstrName = (CHAR *)aAppContainerSid;
		lExplicitAccess.Trustee.TrusteeForm = TRUSTEE_IS_SID;
		lExplicitAccess.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;

		lRetValue = GetNamedSecurityInfoA(
			aObjectToGrantAccessTo.name.c_str(), 
			aObjectToGrantAccessTo.type, 
			DACL_SECURITY_INFORMATION,
			NULL, NULL, &original_acl, NULL, NULL);

		if (lRetValue != ERROR_SUCCESS)
		{
			break;
		}

		lRetValue = SetEntriesInAclA(1, &lExplicitAccess, original_acl, &new_acl);
		if (lRetValue != ERROR_SUCCESS)
		{
			break;
		}

		lRetValue = SetNamedSecurityInfoA(
			aObjectToGrantAccessTo.name.data, 
			aObjectToGrantAccessTo.type, 
			DACL_SECURITY_INFORMATION, 
			NULL, NULL, new_acl, NULL);

	} 
	while (false);

	if (original_acl)
	{
		LocalFree(original_acl);
	}
		
	if (new_acl)
	{
		LocalFree(new_acl);
	}
		
	if (lRetValue != ERROR_SUCCESS)
	{
		return GetLastError();
	}

	return ERROR_SUCCESS;
}