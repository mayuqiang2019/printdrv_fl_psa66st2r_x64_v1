#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateCompleteFlashTransfer::CStateCompleteFlashTransfer(IStateMach* pStateMach,
														 CPrinterContext* pContext,
														 CState* pParent) :
	CStateInitialized(pStateMach, pContext, pParent)
{
	m_PollStatusTimer.SetExpiry(300);
	m_DisconnectTimer.SetExpiry(2000);
}

/// <summary>Rerieves ID of this state.</summary>
int CStateCompleteFlashTransfer::GetID()
{
	return STATE_COMPLETE_FLASH_TRANSFER;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateCompleteFlashTransfer::OnEnter(bool isTarget)
{
	if(isTarget)
	{
		m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateCompleteFlashTransfer::OnEnter]\n");

		m_PollStatusTimer.Reset();
		m_DisconnectTimer.Reset();
	} // if...
}

/// <summary>State execution.<summary>
/// <param name="elapsed">Time elapsed since last run, in milliseconds.</param>
void CStateCompleteFlashTransfer::Run(DWORD elapsed)
{
	DWORD len;
	BYTE buffer[512]; // 512 bytes should be large enough for all printer response.
	CMsgStatus msg;

	m_PollStatusTimer.Elapsed(elapsed);
	m_DisconnectTimer.Elapsed(elapsed);

	try
	{
		m_pContext->m_Port.Poll();
		len = m_pContext->m_Port.GetMsg(buffer, 512);

		if((len <= 0) || (len > 512) || !HandleResp(buffer, len))
		{
			if(m_PollStatusTimer.IsExpired())
			{
				if(m_DisconnectTimer.IsExpired())
				{
					m_pStateMach->Transit(STATE_DISCONNECTED);
				}
				else
				{
					m_PollStatusTimer.Reset();
					len = msg.Build(buffer, 512);
					m_pContext->m_Port.Write(buffer, len);
				}
			} // if...else...
		}
	}
	catch(CCommException&) {}
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateCompleteFlashTransfer::HandleRespStatus(BYTE* resp, DWORD size)
{
	CMsgRespStatus msg;
	BYTE templateID;
	CMsgMgr msgMgr;

	msg.Parse(resp, size);
	m_pContext->m_Status = msg.m_Status;

	templateID = msgMgr.TemplID2Drv(m_pContext->m_LastTemplate.m_nsID);
	m_pContext->m_Template.Set(templateID, m_pContext->m_LastTemplate);
	if(m_pContext->m_pEvtObserver != NULL)
	{
		m_pContext->m_pEvtObserver->OnDefineTemplateSuccess();
	}

	if(msg.m_Status.ShouldSuspend()) { m_pStateMach->Transit(STATE_SUSPENDED); }
	else { m_pStateMach->Transit(STATE_IDLE); }

	CStateInitialized::HandleRespStatus(resp, size);

	return true;
}
