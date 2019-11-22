#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateDisconnected::CStateDisconnected(IStateMach* pStateMach,
                                       CPrinterContext* pContext,
                                       CState* pParent) :
  CStateInitialized(pStateMach, pContext, pParent)
{
  m_PollStatusTimer.SetExpiry(300);
}

/// <summary>Rerieves ID of this state.</summary>
int CStateDisconnected::GetID()
{
  return STATE_DISCONNECTED;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateDisconnected::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateDisconnected::OnEnter]\n");
  }

  m_nResendCnt = 0;
  m_PollStatusTimer.Reset();

  if(m_pContext->m_pEvtObserver != NULL)
  {
    m_pContext->m_pEvtObserver->OnDisconnected();
  }
}

/// <summary>State execution.<summary>
/// <param name="elapsed">Time elapsed since last run, in milliseconds.</param>
void CStateDisconnected::Run(DWORD elapsed)
{
  DWORD len;
  BYTE buffer[512]; // 512 bytes should be large enough for all printer response.
  CMsgStatus msg;

  m_PollStatusTimer.Elapsed(elapsed);

  try
  {
    m_pContext->m_Port.Poll();
    len = m_pContext->m_Port.GetMsg(buffer, 512);

    if((len > 0) && (len < 512))
    {
      HandleResp(buffer, len);
      if(!m_pContext->m_Status.m_bCmdErr)
      {
        if(m_pContext->m_pEvtObserver != NULL)
        {
          m_pContext->m_pEvtObserver->OnConnected();
        }
        m_pStateMach->Transit(STATE_INIT);
      }
    }
    else if(m_PollStatusTimer.IsExpired())
    {
      m_PollStatusTimer.Reset();
      len = msg.Build(buffer, 512);
      m_pContext->m_Port.Write(buffer, len);
    } // if...else...
  }
  catch(CCommException&) {}
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateDisconnected::HandleRespStatus(BYTE* resp, DWORD size)
{
  CMsgRespStatus msg;

  msg.Parse(resp, size);
  m_pContext->m_Status = msg.m_Status;

  return CStateInitialized::HandleRespStatus(resp, size);
}
