#include "stdafx.h"
#include "state.h"

/// <summary>Constructor.</summary>
/// <param name="pStateMach">Pointer to owner state machine.</param>
/// <param name="pContext">Pointer to printer context.</param>
/// <param name="pParent">Pointer to parent state, NULL for no parent state.</param>
/// <exception cref="wcl::CArgumentNullException">If <paramref name="pStateMach"/>
/// or <paramref name="pContext"/> is NULL.</exception>
CStateIdle::CStateIdle(IStateMach* pStateMach, CPrinterContext* pContext,
                       CState* pParent) :
  CStatePollStatus(pStateMach, pContext, pParent)
{
}

/// <summary>Rerieves ID of this state.</summary>
int CStateIdle::GetID()
{
  return STATE_IDLE;
}

/// <summary>Handles state entered event.</summary>
/// <param name="isTarget">True if this state is the final target of transition,
/// false otherwise.</param>
void CStateIdle::OnEnter(bool isTarget)
{
  if(isTarget)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateIdle::OnEnter]\n");
  }
  CStatePollStatus::OnEnter(isTarget);
}

/// <summary>Suspends the printer.</summary>
/// <remarks>If invoked before initialization, printer will enter suspend mode
/// immediately after initialization (which is also the default behaviour),
/// but suspended event will not be issued as observer is not ready yet.
/// If invoked in the middle of printing, printer will enter suspended mode but
/// still continue to finish current print job.</remarks>
void CStateIdle::Suspend()
{
  m_pStateMach->Transit(STATE_SUSPENDED);
  if(m_pContext->m_pEvtObserver != NULL)
  {
    m_pContext->m_pEvtObserver->OnSuspended();
  } // if...
}

