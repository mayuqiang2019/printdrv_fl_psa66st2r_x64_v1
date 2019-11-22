#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateInitialized::CStateInitialized(IStateMach* pStateMach,
                                   CPrinterContext* pContext, CState* pParent) :
  CState(pStateMach, pContext, pParent)
{
}

/// <summary>Un-initializes printer.</summary>
void CStateInitialized::UnInit()
{
  m_pStateMach->Transit(STATE_UNINIT);
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateInitialized::HandleRespStatus(BYTE* resp, DWORD size)
{
  CMsgRespStatus msg;
  CMsgClearErr msgClearErr;
  BYTE buffer[512]; // should be large enough for clear error status command.
  DWORD len;

  msg.Parse(resp, size);

  try
  {
    m_pContext->UpdateSoftwareVer(msg.m_strSoftwareVer);

    //********************************************************
    // RESET ERRORS TO INDICATE THAT THEY HAD BEEN PROCESSED.
    if(msg.m_Status.NeedClearErr())
    {
      len = msgClearErr.Build(buffer, 512);
      m_pContext->m_Port.Write(buffer, len);
    }
    // END OF ERRORS RESET.
    //***************************

  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateInit::HandleRespStatus] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);

    return true;
  } // try...catch...

  return false;
}
