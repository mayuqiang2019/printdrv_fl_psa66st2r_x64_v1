#pragma once

#include "ComPort.h"
#include "message.h"
#include "filter.h"

#define MAX_RESEND_CNT  3
#define RUN_INTERVAL    10

/// <summary>Printer communication port.</summary>
class CPrinterPort : public CComPort
{
public:
  /// <value>Incoming message buffer.</value>
  CWkCirBuffer<BYTE> m_Buffer;

  /// <value>Critical section for <see cref="m_Buffer"/>.</value>
  wcl::CCriticalSection m_csBuffer;

  /// <value>Handshake type, can be "rtsx", "rts", or "x".</value>
  CWkString m_strHandshake;

public:
  CPrinterPort();
  ~CPrinterPort();

public:
  void Parse(const wchar_t* parameters);
  bool Open();
  void Poll();
  DWORD GetMsg(BYTE* buffer, DWORD bufferSize);

  int Write(BYTE* data, int dataSize);

  void Dump(MSXML2::IXMLDOMElement* pElem);
};

/// <summary>Printer context.</summary>
class CPrinterContext
{
public:
  /// <value>Printer communication port.</value>
  CPrinterPort m_Port;

  /// <value>True to output detail debug info, false otherwise.</value>
  bool m_bDebug;

  /// <value>True to enable dumping information to file when unexpected error
  /// occurs, false otherwise.</value>
  bool m_bErrDump;

  /// <value>Pointer to event observer.</value>
  print::IEvtObserver* m_pEvtObserver;

  /// <value>Last sent command (excluding status poll).</value>
  BYTE *m_pbyLastCmd;

  /// <value>Size of last sent command, <see cref="m_pbyLastCmd"/>.</value>
  DWORD m_dwLastCmdSize;

  /// <value>Size of memory allocated for <see cref="m_pbyLastCmt"/>.</value>
  DWORD m_dwLastCmdMemSize;

  /// <value>Last graphic definition.</value>
  print::CGraphic m_LastGraphic;

  /// <value>Last region definition.</value>
  print::CRegion m_LastRegion;

  /// <value>Last template definition.</value>
  print::CTemplate m_LastTemplate;

  /// <value>Printer software version information.</value>
  CWkString m_strSoftwareVer;

  /// <value>Printer status flags.</value>
  CStatus m_Status;

  /// <value>Print regions' default data.</value>
  CWkMapInt<CWkString> m_RegionDefData;

  /// <value>Defined template.</value>
  CWkMapInt<print::CTemplate> m_Template;

  /// <value>Handle to Run thread.</value>
  HANDLE m_hThread;

  /// <value>True to suspend after initiailization, false otherwise.</value>
  bool m_bInitSuspend;

  /// <value>Configured printer software version information.</value>
  CWkString m_strCfgSoftwareVer;

  /// <value>Configured currency.</value>
  CWkString m_strCfgCurrency;

protected:
  /// <value>True to stop Run thread, false otherwise.</value>
  bool m_bStopThread;

  /// <value>Critical section for <see cref="m_bStopThread"/>.</value>
  wcl::CCriticalSection m_csStopThread;

	/// <value>Pointer to current active filter, NULL if no active filter.</value>
	IJobFilter *m_pJobFilter;

	/// <value>Critical section for <see cref="m_pJobFilter"/>.</value>
	wcl::CCriticalSection m_csJobFilter;

	/// <value>Job filter for firmware GUR126001.</value>
	CJobFilterGUR126001 m_JFGUR126001;

	/// <value>Job filter for firmware GUR126003.</value>
	CJobFilterGUR126003 m_JFGUR126003;

	/// <value>Job filter for firmware GUR109000.</value>
	CJobFilterGUR109000 m_JFGUR109000;

	/// <value>Job filter for firmware GUREUR100.</value>
	CJobFilterGUREUR100 m_JFGUREUR100;

	/// <value>Job filter for firmware GUREUR101.</value>
	CJobFilterGUREUR101 m_JFGUREUR101;

	/// <value>Job filter for firmware GUREUR102.</value>
	CJobFilterGUREUR102 m_JFGUREUR102;

	/// <value>Job filter for firmware GUREUR200.</value>
	CJobFilterGUREUR200 m_JFGUREUR200;

	/// <value>Job filter for firmware GURTOR100.</value>
	CJobFilterGURTOR100 m_JFGURTOR100;

