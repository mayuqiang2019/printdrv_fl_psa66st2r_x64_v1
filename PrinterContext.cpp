#include "stdafx.h"
#include "printer.h"

/// <summary>Constructor.</summary>
CPrinterContext::CPrinterContext() :
  m_bDebug(false),
  m_bErrDump(true),
  m_pEvtObserver(NULL),
  m_dwLastCmdSize(0),
  m_dwLastCmdMemSize(1024),
  m_hThread(NULL),
  m_bInitSuspend(true),
  m_bStopThread(true),
  m_pJobFilter(NULL)
{
  m_pbyLastCmd = new BYTE[m_dwLastCmdMemSize];
}

/// <summary>Destructor.</summary>
CPrinterContext::~CPrinterContext()
{
  delete[] m_pbyLastCmd;
}

/// <summary>Extracts context related parameters.</summary>
/// <param name="parameters">Parameters string.</param>
void CPrinterContext::Parse(const wchar_t* parameters)
{
	CWkString value;
	CWkMapStr<CWkString> pair;

  m_Port.Parse(parameters);

	::Parse(parameters, pair);
  if(pair.Get(L"debug", value))
  {
    m_bDebug = (wcstol((const wchar_t*)value, NULL, 10) == 1);
  } // if...

  if(pair.Get(L"error_dump", value))
  {
    m_bErrDump = (wcstol((const wchar_t*)value, NULL, 10) == 1);
  } // if...

	m_strCfgSoftwareVer = CWkString();
	if(pair.Get(L"version", value))
	{
		m_strCfgSoftwareVer = value;
	}

	m_strCfgCurrency = CWkString();
	if(pair.Get(L"currency", value))
	{
		m_strCfgCurrency = value;
	}
}

/// <summary>Thread-safe function to check if <see cref="m_bStopThread"/> is set.
/// </summary>
/// <returns>Value of <see cref="m_bStopThread"/>.</returns>
bool CPrinterContext::StopThread()
{
  bool bTmp;

  m_csStopThread.Enter();
  bTmp = m_bStopThread;
  m_csStopThread.Leave();

  return bTmp;
}

/// <summary>Thread-safe function to set <see cref="m_bStopThread"/>.</summary>
/// <param name="stop">New value of <see cref="m_bStopThread"/>.</param>
void CPrinterContext::SetStopThread(bool stop)
{
  m_csStopThread.Enter();
  m_bStopThread = stop;
  m_csStopThread.Leave();
}

/// <summary>Sends message and remembers the message as last sent command.</summary>
/// <param name="msg">Message to be sent.</param>
void CPrinterContext::SendNUpdateLastCmd(CMsg& msg)
{
  m_dwLastCmdSize = msg.Build(NULL, 0);
  if(m_dwLastCmdSize > m_dwLastCmdMemSize)
  {
    delete[] m_pbyLastCmd;
    m_pbyLastCmd = new BYTE[m_dwLastCmdSize];
    if(m_pbyLastCmd == NULL) { throw wcl::COutOfMemoryException(); }
    m_dwLastCmdMemSize = m_dwLastCmdSize;
  }

  msg.Build(m_pbyLastCmd, m_dwLastCmdSize);
  m_Port.Write(m_pbyLastCmd, m_dwLastCmdSize);
}

#define CHECK_EVT(errFunc, evtFunc) if(status.errFunc() != m_Status.errFunc())\
                                    {\
                                      if(m_pEvtObserver != NULL)\
                                      {\
                                        m_pEvtObserver->evtFunc(status.errFunc());\
                                      }\
                                    }

