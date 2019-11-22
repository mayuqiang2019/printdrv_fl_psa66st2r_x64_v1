#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateUnInit::CStateUnInit(IStateMach* pStateMach, CPrinterContext* pContext,
                       CState* pParent) :
  CState(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateUnInit::GetID()
{
  return STATE_UNINIT;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateUnInit::OnEnter(bool isTarget)
{
  if(isTarget && m_pContext->m_bDebug)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateUnInit::OnEnter]\n");
  }

  m_nResendCnt = 0;

  m_pContext->SetStopThread(true);
  if(m_pContext->m_hThread != NULL)
  {
    ::WaitForSingleObject(m_pContext->m_hThread, INFINITE);
    if(!::CloseHandle(m_pContext->m_hThread))
    {
      CLog::Log(L"[printdrv_fl_psa66st2r][CStateUnInit::OnEnter] failed to ::WaitForSingleObject():%u\n",
        GetLastError());
    }
    m_pContext->m_hThread = NULL;
  } // if...

  m_pContext->m_Port.Close();
}

/// <summary>Initializes printer.</summary>
/// <param name="param">Parameter string.</param>
/// <param name="evtObserver">Pointer to event observer.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="param"/> is
/// invalid.</exception>
/// <exception cref="CException">If initialization failed.</exception>
void CStateUnInit::Init(const wchar_t* param, print::IEvtObserver* evtObserver)
{
  CWkString strTmp;

  m_pContext->Parse(param);
  m_pContext->m_pEvtObserver = evtObserver;

  if(!m_pContext->m_Port.Open())
  {
    strTmp.Format(L"failed to open port:%i", m_pContext->m_Port.GetPort());
    throw print::CException(strTmp, WCL_WFUNCSIG);
  }

  m_pContext->SetStopThread(false);

  m_ThreadParam.m_pContext = m_pContext;
  m_ThreadParam.m_pStateMach = m_pStateMach;

  m_pContext->m_hThread = ::CreateThread(NULL, 0, CPrinterContext::_Run,
    &m_ThreadParam, 0, NULL);
  if(m_pContext->m_hThread == NULL)
  {
    m_pContext->m_Port.Close();

    WCL_THROW_SYSTEMEXCEPTION(GetLastError(), L"failed to CreateThread");
  } // if...

  m_pStateMach->Transit(STATE_INIT);
}

/// <summary>Suspends the printer.</summary>
/// <remarks>If invoked before initialization, printer will enter suspend mode
/// immediately after initialization (which is also the default behaviour),
/// but suspended event will not be issued as observer is not ready yet.
/// If invoked in the middle of printing, printer will enter suspended mode but
/// still continue to finish current print job.</remarks>
void CStateUnInit::Suspend()
{
  m_pContext->m_bInitSuspend = true;
}

/// <summary>Resumes printer from suspend mode.</summary>
/// <remarks>If invoked before initialization, printer will enter resumed mode
/// immediately after initialization, but resumed event will not be issued
/// as observer is not ready yet.</remarks>
void CStateUnInit::Resume()
{
  m_pContext->m_bInitSuspend = false;
}