	/// <value>Job filter for firmware GURTOR101.</value>
	CJobFilterGURTOR101 m_JFGURTOR101;

	/// <value>Job filter for firmware GURUSA001.</value>
	CJobFilterGURUSA001 m_JFGURUSA001;

	/// <value>Job filter for firmware GURUSA003.</value>
	CJobFilterGURUSA003 m_JFGURUSA003;

	/// <value>Job filter for firmware GURNSW200.</value>
	CJobFilterGURNSW200 m_JFGURNSW200;

	/// <value>Job filter for firmware GURSNG103.</value>
	CJobFilterGURSNG103 m_JFGURSNG103;

public:
  CPrinterContext();
  ~CPrinterContext();

public:
  void Parse(const wchar_t* parameters);

  bool StopThread();
  void SetStopThread(bool stop);

  void Trace(const wchar_t* format, ...);
  void SendNUpdateLastCmd(CMsg& msg);
  void UpdateStatusNNotifyObserver(const CStatus& status);
  void UpdateSoftwareVer(const wchar_t* ver);

  IJobFilter* GetJobFilter();

  void Dump(MSXML2::IXMLDOMElement* pElem);

  static DWORD WINAPI _Run(LPVOID lpParameter);
};

/// <summary>Output string to debugger if <see cref="m_bDebug"/> is true.</summary>
/// <param name="format">Format of output string.</param>
inline void CPrinterContext::Trace(const wchar_t* format, ...)
{
  va_list args;

  if(m_bDebug)
  {
    va_start(args, format);
    TRACE_ARGS(format, args);
    va_end(args);
  } // if...
}

/// <summary>Updates software version.</summary>
/// <param name="ver">Software version.</param>
inline void CPrinterContext::UpdateSoftwareVer(const wchar_t* ver)
{
	m_strSoftwareVer = ver;

	// select job filter.
	m_csJobFilter.Enter();
	m_pJobFilter = NULL;
	if(m_strSoftwareVer == L"GUR126001") { m_pJobFilter = &m_JFGUR126001; }
	else if(m_strSoftwareVer == L"GUR126003") { m_pJobFilter = &m_JFGUR126003; }
	else if(m_strSoftwareVer == L"GUR109000") { m_pJobFilter = &m_JFGUR109000; }
  else if(m_strSoftwareVer == L"GUREUR100") { m_pJobFilter = &m_JFGUREUR100; }
  else if(m_strSoftwareVer == L"GUREUR101") { m_pJobFilter = &m_JFGUREUR101; }
  else if(m_strSoftwareVer == L"GUREUR102") { m_pJobFilter = &m_JFGUREUR102; }
  else if(m_strSoftwareVer == L"GUREUR200") { m_pJobFilter = &m_JFGUREUR200; }
  else if(m_strSoftwareVer == L"GURTOR100") { m_pJobFilter = &m_JFGURTOR100; }
  else if(m_strSoftwareVer == L"GURTOR101") { m_pJobFilter = &m_JFGURTOR101; }
  else if(m_strSoftwareVer == L"GURUSA001") { m_pJobFilter = &m_JFGURUSA001; }
  else if(m_strSoftwareVer == L"GURUSA003") { m_pJobFilter = &m_JFGURUSA003; }
	else if(m_strSoftwareVer == L"GURNSW200")	{ m_pJobFilter = &m_JFGURNSW200; }
	else if(m_strSoftwareVer == L"GURSNG103")	{ m_pJobFilter = &m_JFGURSNG103; }

	if (m_strSoftwareVer == L"GUREURGE2") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURMAC105") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURKORGE1") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURLKAGE1") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURMYSGE0") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURPHI101") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURSNG103") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURTHAGE0") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURTHBGE0") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURLTG000") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURUSAG13") { m_pJobFilter = &m_JFGUR126003; }
	else if (m_strSoftwareVer == L"GURSAFGE0") { m_pJobFilter = &m_JFGUR126003; }
	else 
		m_pJobFilter = &m_JFGUR126003;




	m_csJobFilter.Leave();
}

/// <summary>Retrieves currect active job filter.</summary>
/// <returns>Pointer to current active job filter,
/// NULL if not active job filter.</returns>
inline IJobFilter* CPrinterContext::GetJobFilter()
{
	IJobFilter *pTmp;

	m_csJobFilter.Enter();
	pTmp = m_pJobFilter;
	m_csJobFilter.Leave();

	return pTmp;
}