/// <summary>Updates status and Notifies observer if errors detected.</summary>
/// <param name="status">Latest printer status.</param>
void CPrinterContext::UpdateStatusNNotifyObserver(const CStatus& status)
{
  if(status.ExtPowerLost() && !m_Status.ExtPowerLost())
  {
    if(m_pEvtObserver != NULL) { m_pEvtObserver->OnExtPowerLost(); }
  }
  else if(!status.ExtPowerLost() && m_Status.ExtPowerLost())
  {
    if(m_pEvtObserver != NULL) { m_pEvtObserver->OnExtPowerResumed(); }
  } // if...else...

  CHECK_EVT(FirmwareErr, OnFirmwareErr);
  CHECK_EVT(NVMErr, OnNVMErr);
  CHECK_EVT(PrintHeadErr, OnPrintHeadErr);
  CHECK_EVT(TemperatureErr, OnTemperatureErr);
  CHECK_EVT(GeneralErr, OnGeneralErr);
  CHECK_EVT(PrintHeadOpened, OnPrintHead);
  CHECK_EVT(PaperJam, OnPaperJam);
  CHECK_EVT(PaperEmpty, OnPaperEmpty);
  CHECK_EVT(TopOfForm, OnTopOfForm);
  CHECK_EVT(ChassisOpened, OnChassis);
  CHECK_EVT(PaperLow, OnPaperLow);

  m_Status = status;
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CPrinterContext::Dump(MSXML2::IXMLDOMElement* pElem)
{
  DWORD i;
  CWkString str1, str2;
  MSXML2::IXMLDOMElement *pChild = NULL;

  try
  {
    
    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpChild<CPrinterPort&>(pElem, L"m_Port", m_Port);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bDebug", m_bDebug);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bErrDump", m_bErrDump);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pEvtObserver",
        (DWORD)m_pEvtObserver);

      if(!CXmlUtil::AppendChild(pElem, L"m_pbyLastCmd", &pChild)) { throw false; }
      if(m_pbyLastCmd != NULL)
      {
        for(i = 0;i < m_dwLastCmdSize;i++)
        {
          str2.Format(L"0x%X", m_pbyLastCmd[i]);
          str1 += str2;
          if(i != (m_dwLastCmdSize - 1)) { str1 += L" "; }
        } // for...
        if(!CXmlUtil::SetValue(pChild, (const wchar_t*)str1)) { throw false; }
      }
      SAFE_RELEASE(pChild);

      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_dwLastCmdSize", m_dwLastCmdSize);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_dwLastCmdMemSize", m_dwLastCmdMemSize);
      wcl::CDumpHelper::DumpChild<print::CGraphic&>(pElem, L"m_LastGraphic",
        m_LastGraphic);
      wcl::CDumpHelper::DumpChild<print::CRegion&>(pElem, L"m_LastRegion",
        m_LastRegion);
      wcl::CDumpHelper::DumpChild<print::CTemplate&>(pElem, L"m_LastTemplate",
        m_LastTemplate);
      wcl::CDumpHelper::DumpAttr<const wchar_t*>(pElem, L"m_strSoftwareVer",
        m_strSoftwareVer);
      wcl::CDumpHelper::DumpChild<CStatus&>(pElem, L"m_Status", m_Status);

      if(!CXmlUtil::AppendChild(pElem, L"m_RegionDefData", &pChild))
      {
        throw false;
      }
      wcl::CDumpHelper::DumpMap<int, CWkString>(m_RegionDefData, pChild);
      SAFE_RELEASE(pChild);

      if(!CXmlUtil::AppendChild(pElem, L"m_Template", &pChild))
      {
        throw false;
      }
      wcl::CDumpHelper::DumpComplexMap<int, print::CTemplate>(m_Template, pChild);
      SAFE_RELEASE(pChild);

      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_hThread", (DWORD)m_hThread);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bInitSuspend", m_bInitSuspend);
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bStopThread", m_bStopThread);
    } // if...

  }
  catch(...) {}
  SAFE_RELEASE(pChild);
}

/// <summary>Run thread execution.</summary>
DWORD WINAPI CPrinterContext::_Run(LPVOID lpParameter)
{
  CWkString strTmp;
  SRunThreadParam *pParam = (SRunThreadParam*)lpParameter;
  CPrinterContext *pContext = pParam->m_pContext;
  IStateMach *pStateMach = pParam->m_pStateMach;

  DWORD dwNow, dwLastTime = CWkTime::GetTime();

  try
  {

    while(!pContext->StopThread())
    {
      dwNow = CWkTime::GetTime();
      pStateMach->Run(dwNow - dwLastTime);
      dwLastTime = dwNow;
      Sleep(RUN_INTERVAL);
    } // while...

  }
  catch(wcl::CSelfDocException& e)
  {
    e.ToString(strTmp);
    pContext->Trace(L"%s\n", (const wchar_t*)strTmp);
  }
  catch(...)
  {
    try
    {
      pContext->m_pEvtObserver->OnGeneralErr(true);
    }
    catch(...){}

    if(pContext->m_bErrDump) { pStateMach->Dump(L"CPrinterContext::_Run"); }
  } // try...catch...

  return 0;
}
