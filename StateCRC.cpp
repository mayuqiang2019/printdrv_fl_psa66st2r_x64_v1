#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateCRC::CStateCRC(IStateMach* pStateMach, CPrinterContext* pContext,
                       CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateCRC::GetID()
{
  return STATE_CRC;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateCRC::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateCRC::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);
}

/// <summary>Handles printer's response to obtain program CRC command.</summary>
/// <param name="resp">Printer CRC response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateCRC::HandleRespCRC(BYTE* resp, DWORD size)
{
  CMsgRespCRC msg;

  msg.Parse(resp, size);
  if(m_pContext->m_pEvtObserver != NULL)
  {
    m_pContext->m_pEvtObserver->OnCRCReady(msg.m_wCRC);
  }
  m_pStateMach->Transit(STATE_SUSPENDED);

  return true;
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateCRC::HandleRespStatus(BYTE* resp, DWORD size)
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
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateCRC::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // if...

  return true;
}
