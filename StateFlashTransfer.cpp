#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateFlashTransfer::CStateFlashTransfer(IStateMach* pStateMach,
										 CPrinterContext* pContext, CState* pParent) :
	CStateInitialized(pStateMach, pContext, pParent)
{
	m_Timer.SetExpiry(2000);
}

/// <summary>Rerieves ID of this state.</summary>
int CStateFlashTransfer::GetID()
{
	return STATE_FLASH_TRANSFER;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateFlashTransfer::OnEnter(bool isTarget)
{
	if(isTarget)
	{
		m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateFlashTransfer::OnEnter]\n");

		CMsgMgr msgMgr;
		CMsgFlashTransfer msg(msgMgr.TemplID2PageID(
			m_pContext->m_LastTemplate.m_nsID));
		m_pContext->SendNUpdateLastCmd(msg);

		m_Timer.Reset();
	}
}

/// <summary>State execution.<summary>
/// <param name="elapsed">Time elapsed since last run, in milliseconds.</param>
void CStateFlashTransfer::Run(DWORD elapsed)
{
	m_Timer.Elapsed(elapsed);
	if(m_Timer.IsExpired())
	{
        m_pStateMach->Transit(STATE_COMPLETE_FLASH_TRANSFER);
	} // if...
}
