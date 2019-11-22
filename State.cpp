#include "stdafx.h"
#include "printer.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CState::CState(IStateMach* pStateMach, CPrinterContext* pContext, CState* pParent) :
  m_nResendCnt(0),
  m_pStateMach(pStateMach),
  m_pContext(pContext),
  m_pParent(pParent)
{
  if(pStateMach == NULL) { WCL_THROW_ARGUMENTNULLEXCEPTION(L"pStateMach"); }
  if(pContext == NULL) { WCL_THROW_ARGUMENTNULLEXCEPTION(L"pContext"); }
}

/// <summary>Destructor.</summary>
CState::~CState()
{
  POS pos = m_Children.GetHeadPos();
  while(pos != NULL) { delete m_Children.GetNext(pos); }
}

/// <summary>Retrieves parent state.</summary>
/// <returns>Pointer to parent state, NULL if not parent state.</returns>
CState* CState::GetParent()
{
  return m_pParent;
}

/// <summary>Checks if this state is ancestor of specified state.</summary>
/// <param name="id">ID of descendant state to be checked.</param>
/// <returns>Pointer to the child state which is the specified state, or is
/// the ancenstor of specified state. NULL if this state is NOT the ancenstor
/// of specified state.</returns>
CState* CState::IsAncenstorOf(int id)
{
  CState *pChild;
  POS pos = m_Children.GetHeadPos();

  while(pos != NULL)
  {
    pChild = m_Children.GetNext(pos);
    if((pChild->GetID() == id) || pChild->IsAncenstorOf(id)) { return pChild; }
  } // while...

  return NULL;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CState::OnEnter(bool isTarget)
{
  m_nResendCnt = 0;
}

/// <summary>Handles state exited event.</summary>
void CState::OnLeave()
{

}

/// <summary>State execution.<summary>
/// <param name="elapsed">Time elapsed since last run, in milliseconds.</param>
void CState::Run(DWORD elapsed)
{

}

/// <summary>Handles printer response.</summary>
/// <param name="resp">Printer response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CState::HandleResp(BYTE* resp, DWORD size)
{
  CMsgMgr mgr;
  DWORD i;

  switch( mgr.GetType(resp, size) )
  {
  case CMsgMgr::CMD_CRC : return HandleRespCRC(resp, size);
  case CMsgMgr::CMD_STATUS  : return HandleRespStatus(resp, size);
  default :
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CState::HandleResp] unknown response:");
    for(i = 0;i < size;i++) { m_pContext->Trace(L" 0x%X", resp[i]); }
    m_pContext->Trace(L"\n");
    break;
  } // switch...

  return false;
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CState::Dump(MSXML2::IXMLDOMElement* pElem)
{
  POS pos;
  CState *pState;
  MSXML2::IXMLDOMElement *pChild = NULL, *pChild2 = NULL;

  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<int>(pElem, L"m_nResendCnt", m_nResendCnt);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pStateMach",
        (DWORD)m_pStateMach);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pContext", (DWORD)m_pContext);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pParent", (DWORD)m_pParent);

      if(!CXmlUtil::AppendChild(pElem, L"m_Children", &pChild)) { throw false; }
      pos = m_Children.GetHeadPos();
      while(pos != NULL)
      {
        pState = m_Children.GetNext(pos);
        if(!CXmlUtil::AppendChild(pChild, L"CState", &pChild2)) { throw false; }
        pState->Dump(pChild2);
        SAFE_RELEASE(pChild2);
      }
      SAFE_RELEASE(pChild);
    } // if...

  }
  catch(...) {}
}

/// <summary>Initializes printer.</summary>
/// <param name="param">Parameter string.</param>
/// <param name="evtObserver">Pointer to event observer.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="param"/> is
/// invalid.</exception>
/// <exception cref="CException">If initialization failed.</exception>
void CState::Init(const wchar_t* param, print::IEvtObserver* evtObserver)
{
}

/// <summary>Un-initializes printer.</summary>
void CState::UnInit()
{
}

/// <summary>Suspends the printer.</summary>
/// <remarks>If invoked before initialization, printer will enter suspend mode
/// immediately after initialization (which is also the default behaviour),
/// but suspended event will not be issued as observer is not ready yet.
/// If invoked in the middle of printing, printer will enter suspended mode but
/// still continue to finish current print job.</remarks>
void CState::Suspend()
{
}

/// <summary>Resumes printer from suspend mode.</summary>
/// <remarks>If invoked before initialization, printer will enter resumed mode
/// immediately after initialization, but resumed event will not be issued
/// as observer is not ready yet.</remarks>
void CState::Resume()
{
}

/// <summary>Retrieves printer metrics.</summary>
/// <param name="metrics">Reference to <see cref="CMetrics"/> object to receive
/// printer metrics.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CState::GetMetrics(print::CMetrics& metrics)
{
}

/// <summary>Performs self-test.</summary>
/// <param name="clearNVM">True to clear previous queued Transaction ID events
/// and Transaction ID sequence number in the printer's Non-volatile Memory
/// (NVM) before performing self-test, false to perform self-test without
/// clearing the NVM.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CState::SelfTest(bool clearNVM)
{
}

/// <summary>Requests diagnostic information from the printer via a GAT Data
/// event.</summary>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CState::RqGATReport()
{
}

/// <summary>Requests a 32-bit checksum from the code space within the
/// printer's ROM memory.</summary>
/// <param name="seed">32-bit seed.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Printer will not be functioning during the calculation.</remarks>
void CState::CalculateCRC(DWORD seed)
{
}

