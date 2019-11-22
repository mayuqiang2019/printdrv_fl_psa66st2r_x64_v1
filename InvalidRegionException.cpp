#include "stdafx.h"
#include "drvException.h"

/// <summmary>Constructor.</summary>
CInvalidRegionException::CInvalidRegionException()
{
}

/// <summmary>Constructor.</summary>
/// <param name="message">Exception descriptions, NULL is treated as emtpy string.</param>
CInvalidRegionException::CInvalidRegionException(const wchar_t* message) :
  wcl::CSelfDocException(message)
{
}

/// <summmary>Constructor.</summary>
/// <param name="message">Exception descriptions, NULL is treated as emtpy string.</param>
/// <param name="message">Thrower descriptions, NULL is treated as emtpy string.</param>
CInvalidRegionException::CInvalidRegionException(const wchar_t* message,
                                                 const wchar_t* thrower) :
  wcl::CSelfDocException(message, thrower)
{
}

/// <summmary>Copy constructor.</summary>
/// <param name="src">Reference to <see cref="CInvalidREgionException"/> object
/// to be copied.</param>
CInvalidRegionException::CInvalidRegionException(const CInvalidRegionException& src) :
  wcl::CSelfDocException(src)
{
}

/// <summary>Formats exception details into string.</summary>
/// <param name="src">Reference to CWkString object to receive formatted string.</param>
void CInvalidRegionException::ToString(CWkString& str) const
{
  CWkString tmp;

  AttrToString(tmp);
  str = L"[CInvalidRegionException] ";
  str += tmp;
}
