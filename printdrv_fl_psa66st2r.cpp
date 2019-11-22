// printdrv_fl_psa66st2r.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "printdrv_fl_psa66st2r.h"

#include "printer.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

/// <summary>Retrieves version of interface implemented.</summary>
/// <param name="version">Pointer to 4 WORD array to hold version numbers.
/// Version numbers are returned in little-endian.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="version"/>
/// is NULL.</exception>
void PrintGetInterfaceVer(WORD* version)
{
  if(version == NULL) { WCL_THROW_ARGUMENTNULLEXCEPTION(L"version"); }
  version[3] = 1;
  version[2] = 0;
  version[1] = 0;
  version[0] = 6;
}

/// <summary>Initializes driver.</summary>
/// <returns>True if driver initialized successfully, false otherwise.</returns>
/// <exception cref="wcl::CInvalidOperationException">If no driver is loaded.</exception>
bool PrintInit()
{
  return true;
}

/// <summary>Free driver resources.</summary>
void PrintUnInit()
{
}

/// <summary>Creates an instance of <see cref="IPrinter"/>.</summary>
/// <returns>Address of a new instance of <see cref="IPrinter"/>, NULL if failed.</returns>
print::IPrinter* PrintCreateInstance()
{
  CPrinter *pPrinter = new CPrinter();

  if(pPrinter == NULL) { throw wcl::COutOfMemoryException(); }

  return pPrinter;
}

/// <summary>Destroys an instance of <see cref="IPrinter"/>.</summary>
/// <param name="pPrinter">Pointer to <see cref="IPrinter"/> object to be destroyed,
/// cannot be NULL.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pPrinter"/> is
/// NULL.</exception>
void PrintReleaseInstance(print::IPrinter* pPrinter)
{
  CPrinter *pObj = (CPrinter*)pPrinter;
  
  if(pPrinter == NULL) { WCL_THROW_ARGUMENTNULLEXCEPTION(L"pPrinter"); }

  delete pObj;
}
