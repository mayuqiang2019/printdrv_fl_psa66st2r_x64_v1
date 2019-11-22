=============================================================================
    Admendment History
=============================================================================

/////////////////////////////////////////////////////////////////////////////
v1.0.0.34, Tan Jui Ken,
- Set log file to application drive.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.33, Tan Jui Ken,
- Added using values from parameter field to check firmware currency if 
firmware version is not included. This will reduce the need for re-submission.
- Removed v1.0.0.32.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.31, Wu Xiao Qin,
- Added support for firmware GURTOR101 (HKD).

/////////////////////////////////////////////////////////////////////////////
v1.0.0.30, Tan Jui Ken,
- Added support for firmware GURSNG103 (SGD).

/////////////////////////////////////////////////////////////////////////////
v1.0.0.29, Tan Jui Ken,
- Added support for firmware GURNSW200.
- Added new template N for NSW which uses template Id 78.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.28, Tan Jui Ken,
- Added support for firmware GURUSA003.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.27, Koh Hwee Miin,
- Added CPrinterContext::m_dwLastCmdMemSize to implement delayed
  re-allocation on CPrinterContext::m_abyLastCmd, to reduce memory
  fragmentation.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.26, Koh Hwee Miin,
- Re-compiled due to updates in common.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.25, Koh Hwee Miin,
- Added support for firmware GURUSA001.
- Append string "Ticket Void After: " to the front of expiry when printing
  template ID 9, in CJobFilterGUR126003::TransformTempl_9().

/////////////////////////////////////////////////////////////////////////////
v1.0.0.24, Koh Hwee Miin,
- In CJobFilterGUR126003, transform all expiry of value "9999 days" to
  empty string to print output of "never expires".
- Added support for firmware GURTOR100.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.23, Koh Hwee Miin,
- Fixed incorrectly report printing completion when receive printer status
  before printing command is processed by printer, by adding
  CStatePollStatus::m_bPolled.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.22, Koh Hwee Miin,
- Added support for firmware GUREUR200.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.21, Koh Hwee Miin,
- Added support for firmware GUREUR102.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.20, Koh Hwee Miin,
- Added support for firmware GUREUR101.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.19, Koh Hwee Miin,
- Added support for firmware GUREUR100.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.18, Koh Hwee Miin,
- Added IJobFilter, CJobFilterGUR126001, CJobFilterGUR126003,
  CJobFilterGUR109000.
- Added CPrinterContext::m_pJobFilter, CPrinterContext::m_csJobFilter,
  CPrinterContext::m_JFGUR126001, CPrinterContext::m_JFGUR126003,
  CPrinterContext::m_JFGUR109000, CPrinterContext::UpdateSoftwareVer(),
  CPrinterContext::GetJobFilter().

/////////////////////////////////////////////////////////////////////////////
v1.0.0.17, Koh Hwee Miin,
- Added CMsgMgr::TemplID2PageID(), CMsgMgr::IsUserDefinedTempl(),
  CMsgMgr::TemplID2PageIDPrint().
- Added CMsgFlashTransfer.
- Added CStateFlashTransfer, CStateCompleteFlashTransfer.
- Updated CStateAddTempl::HandleRespStatus() to transit to
  CStateFlashTransfer when defining user-defined templates.
- Updated CMsgPrint::Build() to support memory page when printing using
  user-defined templates.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.16, Koh Hwee Miin,
- Updated CState::GetFirmwareCurrency() to support firmware "GUR109000" for
  PHP.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.15, Koh Hwee Miin,
- Removed debug information in release binary.
- Reset timestamps in release binary.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.14, Koh Hwee Miin,
- Fixed unable to re-connect after disconnected due to corrupted message
  received from printer, by updating CPrinterPort::GetMsg() to pop from
  buffer if data in buffer is too long to be a valid message.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.13, Koh Hwee Miin,
- Changed stay alive timer from 3 seconds to 2 seconds.
- Fixed unfinished received message removed from buffer by adding buffer
  full check before removing message in CPrinterPort::GetMsg().
- Fixed passing un-continuous array to CMsg::TryParse() by copying data in
  circular buffer into a temporary array in CPrinterPort::GetMsg().

