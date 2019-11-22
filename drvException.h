#pragma once

#include "wclException.h"

#define THROW_INVALIDREGIONEXCEPTION(msg)\
  throw CInvalidRegionException(msg, WCL_WFUNCSIG)

/// <summary>Invalid print region.</summary>
class CInvalidRegionException : public wcl::CSelfDocException
{
public:
  CInvalidRegionException();
  CInvalidRegionException(const wchar_t* message);
  CInvalidRegionException(const wchar_t* message, const wchar_t* thrower);
  CInvalidRegionException(const CInvalidRegionException& src);

public:
  virtual void ToString(CWkString& str) const;
};
