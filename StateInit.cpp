#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateInit::CStateInit(IStateMach* pStateMach, CPrinterContext* pContext,
                       CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateInit::GetID()
{
  return STATE_INIT;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateInit::OnEnter(bool isTarget)
{
  DWORD len;
  BYTE buffer[512]; // 512 bytes should be large enough for all printer response.
  CMsgStatus msg;

  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateInit::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);

  try
  {

    len = msg.Build(buffer, 512);
    m_pContext->m_Port.Write(buffer, len);

  }
  catch(CCommException&)
  {
    // no need to do anything here as it will eventually timeout by alive timer.
  }
}

/// <summary>Suspends the printer.</summary>
/// <remarks>If invoked before initialization, printer will enter suspend mode
/// immediately after initialization (which is also the default behaviour),
/// but suspended event will not be issued as observer is not ready yet.
/// If invoked in the middle of printing, printer will enter suspended mode but
/// still continue to finish current print job.</remarks>
void CStateInit::Suspend()
{
  m_pContext->m_bInitSuspend = true;
}

/// <summary>Resumes printer from suspend mode.</summary>
/// <remarks>If invoked before initialization, printer will enter resumed mode
/// immediately after initialization, but resumed event will not be issued
/// as observer is not ready yet.</remarks>
void CStateInit::Resume()
{
  m_pContext->m_bInitSuspend = false;
}

#define CHECK_EVT(errFunc, evtFunc) if(msg.m_Status.errFunc())\
                                    {\
                                      if(m_pContext->m_pEvtObserver != NULL)\
                                      {\
                                        m_pContext->m_pEvtObserver->evtFunc(true);\
                                      }\
                                    }\

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateInit::HandleRespStatus(BYTE* resp, DWORD size)
{
  CMsgRespStatus msg;

  msg.Parse(resp, size);

  try
  {

    m_pContext->m_Status = msg.m_Status;
    if(CStatePollStatus::HandleRespStatus(resp, size)) { return true; }

    if(msg.m_Status.m_bCmdErr)
    {
      m_pStateMach->Transit(STATE_DISCONNECTED);
    }
    else
    {
      //*********************************
      // ANNOUNCE ERRORS TO OBSERVER.
      if(m_pContext->m_Status.ExtPowerLost())
      {
        if(m_pContext->m_pEvtObserver != NULL)
        {
          m_pContext->m_pEvtObserver->OnExtPowerLost();
        }
      }

      CHECK_EVT(FirmwareErr, OnFirmwareErr);
      CHECK_EVT(NVMErr, OnNVMErr);
      CHECK_EVT(PrintHeadErr, OnPrintHeadErr);
      CHECK_EVT(TemperatureErr, OnTemperatureErr);
      CHECK_EVT(GeneralErr, OnGeneralErr);
      CHECK_EVT(PrintHeadOpened, OnPrintHead);
      CHECK_EVT(PaperJam, OnPaperJam);
      CHECK_EVT(PaperEmpty, OnPaperEmpty);

      if(!m_pContext->m_Status.TopOfForm())
      {
        if(m_pContext->m_pEvtObserver != NULL)
        {
          m_pContext->m_pEvtObserver->OnTopOfForm(true);
        }
      }

      CHECK_EVT(ChassisOpened, OnChassis);
      CHECK_EVT(PaperLow, OnPaperLow);
      // END OF ERROR ANNOUNCEMENTS.
      //********************************   

      if(m_pContext->m_pEvtObserver != NULL)
      {
        m_pContext->m_pEvtObserver->OnReady();
      } // if...
      m_pStateMach->Transit(STATE_READY);
    } // if...else...

  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateInit::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...

  return true;
}
