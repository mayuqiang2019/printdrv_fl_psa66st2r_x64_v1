#include "stdafx.h"
#include "state.h"

#define CREATE_CHILD_STATE(state)\
  pChild = new state(pStateMach, pContext, this);\
  if(pChild == NULL) { throw wcl::COutOfMemoryException(); }\
  m_Children.AddTail(pChild)

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateReady::CStateReady(IStateMach* pStateMach, CPrinterContext* pContext,
                                CState* pParent) :
  CState(pStateMach, pContext, pParent)
{
  CState *pChild;

  CREATE_CHILD_STATE(CStateSuspended);
  CREATE_CHILD_STATE(CStateGATReport);
  CREATE_CHILD_STATE(CStateCRC);
  CREATE_CHILD_STATE(CStateIdle);
  CREATE_CHILD_STATE(CStateDefineGraphic);
  CREATE_CHILD_STATE(CStateDefineRegion);
  CREATE_CHILD_STATE(CStateDefineTempl);
  CREATE_CHILD_STATE(CStatePrinting);
}

/// <summary>Rerieves ID of this state.</summary>
int CStateReady::GetID()
{
  return STATE_READY;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateReady::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateReady::OnEnter]\n");
  }

  if(isTarget)
  {
    if(m_pContext->m_Status.ShouldSuspend() || m_pContext->m_bInitSuspend)
    {
      m_pStateMach->Transit(STATE_SUSPENDED);
    }
    else
    {
      m_pContext->m_bInitSuspend = true; // next time still default suspend.
      m_pStateMach->Transit(STATE_IDLE);
    }
  } // if...
}
