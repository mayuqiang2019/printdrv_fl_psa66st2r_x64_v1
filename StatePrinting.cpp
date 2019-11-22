#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStatePrinting::CStatePrinting(IStateMach* pStateMach, CPrinterContext* pContext,
                               CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent),
  m_bSuspendPending(false)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStatePrinting::GetID()
{
  return STATE_PRINTING;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStatePrinting::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStatePrinting::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);

  m_bSuspendPending = false;
  if(isTarget)
  {
    if(m_pContext->m_pEvtObserver != NULL)
    {
      m_pContext->m_pEvtObserver->OnPrinting();
    }
  } // if...
}

/// <summary>Suspends the printer.</summary>
/// <remarks>If invoked before initialization, printer will enter suspend mode
/// immediately after initialization (which is also the default behaviour),
/// but suspended event will not be issued as observer is not ready yet.
/// If invoked in the middle of printing, printer will enter suspended mode but
/// still continue to finish current print job.</remarks>
void CStatePrinting::Suspend()
{
  m_bSuspendPending = true;
  if(m_pContext->m_pEvtObserver != NULL)
  {
    m_pContext->m_pEvtObserver->OnSuspended();
  }
}

/// <summary>Resumes printer from suspend mode.</summary>
/// <remarks>If invoked before initialization, printer will enter resumed mode
/// immediately after initialization, but resumed event will not be issued
/// as observer is not ready yet.</remarks>
void CStatePrinting::Resume()
{
  if(m_bSuspendPending)
  {
    m_bSuspendPending = false;
    if(m_pContext->m_pEvtObserver != NULL)
    {
      m_pContext->m_pEvtObserver->OnResumed();
    }
  } // if...
}

#define CHECK_ERR(errFunc, evtFunc) if(msg.m_Status.errFunc())\
                                    {\
                                      if(m_pContext->m_pEvtObserver != NULL)\
                                      {\
                                        m_pContext->m_pEvtObserver->evtFunc(true);\
                                      }\
                                      target = STATE_SUSPENDED;\
                                    }

#define CHECK_EVT(errFunc, evtFunc) if(msg.m_Status.errFunc() &&\
                                      !m_pContext->m_Status.errFunc())\
                                    {\
                                      if(m_pContext->m_pEvtObserver != NULL)\
                                      {\
                                        m_pContext->m_pEvtObserver->evtFunc(true);\
                                      }\
                                    }\
                                    else if(!msg.m_Status.errFunc() &&\
                                      m_pContext->m_Status.errFunc())\
                                    {\
                                      if(m_pContext->m_pEvtObserver != NULL)\
                                      {\
                                        m_pContext->m_pEvtObserver->evtFunc(false);\
                                      }\
                                    }

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStatePrinting::HandleRespStatus(BYTE* resp, DWORD size)
{
  int target = STATE_PRINTING;
  CMsgRespStatus msg;

  msg.Parse(resp, size);

  try
  {

    if(CStatePollStatus::HandleRespStatus(resp, size)) { return true; }

    //**********************************************************
    // THESE ERRORS WILL FAILED THE PRINTING AND GO TO SUSPEND.
    if(msg.m_Status.ExtPowerLost())
    {
      if(m_pContext->m_pEvtObserver != NULL)
      {
        m_pContext->m_pEvtObserver->OnExtPowerLost();
      }
      target = STATE_SUSPENDED;
    } // if...

    CHECK_ERR(FirmwareErr, OnFirmwareErr);
    CHECK_ERR(NVMErr, OnNVMErr);
    CHECK_ERR(PrintHeadErr, OnPrintHeadErr);
    CHECK_ERR(TemperatureErr, OnTemperatureErr);
    CHECK_ERR(GeneralErr, OnGeneralErr);
    CHECK_ERR(PrintHeadOpened, OnPrintHead);
    CHECK_ERR(PaperJam, OnPaperJam);

    if(!msg.m_Status.TopOfForm())
    {
      if(m_pContext->m_pEvtObserver != NULL)
      {
        m_pContext->m_pEvtObserver->OnTopOfForm(true);
      }
      target = STATE_SUSPENDED;
    } // if...

    if(target == STATE_SUSPENDED)
    {
      // error causing print failed, no need to check for the rest of the status.
      m_pContext->m_Status = msg.m_Status;
      m_pStateMach->Transit(STATE_SUSPENDED);
      return true;
    }
    // END OF PRINTING FAILING ERRORS
    //**********************************

    //********************************************
    // THESE ERRORS WILL NOT AFFECT PRINTING.
    CHECK_EVT(ChassisOpened, OnChassis);
    CHECK_EVT(PaperLow, OnPaperLow);
    CHECK_EVT(PaperEmpty, OnPaperEmpty);
    // END OF PRINTING SAFE ERRORS.
    //*********************************

    //*****************************************************
    // RETRY IF PRINTER COMPLAIN ABOUT COMMAND SYNTAX.
    if(m_bPolled && msg.m_Status.m_bCmdErr)
    {
      m_pContext->m_Status = msg.m_Status;
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
    //**********************

    //**********************
    // PRINTING FAILED.
    if(msg.m_Status.m_bRegionDataErr)
    {
      if(m_pContext->m_pEvtObserver != NULL)
      {
        m_pContext->m_pEvtObserver->OnPrintFailed(
          print::IPrintObserver::PRINT_ERR_DATATYPE_MISMATCH);
      }

      m_pContext->UpdateSoftwareVer(msg.m_strSoftwareVer);
      m_pContext->m_Status = msg.m_Status;
      if(m_bSuspendPending || msg.m_Status.ShouldSuspend())
      {
        m_pStateMach->Transit(STATE_SUSPENDED);
      }
      else
      {
        m_pStateMach->Transit(STATE_IDLE);
      } // if...else...
      return true;
    } // if...
    // END OF PRINTING FAILED.
    //**************************

    //*****************************
    // PRINTING COMPLETED.
    // busy flag must be set at least once to indicate that following responses
    // came after printer received the printing command.
    if(m_bPolled && !msg.m_Status.m_bBusy)
    {
      // printing completed.
      if(m_pContext->m_pEvtObserver != NULL)
      {
        m_pContext->m_pEvtObserver->OnPrintCompleted();
      }

      if(m_bSuspendPending || msg.m_Status.ShouldSuspend())
      {
        // go to suspend due to previous command or event.
        target = STATE_SUSPENDED;
      }
      else
      {
        target = STATE_IDLE;
      }
    } // if...

    m_pContext->m_Status = msg.m_Status;
    if(target != STATE_PRINTING) { m_pStateMach->Transit(target); }
    // END OF PRINTING COMPLETED.
    //*****************************

  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStatePrinting::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...

  return true;
}
