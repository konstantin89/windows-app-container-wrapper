#include "AppContainerProfile.h"


AppContainerProfile::AppContainerProfile()
{
	mSid = NULL;
}

AppContainerProfile::~AppContainerProfile()
{
	if (mSid)
	{
		FreeSid(mSid);
		mSid = NULL;
	}
}
