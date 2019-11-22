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
CStateDefineGraphic::CStateDefineGraphic(IStateMach* pStateMach,
                                     CPrinterContext* pContext, CState* pParent) :
  CState(pStateMach, pContext, pParent)
{
  CState *pChild;

  CREATE_CHILD_STATE(CStateDeleteGraphic);
  CREATE_CHILD_STATE(CStateAddGraphic);
}

/// <summary>Rerieves ID of this state.</summary>
int CStateDefineGraphic::GetID()
{
  return STATE_DEFINE_GRAPHIC;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateDefineGraphic::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateDefineGraphic::OnEnter]\n");
  }
}
