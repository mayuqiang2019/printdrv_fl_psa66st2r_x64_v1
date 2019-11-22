#include "stdafx.h"
#include "message.h"

/// <summary>Constructor.</summary>
CMsgRespStatus::CMsgRespStatus() :
  m_dwUnitAddr(0),
  m_byTemplateID(' ')
{

}

/// <summary>Parses printer response.</summary>
/// <param name="resp">Printer's response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="resp"/> is
/// NULL.</exception>
/// <exception cref="wcl::CArgumentException">If <paramref name="resp"/> is not a
/// valid response or if <paramref name="size"/> is zero.</exception>
void CMsgRespStatus::Parse(BYTE* resp, DWORD size)
{
  CWkString strTmp;
  
  if(resp == NULL) { WCL_THROW_ARGUMENTNULLEXCEPTION(L"resp"); }
  if(size == 0) { WCL_THROW_ARGUMENTEXCEPTION(L"size", L"cannot be zero"); }

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
bool CMsgRespStatus::TryParse(BYTE* resp, DWORD size, CWkString* errMsg)
{
  DWORD i, start = 0, state = 0;
  bool end = false;
  BYTE lookUp[12] = {'*', 'S', '|', '|', '|', '|', '|', '|', '|', '|', '|', '*'};
  BYTE statusFlag[5] = {0};
  std::stringstream ss;
  
  if(resp == NULL)
  {
    if(errMsg != NULL) { (*errMsg) = L"resp cannot be NULL"; }
    return false;
  }
  if(size == 0)
  {
    if(errMsg != NULL) { (*errMsg) = L"size cannot be zero"; }
    return false;
  }

  for(i = 0;i < size;i++)
  {
    if(resp[i] == lookUp[state])
    {
      switch(state)
      {
      case 0  : // command header
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
      case 3  : // unit address delimiter
        ss.str("");
        for(;start < i;start++) { ss << (char)resp[start]; }
        m_dwUnitAddr = strtoul(ss.str().c_str(), NULL, 10);
        break;
      case 4  : // software version delimiter
        ss.str("");
        for(;start < i;start++) { ss << (char)resp[start]; }
        m_strSoftwareVer = ss.str().c_str();
        break;
      case 5  : // status flag 1 delimiter
        if((i - start) != 1)
        {
          if(errMsg != NULL) { (*errMsg) = L"incorrect status flag 1"; }
          return false;
        }
        statusFlag[0] = resp[start];
        break;
      case 6  : // status flag 2 delimiter
        if((i - start) != 1)
        {
          if(errMsg != NULL) { (*errMsg) = L"incorrect status flag 2"; }
          return false;
        }
        statusFlag[1] = resp[start];
        break;
      case 7  : // status flag 3 delimiter
        if((i - start) != 1)
        {
          if(errMsg != NULL) { (*errMsg) = L"incorrect status flag 3"; }
          return false;
        }
        statusFlag[2] = resp[start];
        break;
      case 8  : // status flag 4 delimiter
        if((i - start) != 1)
        {
          if(errMsg != NULL) { (*errMsg) = L"incorrect status flag 4"; }
          return false;
        }
        statusFlag[3] = resp[start];
        break;
      case 9  : // status flag 5 delimiter
        if((i - start) != 1)
        {
          if(errMsg != NULL) { (*errMsg) = L"incorrect status flag 5"; }
          return false;
        }
        statusFlag[4] = resp[start];
        break;
      case 10 : // template number delimiter
        if(resp[start] != 'P')
        {
          if(errMsg != NULL) { (*errMsg) = L"incorrect template number"; }
          return false;
        }
        m_byTemplateID = resp[start + 1];
        break;
      case 11 : // command terminator
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
    }
  } // for...

  if(!end)
  {
    if(errMsg != NULL) { (*errMsg) = L"not a valid response"; }
    return false;
  }
  ParseStatus(statusFlag[0], statusFlag[1], statusFlag[2], statusFlag[3],
    statusFlag[4]);

  return true;
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CMsgRespStatus::Dump(MSXML2::IXMLDOMElement* pElem)
{
  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_dwUnitAddr", m_dwUnitAddr);
      wcl::CDumpHelper::DumpAttr<const wchar_t*>(pElem, L"m_strSoftwareVer",
        m_strSoftwareVer);
      wcl::CDumpHelper::DumpChild<CStatus&>(pElem, L"m_Status", m_Status);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_byTemplateID", m_byTemplateID);
    } // if...

  }
  catch(...) {}
}

/// <summary>Parses flags from response message into status.</summary>
/// <param name="flag1">Flag 1.</param>
/// <param name="flag2">Flag 2.</param>
/// <param name="flag3">Flag 3.</param>
/// <param name="flag4">Flag 4.</param>
/// <param name="flag5">Flag 5.</param>
void CMsgRespStatus::ParseStatus(BYTE flag1, BYTE flag2, BYTE flag3, BYTE flag4,
                                 BYTE flag5)
{
  m_Status.m_bBusy = ((flag1 & 0x20) != 0);
  m_Status.m_bError = ((flag1 & 0x10) != 0);
  m_Status.m_bPrintHeadOpen = ((flag1 & 0x08) != 0);
  m_Status.m_bPaperOut = ((flag1 & 0x04) != 0);
  m_Status.m_bPrintHeadErr = ((flag1 & 0x02) != 0);
  m_Status.m_bVoltageErr = ((flag1 & 0x01) != 0);

  m_Status.m_bTemperatureErr = ((flag2 & 0x20) != 0);
  m_Status.m_bLibRefErr = ((flag2 & 0x10) != 0);
  m_Status.m_bRegionDataErr = ((flag2 & 0x08) != 0);
  m_Status.m_bLoadLibErr = ((flag2 & 0x04) != 0);
  m_Status.m_bBufferOverflow = ((flag2 & 0x02) != 0);
  m_Status.m_bJobMemOverflow = ((flag2 & 0x01) != 0);

  m_Status.m_bCmdErr = ((flag3 & 0x20) != 0);
  m_Status.m_bNoFont = ((flag3 & 0x10) != 0);
  m_Status.m_bPaperInChute = ((flag3 & 0x08) != 0);
  m_Status.m_bFlashErr = ((flag3 & 0x04) != 0);
  m_Status.m_bOffLine = ((flag3 & 0x02) != 0);
  m_Status.m_bWrongPaper = ((flag3 & 0x01) != 0);

  m_Status.m_bJournalMode = ((flag4 & 0x08) != 0);
  m_Status.m_bCutterErr = ((flag4 & 0x04) != 0);
  m_Status.m_bPaperJam = ((flag4 & 0x02) != 0);
  m_Status.m_bPaperLow = ((flag4 & 0x01) != 0);

  m_Status.m_bLastBarPrinted = ((flag5 & 0x20) != 0);
  m_Status.m_bTopOfForm = ((flag5 & 0x10) != 0);
  m_Status.m_bReadyToRx = ((flag5 & 0x08) == 0);
  m_Status.m_bDoorOpened = ((flag5 & 0x04) != 0);
  m_Status.m_bPowerUpReset = ((flag5 & 0x01) != 0);
}
