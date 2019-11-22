#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateDeleteTempl::CStateDeleteTempl(IStateMach* pStateMach,
                                     CPrinterContext* pContext, CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateDeleteTempl::GetID()
{
  return STATE_DELETE_TEMPL;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateDeleteTempl::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateDeleteTempl::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateDeleteTempl::HandleRespStatus(BYTE* resp, DWORD size)
{
  CMsgRespStatus msg;
  CMsgDefineTempl msgDefineTempl;
  BYTE templateID;
  CMsgMgr msgMgr;

  msg.Parse(resp, size);

  try
  {

    if(CStatePollStatus::HandleRespStatus(resp, size)) { return true; }
    m_pContext->UpdateStatusNNotifyObserver(msg.m_Status);

    //************************************************
    // RETRY IF PRINTER COMPLAIN ABOUT COMMAND SYNTAX
    if(m_bPolled && msg.m_Status.m_bCmdErr)
    {
      if(m_nResendCnt < MAX_RESEND_CNT)
      {
        m_nResendCnt++;
   
        m_pContext->m_Port.Write(m_pContext->m_pbyLastCmd,
          m_pContext->m_dwLastCmdSize);
        m_bPolled = false;
      }
      else
      {
        m_pStateMach->Transit(STATE_DISCONNECTED);
      } // if...else...
      return true;
    }
    else { m_nResendCnt = 0; }
    // END OF RETRY.
    //********************

    //**********************
    // DEFINITION SUCCESS.
    if(m_bPolled && !msg.m_Status.m_bBusy)
    {
      // remove template.
      templateID = msgMgr.TemplID2Drv(m_pContext->m_LastTemplate.m_nsID);
      m_pContext->m_Template.Remove(templateID);

      msgDefineTempl.m_bDefine = true;
      msgDefineTempl.m_pTemplate = &(m_pContext->m_LastTemplate);

      m_pContext->SendNUpdateLastCmd(msgDefineTempl);

      m_pStateMach->Transit(STATE_ADD_TEMPL);
    } // if...
    // END OF DEFINITION SUCCESS.
    //*****************************

  }
  catch(wcl::CArgumentException& e)
  {
    m_pContext->m_pEvtObserver->OnDefineTemplateFailed(print::IObserver::TEMPL_ERR_ID);

    if(msg.m_Status.ShouldSuspend()) { m_pStateMach->Transit(STATE_SUSPENDED); }
    else { m_pStateMach->Transit(STATE_IDLE); }
  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateDeleteTempl::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...

  return true;
}
