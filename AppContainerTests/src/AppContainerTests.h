#include "AppContainerWrapper.h"

#include <utility>

class AppContainerTests
{
public:

	using EnvVarAndValuePair = std::pair<std::string, std::string>;
	using EnvVarAndValueVec = std::vector<EnvVarAndValuePair>;

	/**
	* @brief: List all visible processes.
	*/
	static DWORD ListVisibleProcessNames(
		__out std::vector<std::wstring>& aProcNamesVec);

	static DWORD GetEnviormentVariables(
		__out EnvVarAndValueVec& aEnvVariavles);

	static DWORD GetEnviormentVariable(
		__in  std::string aVarName,
		__out std::string& aVarValue);

private:

	static const std::vector<std::string> mWindowsEnvVariableList;

};