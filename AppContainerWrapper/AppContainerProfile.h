#ifndef __APP_CONTAINER_PROFILE_H
#define __APP_CONTAINER_PROFILE_H

#include "pch.h"

struct AppContainerProfile
{
	AppContainerProfile();
	~AppContainerProfile();

	std::wstring mName;
	std::wstring mDescription;
	PSID mSid;
	std::vector<SID_AND_ATTRIBUTES> mCapabilitiesVec;
};

#endif