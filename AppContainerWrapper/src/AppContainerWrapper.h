#ifndef __APP_CONTAINER_WRAPPER_H
#define __APP_CONTAINER_WRAPPER_H

#include <Windows.h>
#include <string>
#include <vector>

#include "AppContainerProfile.h"
#include "AccessibleObject.h"

class AppContainerWrapper
{
public:
	static DWORD CreateAppContainerProfile(
		__in std::wstring aProfileName, 
		__out AppContainerProfile& aProfile);

	static DWORD DeleteContainerProfile(
		__in AppContainerProfile& aProfileName);

	static DWORD DeleteContainerProfile(
		__in std::wstring aProfileName);

	static DWORD IsCurrentProcessInContainer(
		__out bool& aIsRunningInContainer);

	static DWORD StartProcInContainer(
		__in std::string aPeFilePath, 
		__in AppContainerProfile& aProfile,
		__out LPPROCESS_INFORMATION aProcessInfo);

	static DWORD GrantAccessToObjects(
		__in AppContainerProfile& aProfile,
		__in std::vector<AccessibleObject>& aAllowedObjects);

private:
	static DWORD createAppContainerProfile(
		__in std::wstring aProfileName,
		__in std::wstring aDisplayName,
		__in std::wstring aDescription,
		__in std::vector<SID_AND_ATTRIBUTES> aCapabilities,
		__out AppContainerProfile& aProfile);

	static DWORD openExistingAppContainerProfile(
		__in std::wstring aProfileName,
		__out AppContainerProfile& aProfile);

	static DWORD startProcInContainer(
		__in std::string aPeFilePath,
		__in AppContainerProfile& aProfile,
		__in int aProcCreationParams,
		__out LPPROCESS_INFORMATION aProcessInfo);

	static DWORD grantAccessToObject(
		__in PSID aAppContainerSid,
		__in AccessibleObject aObjectToGrantAccessTo);
};


#endif