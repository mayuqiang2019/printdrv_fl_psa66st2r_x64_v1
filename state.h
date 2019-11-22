#pragma once

#include "WkTime.h"
#include "printer.h"

/// <summary>Top state.</summary>
class CStateTop : public CState
{
public:
  CStateTop(IStateMach* pStateMach, CPrinterContext* pContext, CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);
};

/// <summary>Un-initialized state.</summary>
class CStateUnInit : public CState
{
protected:
  /// <value>Storage for thread parameters.</value>
  /// <remarks>As parameters passed to thread must have life-span longer than
  /// the thread, and unlikely can be a local.</remarks>
  SRunThreadParam m_ThreadParam;

public:
  CStateUnInit(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

  virtual void Init(const wchar_t* param, print::IEvtObserver* evtObserver);
  virtual void Suspend();
  virtual void Resume();
};

/// <summary>Initialized state.</summary>
/// <remarks>This is not an concrete state, instead it serves as the case
/// class of state which is initialized.</remarks>
class CStateInitialized : public CState
{
public:
  CStateInitialized(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual void UnInit();

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Status polling state.</summary>
/// <remarks>This is not an concrete state, instead it serves as the base
/// class of states which require status polling.</remarks>
class CStatePollStatus : public CStateInitialized
{
public:
  /// <value>Printer status polling interval.</value>
  CWkTimer m_PollStatusTimer;

  /// <value>Printer alive timer.</value>
  CWkTimer m_AliveTimer;

  /// <value>True if a poll status command has been sent, false otherwise.</value>
  bool m_bPolled;

public:
  CStatePollStatus(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual void OnEnter(bool isTarget);
  virtual void Run(DWORD elapsed);
};

/// <summary>Initializing state.</summary>
class CStateInit : public CStatePollStatus
{
public:
  CStateInit(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

  virtual void Suspend();
  virtual void Resume();

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Disconnected state.</summary>
class CStateDisconnected : public CStateInitialized
{
public:
  /// <value>Printer status polling interval.</value>
  CWkTimer m_PollStatusTimer;

public:
  CStateDisconnected(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);
  virtual void Run(DWORD elapsed);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Ready state.</summary>
class CStateReady : public CState
{
public:
  CStateReady(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);
};

/// <summary>Suspended state.</summary>
class CStateSuspended : public CStatePollStatus
{
public:
  CStateSuspended(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

  virtual void Suspend();
  virtual void Resume();
  virtual void SelfTest(bool clearNVM);
  virtual void RqGATReport();
  virtual void CalculateCRC(DWORD seed);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Requesting GAT report state.</summary>
class CStateGATReport : public CStatePollStatus
{
public:
  CStateGATReport(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Calculating CRC state.</summary>
class CStateCRC : public CStatePollStatus
{
public:
  CStateCRC(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

protected:
  virtual bool HandleRespCRC(BYTE* resp, DWORD size);
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Idling state.</summary>
class CStateIdle : public CStatePollStatus
{
public:
  CStateIdle(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

  virtual void Suspend();
  virtual void DefineGraphic(const print::CGraphic& graphic);
  virtual void DefineRegion(const print::CRegion& region);
  virtual void DefineTemplate(const print::CTemplate& templ);
  virtual void Print(const print::CJob& job);
  virtual void FormFeed();

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Define graphic state.</summary>
class CStateDefineGraphic : public CState
{
public:
  CStateDefineGraphic(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);
};

/// <summary>Delete graphic state.</summary>
class CStateDeleteGraphic : public CStatePollStatus
{
public:
  CStateDeleteGraphic(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Add graphic state.</summary>
class CStateAddGraphic : public CStatePollStatus
{
public:
  CStateAddGraphic(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Define region state.</summary>
class CStateDefineRegion : public CState
{
public:
  CStateDefineRegion(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);
};

/// <summary>Delete region state.</summary>
class CStateDeleteRegion : public CStatePollStatus
{
public:
  CStateDeleteRegion(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Add region state.</summary>
class CStateAddRegion : public CStatePollStatus
{
public:
  CStateAddRegion(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Define template state.</summary>
class CStateDefineTempl : public CState
{
public:
  CStateDefineTempl(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);
};

/// <summary>Delete template state.</summary>
class CStateDeleteTempl : public CStatePollStatus
{
public:
  CStateDeleteTempl(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Add template state.</summary>
class CStateAddTempl : public CStatePollStatus
{
public:
  CStateAddTempl(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Flash transfer state.</summary>
class CStateFlashTransfer : public CStateInitialized
{
protected:
	/// <value>Timer to wait for flash transfer to complete.</value>
	CWkTimer m_Timer;

public:
	CStateFlashTransfer(IStateMach* pStateMach, CPrinterContext* pContext,
		CState* pParent);

public:
	virtual int GetID();
	virtual void OnEnter(bool isTarget);
	virtual void Run(DWORD elapsed);
};

/// <summary>Completing flash transfer state.</summary>
class CStateCompleteFlashTransfer : public CStateInitialized
{
protected:
  /// <value>Printer status polling interval.</value>
  CWkTimer m_PollStatusTimer;

  /// <value>Timer to determined whether printer is disconnected.</value>
  CWkTimer m_DisconnectTimer;

public:
	CStateCompleteFlashTransfer(IStateMach* pStateMach, CPrinterContext* pContext,
		CState* pParent);

public:
	virtual int GetID();
	virtual void OnEnter(bool isTarget);
	virtual void Run(DWORD elapsed);

protected:
	virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Printing state.</summary>
class CStatePrinting : public CStatePollStatus
{
public:
  /// <value>True if suspend command pending, false otherwise.</value>
  bool m_bSuspendPending;

public:
  CStatePrinting(IStateMach* pStateMach, CPrinterContext* pContext,
    CState* pParent);

public:
  virtual int GetID();

  virtual void OnEnter(bool isTarget);

  virtual void Suspend();
  virtual void Resume();

protected:
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};
