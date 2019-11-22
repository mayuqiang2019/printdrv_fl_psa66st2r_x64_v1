#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateAddTempl::CStateAddTempl(IStateMach* pStateMach,
                               CPrinterContext* pContext, CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateAddTempl::GetID()
{
  return STATE_ADD_TEMPL;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateAddTempl::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateAddTempl::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateAddTempl::HandleRespStatus(BYTE* resp, DWORD size)
{
  int target = STATE_ADD_TEMPL;
  CMsgRespStatus msg;
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

    //***********************************************
    // THESE ERRORS INDICATE FAILURE OF DEFINITION.
    if(m_bPolled)
    {
      if(msg.m_Status.m_bLibRefErr || msg.m_Status.m_bRegionDataErr)
      {
        if(m_pContext->m_pEvtObserver != NULL)
        {
          m_pContext->m_pEvtObserver->OnDefineTemplateFailed(
            print::IObserver::TEMPL_ERR_UNDEFINED_REGION);
        }
        target = STATE_IDLE;
      }
      else if(msg.m_Status.m_bBufferOverflow)
      {
        if(m_pContext->m_pEvtObserver != NULL)
        {
          m_pContext->m_pEvtObserver->OnDefineTemplateFailed(
            print::IObserver::TEMPL_ERR_MEMORY);
        }
        target = STATE_IDLE;
      } // if...
    }

    if(target != STATE_ADD_TEMPL)
    {
      m_pStateMach->Transit(target);
      return true;
    }
    // END OF DEFINITION FAILURE.
    //******************************

    //**********************
    // DEFINITION SUCCESS.
    if(m_bPolled && !msg.m_Status.m_bBusy)
    {
		  if(!msgMgr.IsUserDefinedTempl(m_pContext->m_LastTemplate.m_nsID))
		  {
			  // no need to perform flash transfer when overwriting pre-defined template.
			  // store template.
			  templateID = msgMgr.TemplID2Drv(m_pContext->m_LastTemplate.m_nsID);
			  m_pContext->m_Template.Set(templateID, m_pContext->m_LastTemplate);
			  if(m_pContext->m_pEvtObserver != NULL)
			  {
				  m_pContext->m_pEvtObserver->OnDefineTemplateSuccess();
			  }
			  if(msg.m_Status.ShouldSuspend()) { m_pStateMach->Transit(STATE_SUSPENDED); }
			  else { m_pStateMach->Transit(STATE_IDLE); }
		  }
		  else
		  {
			  // perform flash transfer for user-defined template
			  m_pStateMach->Transit(STATE_FLASH_TRANSFER);
		  }
    } // if...
    // END OF DEFINITION SUCCESS.
    //*****************************

  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateAddTempl::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...

  return true;
}
