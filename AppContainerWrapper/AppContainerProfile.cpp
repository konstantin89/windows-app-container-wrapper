#include "pch.h"
#include "AppContainerProfile.h"

AppContainerProfile::AppContainerProfile()
{
	mSid = NULL;
}

AppContainerProfile::~AppContainerProfile()
{
	if (mSid)
	{
		delete mSid;
		mSid = NULL;
	}
}
