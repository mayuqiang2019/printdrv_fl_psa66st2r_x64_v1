#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStatePollStatus::CStatePollStatus(IStateMach* pStateMach,
                                   CPrinterContext* pContext, CState* pParent) :
  CStateInitialized(pStateMach, pContext, pParent)
{
  m_AliveTimer.SetExpiry(2000);
  m_PollStatusTimer.SetExpiry(300);
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStatePollStatus::OnEnter(bool isTarget)
{
  CState::OnEnter(isTarget);

  m_AliveTimer.Reset();
  m_PollStatusTimer.Reset();
  m_bPolled = false;
}

/// <summary>State execution.<summary>
/// <param name="elapsed">Time elapsed since last run, in milliseconds.</param>
void CStatePollStatus::Run(DWORD elapsed)
{
  DWORD len;
  BYTE buffer[512]; // 512 bytes should be large enough for all printer response.
  CMsgStatus msg;

  m_AliveTimer.Elapsed(elapsed);
  m_PollStatusTimer.Elapsed(elapsed);

  try
  {
    m_pContext->m_Port.Poll();
    len = m_pContext->m_Port.GetMsg(buffer, 512);

    if((len > 0) && (len < 512))
    {
      m_PollStatusTimer.Reset();
      m_AliveTimer.Reset();

      /*{
          m_pContext->Trace(L"[printdrv_fl_psa66st2r] RECV ");
          for(DWORD i = 0;i < len;i++)
          {
              m_pContext->Trace(L"%c", buffer[i]);
          }
          m_pContext->Trace(L"\n");
      }*/
      HandleResp(buffer, len);
    }
    else if(m_AliveTimer.IsExpired())
    {
      m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStatePollStatus::Run] disconnected due to alive timer expired.\n");
      m_pStateMach->Transit(STATE_DISCONNECTED);
    }
    else if(m_PollStatusTimer.IsExpired())
    {
      m_PollStatusTimer.Reset();
      len = msg.Build(buffer, 512);
      m_pContext->m_Port.Write(buffer, len);
      m_bPolled = true;
    } // if...else...
  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStatePollStatus::Run] CCommException caught, Port:%i, message:%s, error code:%u.\n",
        e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  }
  catch(...)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStatePollStatus::Run] unexpected exception caught.\n");
    throw;
  } // try...catch...
}