/// <summary>State machine interface.</summary>
class IStateMach
{
public:
  /// <summary>Transits to target state.</summary>
  /// <param name="target">Target state ID.</param>
  /// <exception cref="wcl::CArgumentException">If state referred by
  /// <paramref name="target"/> does not exist.</exception>
  virtual void Transit(int target) = 0;

  /// <summary>State execution.</summary>
  /// <param name="elapsed">Time elapsed since last run, in milliseconds.</param>
  virtual void Run(DWORD elapsed) = 0;

  ///	<summary>Dump object data to XML.</summary>
  ///	<param name='func'>Name of function which triggers the dump.</param>
  virtual void Dump(const wchar_t* func) {}
};

/// <summary>Parameters for Run thread.</summary>
struct SRunThreadParam
{
  CPrinterContext *m_pContext;
  IStateMach *m_pStateMach;
};

/// <summary>Printer state.</summary>
class CState : public print::IPrinter
{
protected:
  /// <value>Number of resend left before declaring failure.</value>
  int m_nResendCnt;

  /// <value>Pointer to owner state machine.</value>
  IStateMach *m_pStateMach;

  /// <value>Pointer to printer context.</value>
  CPrinterContext *m_pContext;

  /// <value>Pointer to parent state.</value>
  CState *m_pParent;

  /// <value>Children state.</value>
  CWkList<CState*> m_Children;

public:
  CState(IStateMach* pStateMach, CPrinterContext* pContext, CState* pParent);
  virtual ~CState();

public:
  /// <summary>Rerieves ID of this state.</summary>
  virtual int GetID() = 0;

  virtual CState* GetParent();

  virtual CState* IsAncenstorOf(int id);

  virtual void OnEnter(bool isTarget);
  virtual void OnLeave();
  virtual void Run(DWORD elapsed);
  virtual bool HandleResp(BYTE* resp, DWORD size);

  virtual void Dump(MSXML2::IXMLDOMElement* pElem);

  virtual void Init(const wchar_t* param, print::IEvtObserver* evtObserver);
  virtual void UnInit();
  virtual void Suspend();
  virtual void Resume();
  virtual void GetMetrics(print::CMetrics& metrics);
  virtual void SelfTest(bool clearNVM);
  virtual void RqGATReport();
  virtual void CalculateCRC(DWORD seed);
  virtual void DefineGraphic(const print::CGraphic& graphic);
  virtual void DefineRegion(const print::CRegion& region);
  virtual void DefineTemplate(const print::CTemplate& templ);
  virtual void Print(const print::CJob& job);
  virtual void FormFeed();
  virtual void GetFirmwareCurrency(CWkString& currency);

protected:
  virtual bool HandleRespCRC(BYTE* resp, DWORD size);
  virtual bool HandleRespStatus(BYTE* resp, DWORD size);
};

/// <summary>Implementation of <see cref="print::IPrinter"/> on PSA-66-ST2R.</summary>
class CPrinter : public IStateMach, public print::IPrinter
{
public:
  /// <value>Printer context.</value>
  CPrinterContext m_Context;

  /// <value>Top state.</value>
  CState *m_pStateTop;

  /// <value>Pointer to current state.</value>
  CState *m_pCurState;

  /// <value>Critical section for this object.</value>
  wcl::CCriticalSection m_csThis;

public:
  CPrinter();
  virtual ~CPrinter();

public:
  virtual void Transit(int target);

  virtual void Init(const wchar_t* param, print::IEvtObserver* evtObserver);
  virtual void UnInit();
  virtual void Suspend();
  virtual void Resume();
  virtual void GetMetrics(print::CMetrics& metrics);
  virtual void SelfTest(bool clearNVM);
  virtual void RqGATReport();
  virtual void CalculateCRC(DWORD seed);
  virtual void DefineGraphic(const print::CGraphic& graphic);
  virtual void DefineRegion(const print::CRegion& region);
  virtual void DefineTemplate(const print::CTemplate& templ);
  virtual void Print(const print::CJob& job);
  virtual void FormFeed();
  virtual void GetFirmwareCurrency(CWkString& currency);

  virtual void Run(DWORD elapsed);

  void Dump(MSXML2::IXMLDOMElement* pElem);
  void Dump(MSXML2::IXMLDOMElement* pElem, const wchar_t* func);
  virtual void Dump(const wchar_t* func);
};
