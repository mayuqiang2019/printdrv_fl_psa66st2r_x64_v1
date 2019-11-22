#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateSuspended::CStateSuspended(IStateMach* pStateMach, CPrinterContext* pContext,
                                CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateSuspended::GetID()
{
  return STATE_SUSPENDED;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateSuspended::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateSuspended::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);
}

/// <summary>Suspends the printer.</summary>
/// <remarks>If invoked before initialization, printer will enter suspend mode
/// immediately after initialization (which is also the default behaviour),
/// but suspended event will not be issued as observer is not ready yet.
/// If invoked in the middle of printing, printer will enter suspended mode but
/// still continue to finish current print job.</remarks>
void CStateSuspended::Suspend()
{
  if(m_pContext->m_pEvtObserver != NULL)
  {
    m_pContext->m_pEvtObserver->OnSuspended();
  } // if...
}

/// <summary>Resumes printer from suspend mode.</summary>
/// <remarks>If invoked before initialization, printer will enter resumed mode
/// immediately after initialization, but resumed event will not be issued
/// as observer is not ready yet.</remarks>
void CStateSuspended::Resume()
{
  if(m_pContext->m_Status.ShouldSuspend())
  {
    if(m_pContext->m_pEvtObserver != NULL)
    {
      m_pContext->m_pEvtObserver->OnSuspended();
    }
  }
  else
  {
    if(m_pContext->m_pEvtObserver != NULL)
    {
      m_pContext->m_pEvtObserver->OnResumed();
    }
    m_pStateMach->Transit(STATE_IDLE);
  } // if...else...
}

/// <summary>Performs self-test.</summary>
/// <param name="clearNVM">True to clear previous queued Transaction ID events
/// and Transaction ID sequence number in the printer's Non-volatile Memory
/// (NVM) before performing self-test, false to perform self-test without
/// clearing the NVM.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CStateSuspended::SelfTest(bool clearNVM)
{
  // just print a void page, and don't care whether it success or not.
  int i;
  print::CJob job;
  CMsgPrint msg;

  job.m_nsTemplateID = 6;
  for(i = 7;i > 0;--i) { job.AddTail(print::CData()); }

  msg.m_pJob = &job;

  try
  {
    m_pContext->SendNUpdateLastCmd(msg);
  }
  catch(CCommException& e)
  {
    //m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateSuspended::SelfTest] disconnected due to CCommException, port:%i, msg:%s, err:%u.\n",
    //  e.GetPort(), e.GetMsg(), e.GetSysErrCode);
    m_pStateMach->Transit(STATE_DISCONNECTED);
  }
}

/// <summary>Requests diagnostic information from the printer via a GAT Data
/// event.</summary>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CStateSuspended::RqGATReport()
{
  CMsgStatus msg;

  try
  {
    m_pContext->SendNUpdateLastCmd(msg);
    m_pStateMach->Transit(STATE_GAT_REPORT);
  }
  catch(CCommException& e)
  {
    //m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateSuspended::RqGATReport] disconnected due to CCommException, port:%i, msg:%s, err:%u.\n",
    //  e.GetPort(), e.GetMsg(), e.GetSysErrCode);
    m_pStateMach->Transit(STATE_DISCONNECTED);
  }
}

/// <summary>Requests a 32-bit checksum from the code space within the
/// printer's ROM memory.</summary>
/// <param name="seed">32-bit seed.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Printer will not be functioning during the calculation.</remarks>
void CStateSuspended::CalculateCRC(DWORD seed)
{
  CMsgObtainCRC msg;

  msg.m_dwStartAddr = 0;
  msg.m_wSeed = (WORD)seed;

  try
  {
    m_pContext->SendNUpdateLastCmd(msg);
    m_pStateMach->Transit(STATE_CRC);
  }
  catch(CCommException& e)
  {
    //m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateSuspended::CalculateCRC] disconnected due to CCommException, port:%i, msg:%s, err:%u.\n",
    //  e.GetPort(), e.GetMsg(), e.GetSysErrCode);
    m_pStateMach->Transit(STATE_DISCONNECTED);
  }
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateSuspended::HandleRespStatus(BYTE* resp, DWORD size)
{
  CMsgRespStatus msg;

  msg.Parse(resp, size);

  try
  {

    if(CStatePollStatus::HandleRespStatus(resp, size)) { return true; }
    m_pContext->UpdateStatusNNotifyObserver(msg.m_Status);

  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateSuspended::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...

  return true;
}
