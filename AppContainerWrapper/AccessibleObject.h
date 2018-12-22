#ifndef __ACCESSIBLE_OBJECT_H
#define __ACCESSIBLE_OBJECT_H

#include "pch.h"
#include <string>
#include <accctrl.h>

/**
* @brief: Struct that represents object that can be accessed from app container.
*         For example: File or Registry.
*/
struct AccessibleObject
{
	std::string name;
	SE_OBJECT_TYPE type;
	DWORD accessMask;

	LPSTR getNameAsLpstr()
	{
		return const_cast<char *>(name.c_str());
	}
};

#endif