/// <summary>Defines graphic.</summary>
/// <param name="graphic">Graphic.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="graphic"/> is
/// invalid.</exception>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>If graphic of same identifier already exists, new graphic will
/// replace current one. If identifier being replaced is for a predefined
/// graphic, the new graphic will still replace the predefined graphic, but the
/// original predefined graphic will be restored after power cycle reset.</remarks>
void CState::DefineGraphic(const print::CGraphic& graphic)
{
}

/// <summary>Defines printable region.</summary>
/// <param name="region">Printable region.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="region"/> is
/// invalid.</exception>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Only region ID between 100 ~ 999 can be defined. If a region ID
/// already defined, new region will replace the current region definition.</remarks>
void CState::DefineRegion(const print::CRegion& region)
{
}

/// <summary>Defines printable template.</summary>
/// <param name="templ">Printable template.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="template"/>
/// is invalid.</exception>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CState::DefineTemplate(const print::CTemplate& templ)
{
}

/// <summary>Prints a job using specified template.</summary>
/// <param name="job">Print job.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Printer will ignore this call when it is in suspend mode, or when
/// following conditions are present: print head open, paper jam, paper empty,
/// top of form.</remarks>
void CState::Print(const print::CJob& job)
{
}

/// <summary>Feeds a blank ticket.</summary>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CState::FormFeed()
{
}

/// <summary>Retrieves firmware currency.</summary>
/// <param name="currency">Reference to <see cref="CWkString"/> object to
/// receive firmware currency, is ISO 4217.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CState::GetFirmwareCurrency(CWkString& currency)
{
    /*
    char* software = new char[m_pContext->m_strSoftwareVer.GetLength()];
    if (strstr(software, "HKD") != NULL) { currency = L"HKD";  }
    if (strstr(software, "PHP") != NULL) { currency = L"PHP"; }
    if (strstr(software, "EUR") != NULL) { currency = L"EUR"; }
    if (strstr(software, "KRW") != NULL) { currency = L"KRW"; }
    if (strstr(software, "LKR") != NULL) { currency = L"LKR"; }
    if (strstr(software, "MYR") != NULL) { currency = L"MYR"; }
    if (strstr(software, "SGD") != NULL) { currency = L"SGD"; }
    if (strstr(software, "THB") != NULL) { currency = L"THB"; }
    if (strstr(software, "TWD") != NULL) { currency = L"TWD"; }
    if (strstr(software, "USD") != NULL) { currency = L"USD"; }
    if (strstr(software, "ZAR") != NULL) { currency = L"ZAR"; }
    if (strstr(software, "ZAR") != NULL) { currency = L"AUD"; }
    */    
  if(m_pContext->m_strSoftwareVer == L"GUR126001") { currency = L"HKD"; }
  else if(m_pContext->m_strSoftwareVer == L"GUR126003") { currency = L"HKD"; }
  else if (m_pContext->m_strSoftwareVer == L"GURMAC105") { currency = L"HKD"; }
  else if(m_pContext->m_strSoftwareVer == L"GUR109000") { currency = L"PHP"; }
  else if(m_pContext->m_strSoftwareVer == L"GUREUR100") { currency = L"EUR"; }
  else if(m_pContext->m_strSoftwareVer == L"GUREUR101") { currency = L"EUR"; }
  else if(m_pContext->m_strSoftwareVer == L"GUREUR102") { currency = L"EUR"; }
  else if(m_pContext->m_strSoftwareVer == L"GUREUR200") { currency = L"EUR"; }
  else if (m_pContext->m_strSoftwareVer == L"GUREURGE2") { currency = L"EUR"; }
  else if (m_pContext->m_strSoftwareVer == L"GURKORGE1") { currency = L"KRW"; }
  else if (m_pContext->m_strSoftwareVer == L"GURLKAGE1") { currency = L"LKR"; }
  else if (m_pContext->m_strSoftwareVer == L"GURMYSGE0") { currency = L"MYR"; }
  else if (m_pContext->m_strSoftwareVer == L"GURPHI101") { currency = L"PHP"; }
  else if (m_pContext->m_strSoftwareVer == L"GURSNG103") { currency = L"SGD"; }
  else if (m_pContext->m_strSoftwareVer == L"GURTHAGE0") { currency = L"THB"; }
  else if (m_pContext->m_strSoftwareVer == L"GURTHBGE0") { currency = L"THB"; }
  else if (m_pContext->m_strSoftwareVer == L"GURLTG000") { currency = L"TWD"; }
  else if (m_pContext->m_strSoftwareVer == L"GURUSAG13") { currency = L"USD"; }
  else if (m_pContext->m_strSoftwareVer == L"GURUSDUS1") { currency = L"USD"; }
  else if (m_pContext->m_strSoftwareVer == L"GURSAFGE0") { currency = L"ZAR"; }
  else if((m_pContext->m_strSoftwareVer == L"GURTOR100")||(m_pContext->m_strSoftwareVer == L"GURTOR101")) 
  {
    currency = L"HKD";
  }
  else if((m_pContext->m_strSoftwareVer == L"GURUSA001")||(m_pContext->m_strSoftwareVer == L"GURUSA003"))
  {
	  currency = L"USD";
  }
  else if(m_pContext->m_strSoftwareVer == L"GURNSW200")	{ currency = L"AUD"; }
  else if(m_pContext->m_strSoftwareVer == L"GURSNG103")	{ currency = L"SGD"; }
  else
  {
	  if(m_pContext->m_strSoftwareVer == m_pContext->m_strCfgSoftwareVer)
	  {
		  currency = m_pContext->m_strCfgCurrency;
	  }
	  else { currency = L""; }
  }
}

/// <summary>Handles printer's response to obtain program CRC command.</summary>
/// <param name="resp">Printer CRC response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CState::HandleRespCRC(BYTE* resp, DWORD size)
{
  return false;
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CState::HandleRespStatus(BYTE* resp, DWORD size)
{
  return false;
}
