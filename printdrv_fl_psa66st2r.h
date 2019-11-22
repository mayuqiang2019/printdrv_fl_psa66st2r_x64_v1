#pragma once
#pragma warning (disable : 4786)
#pragma warning (disable : 4231)

#include "printPrinter.h"

void PrintGetInterfaceVer(WORD* version);
bool PrintInit();
void PrintUnInit();
print::IPrinter* PrintCreateInstance();
void PrintReleaseInstance(print::IPrinter* pPrinter);
