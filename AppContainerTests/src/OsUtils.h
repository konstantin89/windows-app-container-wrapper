#include <Windows.h>
#include <string>
#include <vector>

class OsUtils
{
public:

	static DWORD CreateFileUnderEnvVar(
		__in  std::string aEnvVar, 
		__in  std::string aFileName,
		__out std::string& aFileFullPath);

	static DWORD GetEnviormentVariable(
		__in  std::string aVarName,
		__out std::string& aVarValue);

	static DWORD ListVisibleProcessNames(
		__out std::vector<std::wstring>& aProcNamesVec);

private:
};