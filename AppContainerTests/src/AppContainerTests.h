#include "AppContainerWrapper.h"

#include <iostream>
#include <utility>

#define LOG_MESSAGE(msg) do {std::cout << "[ ] " << msg << std::endl; } while(0)
#define LOG_ERROR(msg)   do {std::cout << "[X] " << msg << std::endl; } while(0)
#define LOG_SUCCESS(msg) do {std::cout << "[V] " << msg << std::endl; } while(0)

class AppContainerTests
{
public:

	using EnvVarAndValuePair = std::pair<std::string, std::string>;
	using EnvVarAndValueVec = std::vector<EnvVarAndValuePair>;

	/**
	* @brief: Get the values of Windows enviorment variables.
	*/
	static DWORD GetEnviormentVariables(
		__out EnvVarAndValueVec& aEnvVariavles);

public: 

	static DWORD FilePermissionsTest();

	static DWORD VisibleProcessesTest();

	static DWORD EnviormentVariablesTest();



private:

	static const std::vector<std::string> mWindowsEnvVariableList;

};