#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateDeleteRegion::CStateDeleteRegion(IStateMach* pStateMach,
                                     CPrinterContext* pContext, CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateDeleteRegion::GetID()
{
  return STATE_DELETE_REGION;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateDeleteRegion::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateDeleteRegion::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateDeleteRegion::HandleRespStatus(BYTE* resp, DWORD size)
{
  CMsgRespStatus msg;
  CMsgDefineRegion msgDefineRegion;
  BYTE regionID;
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
      // remove associated default data.
      regionID = msgMgr.RegionID2Drv(m_pContext->m_LastRegion.m_nsID);
      m_pContext->m_RegionDefData.Remove(regionID);

      msgDefineRegion.m_bDefine = true;
      msgDefineRegion.m_pRegion = &(m_pContext->m_LastRegion);

      m_pContext->SendNUpdateLastCmd(msgDefineRegion);

      m_pStateMach->Transit(STATE_ADD_REGION);
    } // if...
    // END OF DEFINITION SUCCESS.
    //*****************************

  }
  catch(CInvalidRegionException& e)
  {
    m_pContext->m_pEvtObserver->OnDefineRegionFailed(print::IObserver::REGION_ERR_DATATYPE_MISMATCH);

    if(msg.m_Status.ShouldSuspend()) { m_pStateMach->Transit(STATE_SUSPENDED); }
    else { m_pStateMach->Transit(STATE_IDLE); }
  }
  catch(wcl::CArgumentException& e)
  {
    m_pContext->m_pEvtObserver->OnDefineRegionFailed(print::IObserver::REGION_ERR_ID);

    if(msg.m_Status.ShouldSuspend()) { m_pStateMach->Transit(STATE_SUSPENDED); }
    else { m_pStateMach->Transit(STATE_IDLE); }
  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateDeleteRegion::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...

  return true;
}
