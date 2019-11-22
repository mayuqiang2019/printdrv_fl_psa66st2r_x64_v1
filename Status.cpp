#include "stdafx.h"
#include "message.h"

/// <summary>Constructor.</summary>
CStatus::CStatus() :
  m_bBusy(false),
  m_bError(false),
  m_bPrintHeadOpen(false),
  m_bPaperOut(false),
  m_bPrintHeadErr(false),
  m_bVoltageErr(false),
  m_bTemperatureErr(false),
  m_bLibRefErr(false),
  m_bRegionDataErr(false),
  m_bLoadLibErr(false),
  m_bBufferOverflow(false),
  m_bJobMemOverflow(false),
  m_bCmdErr(false),
  m_bNoFont(false),
  m_bPaperInChute(false),
  m_bFlashErr(false),
  m_bOffLine(false),
  m_bWrongPaper(false),
  m_bJournalMode(false),
  m_bCutterErr(false),
  m_bPaperJam(false),
  m_bPaperLow(false),
  m_bLastBarPrinted(false),
  m_bTopOfForm(true),
  m_bReadyToRx(false),
  m_bDoorOpened(false),
  m_bPowerUpReset(false)
{
}

/// <summary>Checks for general error.</summary>
/// <returns>True if general error, false otherwise.</returns>
bool CStatus::GeneralErr() const
{
  if(m_bNoFont) { return true; }
  if(m_bOffLine) { return true; }
  if(m_bJournalMode) { return true; }

  return false;
}

/// <summary>Checks if should go suspend.</summary>
/// <returns>True if should go suspend, false otherwise.</returns>
bool CStatus::ShouldSuspend() const
{
  if(ExtPowerLost()) { return true; }
  if(FirmwareErr()) { return true; }
  if(NVMErr()) { return true; }
  if(PrintHeadErr()) { return true; }
  if(TemperatureErr()) { return true; }
  if(GeneralErr()) { return true; }
  if(PrintHeadOpened()) { return true; }
  if(PaperJam()) { return true; }
  if(PaperEmpty()) { return true; }
  if(!TopOfForm()) { return true; }

  return false;
}

/// <summary>Checks if need to send clear error status command.</summary>
/// <returns>True if need to send clear error command, false otherwise.</returns>
bool CStatus::NeedClearErr() const
{
  return (m_bLibRefErr || m_bRegionDataErr || m_bLoadLibErr ||
    m_bBufferOverflow || m_bJobMemOverflow || m_bCmdErr ||
    m_bLastBarPrinted || !m_bTopOfForm);
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CStatus::Dump(MSXML2::IXMLDOMElement* pElem)
{
  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bBusy", m_bBusy);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bError", m_bError);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bPrintHeadOpen",
        m_bPrintHeadOpen);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bPaperOut", m_bPaperOut);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bPrintHeadErr", m_bPrintHeadErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bVoltageErr", m_bVoltageErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bTemperatureErr",
        m_bTemperatureErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bLibRefErr", m_bLibRefErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bRegionDataErr",
        m_bRegionDataErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bLoadLibErr", m_bLoadLibErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bBufferOverflow",
        m_bBufferOverflow);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bJobMemOverflow",
        m_bJobMemOverflow);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bCmdErr", m_bCmdErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bNoFont", m_bNoFont);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bPaperInChute", m_bPaperInChute);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bFlashErr", m_bFlashErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bOffLine", m_bOffLine);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bWrongPaper", m_bWrongPaper);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bJournalMode", m_bJournalMode);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bCutterErr", m_bCutterErr);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bPaperJam", m_bPaperJam);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bPaperLow", m_bPaperLow);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bLastBarPrinted",
        m_bLastBarPrinted);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bTopOfForm", m_bTopOfForm);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bReadyToRx", m_bReadyToRx);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bDoorOpened", m_bDoorOpened);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bPowerUpReset", m_bPowerUpReset);
    } // if...

  }
  catch(...) {}
}

