#include "stdafx.h"
#include "printer.h"

#include "state.h"

/// <summary>Constructor.</summary>
CPrinter::CPrinter()
{
  m_pStateTop = new CStateTop(this, &m_Context, NULL);
  if(m_pStateTop == NULL) { throw wcl::COutOfMemoryException(); }

  m_pCurState = m_pStateTop;
  Transit(STATE_UNINIT);
}

/// <summary>Destructor.</summary>
CPrinter::~CPrinter()
{
  UnInit();
  delete m_pStateTop;
}

/// <summary>Transits to target state.</summary>
/// <param name="target">Target state ID.</param>
/// <exception cref="wcl::CArgumentException">If state referred by
/// <paramref name="target"/> does not exist.</exception>
void CPrinter::Transit(int target)
{
  bool reached = false;
  CState *pNext;

  do
  {
    pNext = m_pCurState->IsAncenstorOf(target);
    if(pNext != NULL)
    {
      m_pCurState = pNext;
      reached = (m_pCurState->GetID() == target);
      m_pCurState->OnEnter(reached);

      // to ensure loop break even when OnEnter() recursively calls this function.
      if(reached) { break; }
    }
    else
    {
      // does not support OnLeave() recursively calls this function.
      m_pCurState->OnLeave();
      m_pCurState = m_pCurState->GetParent();
      if(m_pCurState == NULL)
      {
        WCL_THROW_ARGUMENTEXCEPTION(L"target", L"target not found");
      }
    } // if...else...
  }
  while(m_pCurState->GetID() != target);
}

/// <summary>Initializes printer.</summary>
/// <param name="param">Parameter string.</param>
/// <param name="evtObserver">Pointer to event observer.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="param"/> is
/// invalid.</exception>
/// <exception cref="CException">If initialization failed.</exception>
void CPrinter::Init(const wchar_t* param, print::IEvtObserver* evtObserver)
{
  m_csThis.Enter();
  m_pCurState->Init(param, evtObserver);
  m_csThis.Leave();
}

/// <summary>Un-initializes printer.</summary>
void CPrinter::UnInit()
{
  m_csThis.Enter();
  m_pCurState->UnInit();
  m_csThis.Leave();
}

/// <summary>Suspends the printer.</summary>
/// <remarks>If invoked before initialization, printer will enter suspend mode
/// immediately after initialization (which is also the default behaviour),
/// but suspended event will not be issued as observer is not ready yet.
/// If invoked in the middle of printing, printer will enter suspended mode but
/// still continue to finish current print job.</remarks>
void CPrinter::Suspend()
{
  m_csThis.Enter();
  m_pCurState->Suspend();
  m_csThis.Leave();
}

/// <summary>Resumes printer from suspend mode.</summary>
/// <remarks>If invoked before initialization, printer will enter resumed mode
/// immediately after initialization, but resumed event will not be issued
/// as observer is not ready yet.</remarks>
void CPrinter::Resume()
{
  m_csThis.Enter();
  m_pCurState->Resume();
  m_csThis.Leave();
}

/// <summary>Retrieves printer metrics.</summary>
/// <param name="metrics">Reference to <see cref="CMetrics"/> object to receive
/// printer metrics.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CPrinter::GetMetrics(print::CMetrics& metrics)
{
  m_csThis.Enter();
  m_pCurState->GetMetrics(metrics);
  m_csThis.Leave();
}

/// <summary>Performs self-test.</summary>
/// <param name="clearNVM">True to clear previous queued Transaction ID events
/// and Transaction ID sequence number in the printer's Non-volatile Memory
/// (NVM) before performing self-test, false to perform self-test without
/// clearing the NVM.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CPrinter::SelfTest(bool clearNVM)
{
  m_csThis.Enter();
  m_pCurState->SelfTest(clearNVM);
  m_csThis.Leave();
}

/// <summary>Requests diagnostic information from the printer via a GAT Data
/// event.</summary>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CPrinter::RqGATReport()
{
  m_csThis.Enter();
  m_pCurState->RqGATReport();
  m_csThis.Leave();
}

