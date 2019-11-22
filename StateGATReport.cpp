#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateGATReport::CStateGATReport(IStateMach* pStateMach, CPrinterContext* pContext,
                       CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateGATReport::GetID()
{
  return STATE_GAT_REPORT;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateGATReport::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateGATReport::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateGATReport::HandleRespStatus(BYTE* resp, DWORD size)
{
  CMsgRespStatus msg;

  msg.Parse(resp, size);

  try
  {

    if(CStatePollStatus::HandleRespStatus(resp, size)) { return true; }
    m_pContext->UpdateStatusNNotifyObserver(msg.m_Status);

    if(m_pContext->m_pEvtObserver != NULL)
    {
      m_pContext->m_pEvtObserver->OnGATReportReady(msg.m_strSoftwareVer);
    }
    m_pStateMach->Transit(STATE_SUSPENDED);

  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateGATReport::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...

  return true;
}
