#include "stdafx.h"
#include "message.h"

/// <summary>Constructor.</summary>
CMsgRespCRC::CMsgRespCRC() : m_wCRC(0)
{
}

/// <summary>Parses printer response.</summary>
/// <param name="resp">Printer's response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="resp"/> is
/// NULL.</exception>
/// <exception cref="wcl::CArgumentException">If <paramref name="resp"/> is not a
/// valid response or if <paramref name="size"/> is zero.</exception>
void CMsgRespCRC::Parse(BYTE* resp, DWORD size)
{
  CWkString strTmp;

  if(resp == NULL) { WCL_THROW_ARGUMENTNULLEXCEPTION(L"resp"); }
  if(size == 0) { WCL_THROW_ARGUMENTEXCEPTION(L"size", L"cannot be zero"); }
  if(size != 7) { WCL_THROW_ARGUMENTEXCEPTION(L"size", L"incorrect message size"); }

  if(!TryParse(resp, size, &strTmp))
  {
    WCL_THROW_ARGUMENTEXCEPTION(L"resp", (const wchar_t*)strTmp);
  }
}

/// <summary>Tries to parses printer response.</summary>
/// <param name="resp">Printer's response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <param name="errMsg">Pointer to <see cref="CWkString"/> object to receive
/// error description if parse failed. If NULL, no error description is returned.</param>
/// <returns>True if parse success, false otherwise.</returns>
bool CMsgRespCRC::TryParse(BYTE* resp, DWORD size, CWkString* errMsg)
{
  DWORD i = 0, start = 0, state = 0;
  bool end = false;
  BYTE lookUp[5] = {'*', 'G', '|', '|', '*'};

  if(resp == NULL)
  {
    if(errMsg != NULL) { (*errMsg) = L"resp cannot be NULL."; }
    return false;
  }
  if(size == 0)
  {
    if(errMsg != NULL) { (*errMsg) = L"size cannot be zero."; }
    return false;
  }
  if(size != 7)
  {
    if(errMsg != NULL) { (*errMsg) = L"incorrect message size."; }
    return false;
  }

  for(i = 0;i < size;i++)
  {
    if(resp[i] == lookUp[state])
    {
      switch(state)
      {
      case 0  : // header
        if((i != 0) || (i != start))
        {
          if(errMsg != NULL) { (*errMsg) = L"missing header"; }
          return false;
        }
        break;
      case 1  : // command
        if((i != 1) || (i != start))
        {
          if(errMsg != NULL) { (*errMsg) = L"missing command"; }
          return false;
        }
        break;
      case 2  : // command delimiter
        if((i != 2) || (i != start))
        {
          if(errMsg != NULL) { (*errMsg) = L"missing command delimiter"; }
          return false;
        }
        break;
      case 3  : // CRC
        if( (i != 5) || ((i - start) != 2) )
        {
          if(errMsg != NULL) { (*errMsg) = L"missing CRC"; }
          return false;
        }
        m_wCRC = resp[start] + ( ((WORD)resp[start + 1]) << 8 );
        break;
      case 4 : // command terminator
        if( (i != start) || (i != (size - 1)) )
        {
          if(errMsg != NULL) { (*errMsg) = L"missing terminator"; }
          return false;
        }
        end = true;
        break;
      } // switch...
      start = i + 1;
      state++;
    } // if...
  } // for...

  if(!end)
  {
    if(errMsg != NULL) { (*errMsg) = L"not a valid response"; }
    return false;
  }
  return true;
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CMsgRespCRC::Dump(MSXML2::IXMLDOMElement* pElem)
{
  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_wCRC", m_wCRC);
    } // if...

  }
  catch(...) {}
}