/////////////////////////////////////////////////////////////////////////////
v1.0.0.12, Koh Hwee Miin,
- Added "elapsed" parameter to IStateMach::Run() and CState::Run() to
  reduce the number of times to retrieve system time.
- Extend stay alive timer from 2 seconds to 3 seconds.
- Made CMsgMgr::FontID2Drv(), CMsgMgr::GraphicID2Drv(), and
  CMsgMgr::BarcodeID2Drv() inline for optimization purposes.
- Made CStatus::ExtPowerLost(), CStatus::FirmwareErr(), CStatus::NVMErr(),
  CStatus::PrintHeadErr(), CStatus::TemperatureErr(),
  CStatus::ChassisOpened(), CStatus::PrintHeadOpened(),
  CStatus::TopOfForm(), CStatus::PaperJam(), CStatus::PaperLow(),
  CStatus::PaperEmpty() inline for optimization purposes.


/////////////////////////////////////////////////////////////////////////////
v1.0.0.11, Koh Hwee Miin,
- Added more trace.
- Added handling of wcl::CArgumentException in CStateIdle::DefineGraphic(),
  CStateIdle::DefineRegion(), CStateIdle::DefineTemplate(), to prevent
  crash due to invalid graphic/region/template definitions.
- Added handling of wcl::CArgumentException in
  CStateDeleteGraphic::HandleRespStatus(), to prevent crash due to
  invalid graphic definition.
- Added handling of wcl::CArgumentException and CInvalidRegionException in
  CStateDeleteRegion::HandleRespStatus(), to prevent crash due to invalid
  region definition.
- Added handling of wcl::CArgumentException in
  CStateDeleteTempl::HandleRespStatus(), to prevent crash due to invalid
  template definition.
- Fixed m_pbyLastCmd not set to NULL after deletion in 
  CPrinterContext::SendNUpdateLastCmd(), causing crash due to dangling
  pointer after exception thrown in CMsg::Build().
- Added report of general error for unhandled exception caught in
  CPrinterContext::_Run().

/////////////////////////////////////////////////////////////////////////////
v1.0.0.10, Koh Hwee Miin,
- Fixed potential deadlock when un-initialized printer, by changing calls
  of wcl::CCriticalSection::Enter() to wcl::CCriticalSection::TryEnter() in
  CPrinter::Run().
- Fixed access violation crash in CPrinterContext::_Run() as
  SRunThreadParam passed into the thread should not be local, thus added
  CStateUnInit::m_ThreadParam to act as thread parameters storage.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.9, Koh Hwee Miin,
- Added support for firmware version "GUR126001".
  In CState::GetFirmwareCurrency().

/////////////////////////////////////////////////////////////////////////////
v1.0.0.8, Koh Hwee Miin,
- Re-compiled due to updates in print/common_v2.
- Updated PrintGetInterfaceVer().

/////////////////////////////////////////////////////////////////////////////
v1.0.0.7, Koh Hwee Miin,
- Removed definition of POLL_STATUS_INTERVAL, as the interval is now set
  in the constructor of CStatePollStatus and CStateDisconnected.
- Removed definition of ALIVE_TIMEOUT, as the interval is now set in the
  constructor of CStatePollStatus.
- Changed "class CRunThreadParam" to "struct SRunThreadParam".

/////////////////////////////////////////////////////////////////////////////
v1.0.0.7, Koh Hwee Miin,
- Updated PrintGetInterfaceVer().
- Added CStatePollStatus, and derived CStateInit, CStateSuspended,
  CStateGATReport, CStateCRC, CStateIdle, CStateDeleteGraphic,
  CStateAddGraphic, CStateDeleteRegion, CStateAddRegion, CStateDeleteTempl,
  CStateAddTempl, CStatePrinting from it.
- Added CStateInitialized, and derived CStatePollStatus, CStateDisconnected
  from it.
- Added CPrinterContext::Trace(), and replaced all TRACE() macro instance with
  it.
- Added CPrinterContext::SendNUpdateLastCmd(), and updated
  CStateDeleteGraphic::HandleRespStatus(),
  CStateDeleteRegion::HandleRespStatus(),
  CStateDeleteTempl::HandleRespStatus(),
  CStateIdle::DefineGraphic(), CStateIdle::DefineRegion(),
  CStateIdle::DefineTemplate(), CStateIdle::Print(),
  CStateIdle::FormFeed(), CStateSuspended::SelfTest(),
  CStateSuspended::RqGATReport(), CStateSuspended::CalculateCRC() to
  use this method to send commands to printer.
