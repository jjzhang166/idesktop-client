
#include <list>
#define MAX_PATH 260
struct MY_APP_STRUCT
{
	char strAppName[MAX_PATH];
	char strVer[MAX_PATH];
	char strExePath[MAX_PATH];
	char strParam[MAX_PATH];
	char strIconPath[MAX_PATH];
};

typedef std::list<MY_APP_STRUCT>		MY_APP_StructList;
typedef MY_APP_StructList::iterator			MY_APP_StructIter;
typedef MY_APP_StructList::const_iterator		MY_APP_StructCIter;