/// <summary>Requests a 32-bit checksum from the code space within the
/// printer's ROM memory.</summary>
/// <param name="seed">32-bit seed.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Printer will not be functioning during the calculation.</remarks>
void CPrinter::CalculateCRC(DWORD seed)
{
  m_csThis.Enter();
  m_pCurState->CalculateCRC(seed);
  m_csThis.Leave();
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
void CPrinter::DefineGraphic(const print::CGraphic& graphic)
{
  m_csThis.Enter();
  m_pCurState->DefineGraphic(graphic);
  m_csThis.Leave();
}

/// <summary>Defines printable region.</summary>
/// <param name="region">Printable region.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="region"/> is
/// invalid.</exception>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Only region ID between 100 ~ 999 can be defined. If a region ID
/// already defined, new region will replace the current region definition.</remarks>
void CPrinter::DefineRegion(const print::CRegion& region)
{
  m_csThis.Enter();
  m_pCurState->DefineRegion(region);
  m_csThis.Leave();
}

/// <summary>Defines printable template.</summary>
/// <param name="templ">Printable template.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="template"/>
/// is invalid.</exception>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CPrinter::DefineTemplate(const print::CTemplate& templ)
{
  m_csThis.Enter();
  m_pCurState->DefineTemplate(templ);
  m_csThis.Leave();
}

/// <summary>Prints a job using specified template.</summary>
/// <param name="job">Print job.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Printer will ignore this call when it is in suspend mode, or when
/// following conditions are present: print head open, paper jam, paper empty,
/// top of form.</remarks>
void CPrinter::Print(const print::CJob& job)
{
  m_csThis.Enter();
  m_pCurState->Print(job);
  m_csThis.Leave();
}

/// <summary>Feeds a blank ticket.</summary>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CPrinter::FormFeed()
{
  m_csThis.Enter();
  m_pCurState->FormFeed();
  m_csThis.Leave();
}

/// <summary>Retrieves firmware currency.</summary>
/// <param name="currency">Reference to <see cref="CWkString"/> object to
/// receive firmware currency, is ISO 4217.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CPrinter::GetFirmwareCurrency(CWkString& currency)
{
  m_csThis.Enter();
  m_pCurState->GetFirmwareCurrency(currency);
  m_csThis.Leave();
}

/// <summary>Executes state.</summary>
/// <param name="elapsed">Time elapsed since last run, in milliseconds.</param>
void CPrinter::Run(DWORD elapsed)
{
  if( m_csThis.TryEnter() )
  {
    m_pCurState->Run(elapsed);
    m_csThis.Leave();
  } // if...
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CPrinter::Dump(MSXML2::IXMLDOMElement* pElem)
{
  MSXML2::IXMLDOMElement *pChild = NULL;

  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpChild<CPrinterContext&>(pElem, L"m_Context",
        m_Context);

      if(!CXmlUtil::AppendChild(pElem, L"m_pStateTop", &pChild)) { throw false; }
      m_pStateTop->Dump(pChild);
      SAFE_RELEASE(pChild);

      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pCurState", (DWORD)m_pCurState);
    } // if...

  }
  catch(...) {}
  SAFE_RELEASE(pChild);
}

///	<summary>Dump object data to XML.</summary>
///	<param name='pElem'>XML DOM to contain object data.</param>
///	<param name='func'>Name of function which triggers the dump.</param>
///	<remarks>
///	The purpose of this function is to dump debugging information when unexpected
///	error occurred (due to software bugs). Do NOT invoked this function during 
///	application running, as all critical section will be by-passed in this function 
///	call (to prevent deadlock during dumping).
///	</remarks>
void CPrinter::Dump(MSXML2::IXMLDOMElement* pElem, const wchar_t* func)
{
  try
  {

    if(pElem != NULL)
    {
      if(func != NULL)
      {
        wcl::CDumpHelper::DumpAttr<const wchar_t*>(pElem, L"func", func);
      }
      Dump(pElem);
    } // if...

  }
  catch(...) {}
}

///	<summary>Dump object data to XML.</summary>
///	<param name='func'>Name of function which triggers the dump.</param>
///	<remarks>
///	The purpose of this function is to dump debugging information when unexpected
///	error occurred (due to software bugs). Do NOT invoked this function during 
///	application running, as all critical section will be by-passed in this function 
///	call (to prevent deadlock during dumping).
///	</remarks>
void CPrinter::Dump(const wchar_t* func)
{
  CWkString filename(L"err_printdrv_fl_psa66st2r_"), strTmp;
  CWkTime tTmp;
  MSXML2::IXMLDOMDocument *pDoc = NULL;
  MSXML2::IXMLDOMElement *pRoot = NULL;
  	_TCHAR	oldDir[MAX_PATH], name[MAX_PATH];

	_tgetcwd(oldDir, MAX_PATH);

  try
  {

	// retrieves root directory name.
	memset(name, 0, MAX_PATH);
	strTmp	= oldDir;
	if(strTmp.GetLength() == 0)
	{
		strTmp	= _T("D:\\");
	}
	name[0] = strTmp[0];
	name[1]	= ':';
	name[2]	= '\\';
	_tchdir(name);

    if(!CXmlUtil::CreateDocument(&pDoc)) { throw false; }
    if(!CXmlUtil::AppendChild(pDoc, L"CPrinter", &pRoot)) { throw false; }
    Dump(pRoot, func);

    tTmp.FormatDate(_T("yyyy'_'MM'_'dd'_'"), strTmp);
    filename	+= strTmp;
    tTmp.FormatTime(_T("HH'_'mm'_'ss"), strTmp);
    filename	+= strTmp;
    filename	+= _T(".xml");

    CXmlUtil::SaveXml(filename, pDoc);

  }
  catch(...) {}
  SAFE_RELEASE(pRoot);
  SAFE_RELEASE(pDoc);
	_tchdir(oldDir);
}