- Made CMsg::Build(), CMsg::Parse(), CMsg::TryParse() virtual.
- Added CPrinterContext::UpdateStatusNNotifyObserver(), and updated
  CStateSuspended::HandleRespStatus(), CStateGATReport::HandleRespStatus(),
  CStateCRC::HandleRespStatus(), CStateIdle::HandleRespStatus(),
  CStateDeleteGraphic::HandleRespStatus(),
  CStateAddGraphic::HandleRespStatus(),
  CStateDeleteRegion::HandleRespStatus(),
  CStateAddRegion::HandleRespStatus(),
  CStateDeleteTempl::HandleRespStatus(), CStateAddTempl::HandleRespStatus()
  to call this method.
- Updated CPrinterPort::Write() to ensure the method timeout if printer
  refuses to accept transfer.
- Added CCommException handling (by transitting to disconnected state) in
  CStateIdle::DefineGraphic(), CStateIdle::DefineRegion(),
  CStateIdle::DefineTemplate(), CStateIdle::Print(), CStateIdle::FormFeed(),
  CStateSuspended::SelfTest(), CStateSuspended::RqGATReport(),
  CStateSuspended::CalculateCRC().
- Added IStateMach::Dump(const wchar_t*),
  CPrinter::Dump(MSXML2::IXMLDOMElement*, const wchar_t*),
  CPrinter::Dump(const wchar_t*), and updated CPrinterContext::_Run() to
  invoke the dump when neccessary.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.6, Koh Hwee Miin,
- Updated PrintGetInterfaceVer().

/////////////////////////////////////////////////////////////////////////////
v1.0.0.5, Koh Hwee Miin,
- Added CStateDeleteGraphic, CStateAddGraphic as sub-state of
  CStateDefineGraphic to remove graphic of duplicate id before defining it.
- Added CStateDeleteRegion, CStateAddRegion as sub-state of
  CStateDefineRegion to remove region of duplicate id before defining it.
- Added CStateDeleteTempl, CStateAddTempl as sub-state of
  CStateDefineTempl to remove template of duplicate id before defining it. 
- Added CPrinterContext::m_LastGraphic;
- Extended polling interval from 200ms to 300ms.
- Extended alive interval from 1500ms to 2000ms.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.4, Koh Hwee Miin,
- Added CPrinterPort::Open() to fix handshaking not prepared when opening
  port.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.3, Koh Hwee Miin,
- Fixed unable to detect disconnection when failed to read/write comm. port.
- Fixed unable to un-initializes printer during initialization and
  disconnected state, by adding CStateInit::UnInit() and
  CStateDisconnected::UnInit().
- Fixed unable to detect disconnection by adding CStateInit::m_AliveTimer,
  CStateSuspended::m_AliveTimer, CStateGATReport::m_AliveTimer,
  CStateCRC::m_AliveTimer, CStateIdle::m_AliveTimer,
  CStateDefineGraphic::m_AliveTimer, CStateDefineRegion::m_AliveTimer,
  CStateDefineTempl::m_AliveTimer, CStatePrinting::m_AliveTimer.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.2, Koh Hwee Miin,
- Added CPrinter::GetFirmwareCurrency(), CState::GetFirmwareCurrency() and
  CPrinterContext::m_strSoftwareVer.

/////////////////////////////////////////////////////////////////////////////
v1.0.0.1, Koh Hwee Miin,
- Fixed PrintCreateInstance() always returns NULL.
- Added CMsg::TryParse(), CMsgRespCRC::TryParse(),
  CMsgRespStatus::TryParse().
- Removed CPrinterPort::TryParseRespCRC(),
  CPrinterPort::TryParseRespStatus().
- Fixed errors not announced when transit from STATE_INIT to STATE_READY.
- Fixed job pointer not set when building print command in
  CStateIdle::Print().
- Added CPrinterContext::m_bInitSuspend.
- Added CStateSuspended::SelfTest() to print void page.
