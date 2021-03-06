#include "AppContainerWrapper.h"

#include <strsafe.h>
#include <Sddl.h>
#include <Userenv.h>
#include <AccCtrl.h>
#include <Aclapi.h>
#include <winerror.h>

#pragma comment(lib, "Userenv.lib")

DWORD AppContainerWrapper::CreateAppContainerProfile(
	std::wstring aProfileName,
	AppContainerProfile& aProfile)
{
	std::vector<SID_AND_ATTRIBUTES> lEmptyCababilitiesVec;

	return createAppContainerProfile(
		aProfileName, 
		aProfileName, 
		aProfileName,
		lEmptyCababilitiesVec, 
		aProfile);
}

DWORD AppContainerWrapper::createAppContainerProfile(
	std::wstring aProfileName,
	std::wstring aDisplayName,
	std::wstring aDescription,
	std::vector<SID_AND_ATTRIBUTES> aCapabilities,
	AppContainerProfile& aProfile)
{
	PSID lSid = NULL;
	DWORD lRetVal = ERROR_SUCCESS;

	WCHAR* lName = const_cast<WCHAR*>(aProfileName.data());

	if (aCapabilities.size())
	{
		lRetVal = ::CreateAppContainerProfile(
			lName,
			lName,
			lName,
			&aCapabilities.front(),
			(DWORD)aCapabilities.size(),
			&lSid);
	}

	else
	{
		lRetVal = ::CreateAppContainerProfile(
			lName,
			lName,
			lName,
			NULL,
			0,
			&lSid);
	}

	lRetVal = HRESULT_CODE(lRetVal);

	if (lRetVal != S_OK && lRetVal != ERROR_ALREADY_EXISTS)
	{
		return lRetVal;
	}

	else if (lRetVal == ERROR_ALREADY_EXISTS)
	{
		openExistingAppContainerProfile(aProfileName, aProfile);
	}
	
	else
	{
		aProfile.mName = aProfileName;
		aProfile.mDescription = aDescription;
		aProfile.mSid = lSid;
		aProfile.mCapabilitiesVec = aCapabilities;
	}

	return S_OK;
}


DWORD AppContainerWrapper::DeleteContainerProfile(std::wstring aProfileName)
{
	return DeleteAppContainerProfile(aProfileName.c_str());
}

DWORD AppContainerWrapper::DeleteContainerProfile(AppContainerProfile& aProfileName)
{
	return DeleteContainerProfile(aProfileName.mName);
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
	for (auto lAllowedObj : aAllowedObjects)
	{
		auto lRetVal = grantAccessToObject(aProfile.mSid, lAllowedObj);
		if (lRetVal != ERROR_SUCCESS)
		{
			return GetLastError();
		}
	}

	return ERROR_SUCCESS;
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
			aObjectToGrantAccessTo.getNameAsLpstr(),
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

DWORD AppContainerWrapper::StartProcInContainer(
	std::string aPeFilePath,
	AppContainerProfile& aProfile,
	LPPROCESS_INFORMATION aProcessInfo)
{
	return startProcInContainer(aPeFilePath, aProfile, 0,aProcessInfo);
}


DWORD AppContainerWrapper::startProcInContainer(
	std::string aPeFilePath,
	AppContainerProfile& aProfile,
	int aProcCreationParams,
	LPPROCESS_INFORMATION aProcessInfo)
{
	STARTUPINFOEXA lStartupInfo = { 0 };
	SECURITY_CAPABILITIES lSecurityCapabilities = { 0 };
	SIZE_T lThreadListSize = 0;
	DWORD lErrorCode = ERROR_SUCCESS;

	do // Not a loop
	{
		InitializeProcThreadAttributeList(NULL, 1, NULL, &lThreadListSize);

		lStartupInfo.lpAttributeList = 
			(LPPROC_THREAD_ATTRIBUTE_LIST)malloc(lThreadListSize);

		auto lRetVal = InitializeProcThreadAttributeList(
			lStartupInfo.lpAttributeList, 
			1,
			NULL, 
			&lThreadListSize);

		if (!lRetVal)
		{
			lErrorCode = GetLastError();
			break;
		}

		ZeroMemory(&lSecurityCapabilities, sizeof(SECURITY_CAPABILITIES));
		lSecurityCapabilities.AppContainerSid = aProfile.mSid;
		lSecurityCapabilities.Capabilities = NULL;
		lSecurityCapabilities.CapabilityCount = 0;

		lRetVal = UpdateProcThreadAttribute(
			lStartupInfo.lpAttributeList,
			0,
			PROC_THREAD_ATTRIBUTE_SECURITY_CAPABILITIES,
			&lSecurityCapabilities,
			sizeof(lSecurityCapabilities),
			NULL, NULL);

		if (!lRetVal)
		{
			lErrorCode = GetLastError();
			break;
		}

		CHAR* lPeFileChar 
			= const_cast<CHAR*>(aPeFilePath.data());

		lRetVal = CreateProcessA(
			lPeFileChar,
			NULL,NULL,NULL, 
			FALSE, 
			aProcCreationParams | EXTENDED_STARTUPINFO_PRESENT, 
			NULL, NULL,
			(LPSTARTUPINFOA)&lStartupInfo, 
			aProcessInfo);

		if(!lRetVal)
		{
			lErrorCode = GetLastError();
			break;
		}

	} 
	while (0);

	if (lStartupInfo.lpAttributeList)
	{
		DeleteProcThreadAttributeList(lStartupInfo.lpAttributeList);
	}

	return lErrorCode;
}

DWORD AppContainerWrapper::openExistingAppContainerProfile(
	__in std::wstring aProfileName,
	__out AppContainerProfile& aProfile)
{
	WCHAR* lContainerNameWchar = const_cast<WCHAR*>(aProfileName.data());

	PSID lSid = NULL;

	auto lRetVal = DeriveAppContainerSidFromAppContainerName(
		lContainerNameWchar, &lSid);

	if (!SUCCEEDED(lRetVal))
	{
		return HRESULT_CODE(lRetVal);
	}

	aProfile.mName = aProfileName;
	aProfile.mSid = lSid;

	return ERROR_SUCCESS;
}