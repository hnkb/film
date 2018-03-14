#pragma once

#include "targetver.h"

#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <functional>

#include <windows.h>
#include <strsafe.h>
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")




//#include "sqlce/sqlce_err.h"
#include "sqlce/sqlce_oledb.h"
//#include "sqlce/sqlce_sync.h"

#include <atlbase.h>
#include <atldbcli.h>


#undef min
#undef max