/// <summary>Defines graphic.</summary>
/// <param name="graphic">Graphic.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="graphic"/> is
/// invalid.</exception>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>If graphic of same identifier already exists, new graphic will
/// replace current one. If identifier being replaced is for a predefined
/// graphic, the new graphic will still replace the predefined graphic, but the
/// original predefined graphic will be restored after power cycle reset.</remarks>
void CStateIdle::DefineGraphic(const print::CGraphic& graphic)
{
  CMsgLibManage msg;

  msg.m_bDefine = false;
  msg.m_pGraphic = &graphic;

  m_pContext->m_LastGraphic = graphic;

  try
  {
    m_pContext->SendNUpdateLastCmd(msg);
    m_pStateMach->Transit(STATE_DELETE_GRAPHIC);
  }
  catch(wcl::CArgumentException& e)
  {
    m_pContext->m_pEvtObserver->OnDefineGraphicFailed(print::IObserver::GRAPH_ERR_ID);
  }
  catch(CCommException& e)
  {
  //  m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateIdle::DefineGraphic] disconnected due to CCommException, port:%i, msg:%s, err:%u.\n",
 //     e.GetPort(), e.GetMsg(), e.GetSysErrCode);
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...
}

/// <summary>Defines printable region.</summary>
/// <param name="region">Printable region.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="region"/> is
/// invalid.</exception>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Only region ID between 100 ~ 999 can be defined. If a region ID
/// already defined, new region will replace the current region definition.</remarks>
void CStateIdle::DefineRegion(const print::CRegion& region)
{
  CMsgDefineRegion msg;

  msg.m_bDefine = false;
  msg.m_pRegion = &region;

  m_pContext->m_LastRegion = region;

  try
  {
    m_pContext->SendNUpdateLastCmd(msg);
    m_pStateMach->Transit(STATE_DELETE_REGION);
  }
  catch(wcl::CArgumentException& e)
  {
    m_pContext->m_pEvtObserver->OnDefineRegionFailed(print::IObserver::REGION_ERR_ID);
  }
  catch(CCommException& e)
  {
   // m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateIdle::DefineRegion] disconnected due to CCommException, port:%i, msg:%s, err:%u.\n",
   //   e.GetPort(), e.GetMsg(), e.GetSysErrCode);
    m_pStateMach->Transit(STATE_DISCONNECTED);
  }
}

/// <summary>Defines printable template.</summary>
/// <param name="templ">Printable template.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="template"/>
/// is invalid.</exception>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CStateIdle::DefineTemplate(const print::CTemplate& templ)
{
  CMsgDefineTempl msg;

  msg.m_bDefine = false;
  msg.m_pTemplate = &templ;

  m_pContext->m_LastTemplate = templ;

  try
  {
    m_pContext->SendNUpdateLastCmd(msg);
    m_pStateMach->Transit(STATE_DELETE_TEMPL);
  }
  catch(wcl::CArgumentException& e)
  {
    m_pContext->m_pEvtObserver->OnDefineTemplateFailed(print::IObserver::TEMPL_ERR_ID);
  }
  catch(CCommException& e)
  {
   // m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateIdle::DefineTemplate] disconnected due to CCommException, port:%i, msg:%s, err:%u.\n",
  //    e.GetPort(), e.GetMsg(), e.GetSysErrCode);
    m_pStateMach->Transit(STATE_DISCONNECTED);
  }
}

/// <summary>Prints a job using specified template.</summary>
/// <param name="job">Print job.</param>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
/// <remarks>Printer will ignore this call when it is in suspend mode, or when
/// following conditions are present: print head open, paper jam, paper empty,
/// top of form.</remarks>
void CStateIdle::Print(const print::CJob& job)
{
  CMsgMgr msgMgr;
  BYTE templateID, regionID;
  print::CTemplate templ;
  int i, cnt;
  POS posJob, posTempl;
  CWkString defData;
  CMsgPrint msg;
  print::CJob ppJob = job;
  IJobFilter *pJobFilter;

  // pre-process job to fill in default data.
  templateID = msgMgr.TemplID2Drv(job.m_nsTemplateID);
  if(m_pContext->m_Template.Get(templateID, templ))
  {
    cnt = __min(job.GetCount(), templ.GetCount());
    for(i = 0;i < cnt;i++)
    {
      posJob = ppJob.FindIndex(i);
      posTempl = templ.FindIndex(i);
      regionID = msgMgr.RegionID2Drv( templ.GetAt(posTempl) );

      if( (job.GetAt(posJob).m_strData.GetLength() <= 0) &&
        m_pContext->m_RegionDefData.Get(regionID, defData) )
      {
        ppJob.GetAt(posJob).m_strData = defData;
      }
    } // for...
  } // if...

  // apply job filter.
  pJobFilter = m_pContext->GetJobFilter();
  if(pJobFilter && pJobFilter->NeedTransform(ppJob))
  {
	  ppJob = pJobFilter->Transform(ppJob);
  }
  msg.m_pJob = &ppJob;

  try
  {
    m_pContext->SendNUpdateLastCmd(msg);
    m_pStateMach->Transit(STATE_PRINTING);
  }
  catch(CCommException& e)
  {
   // m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateIdle::Print] disconnected due to CCommException, port:%i, msg:%s, err:%u.\n",
   //   e.GetPort(), e.GetMsg(), e.GetSysErrCode);
    m_pStateMach->Transit(STATE_DISCONNECTED);
  }
}

/// <summary>Feeds a blank ticket.</summary>
/// <exception cref="wcl::CInvalidOperationException">If invoked before printer
/// is initialized.</exception>
void CStateIdle::FormFeed()
{
  CMsgFeed msg;

  try
  {
    m_pContext->SendNUpdateLastCmd(msg);
  }
  catch(CCommException& e)
  {
    //m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateIdle::FormFeed] disconnected due to CCommException, port:%i, msg:%s, err:%u.\n",
    //  e.GetPort(), e.GetMsg(), e.GetSysErrCode);
    m_pStateMach->Transit(STATE_DISCONNECTED);
  }
}

/// <summary>Handles printer status response.</summary>
/// <param name="resp">Printer status response.</param>
/// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
/// <returns>True if the response was consumed (thus should not be used for
/// further process anymore), false otherwise.</returns>
bool CStateIdle::HandleRespStatus(BYTE* resp, DWORD size)
{
  CMsgRespStatus msg;

  msg.Parse(resp, size);

  try
  {

    if(CStatePollStatus::HandleRespStatus(resp, size)) { return true; }
    m_pContext->UpdateStatusNNotifyObserver(msg.m_Status);

    if(msg.m_Status.ShouldSuspend()) { m_pStateMach->Transit(STATE_SUSPENDED); }

  }
  catch(CCommException& e)
  {
    m_pContext->Trace(L"[printdrv_fl_psa66st2r][CStateIdle::Run] CCommException caught, Port:%i, message:%s, error code:%u.\n",
      e.GetPort(), e.GetMsg(), e.GetSysErrCode());
    m_pStateMach->Transit(STATE_DISCONNECTED);
  } // try...catch...

  return true;
}
