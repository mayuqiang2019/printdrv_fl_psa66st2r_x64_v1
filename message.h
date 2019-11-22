#pragma once
#pragma warning (disable : 4786)
#pragma warning (disable : 4231)

#include "printPrinter.h"

/// <summary>Message manager.</summary>
class CMsgMgr
{
public:
  /// <value>Command header.</value>
  static const char CMD_START = '^';
  
  /// <value>Command terminator.</value>
  static const char CMD_END = '^';
  
  /// <value>Response header.</value>
  static const char RESP_START = '*';
  
  /// <value>Response terminator.</value>
  static const char RESP_END = '*';

  /// <value>Unknown/unsupported command.</value>
  static const char CMD_UNKNOWN = 0;
  
  /// <value>Delimiter.</value>
  static const char CMD_DELIMITER = '|';
  
  /// <value>Define template.</value>
  static const char CMD_DEFINE_TEMPL = 'T';
  
  /// <value>Define print region.</value>
  static const char CMD_DEFINE_REGION = 'R';

  /// <value>Print.</value>
  static const char CMD_PRINT = 'P';
  
  /// <value>Library management command.</value>
  static const char CMD_LIBMANAGE = 'l';
  
  /// <value>Obtain program CRC command and response.</value>
  static const char CMD_CRC = 'G';
  
  /// <value>Request printer status command and response.</value>
  static const char CMD_STATUS = 'S';
  
  /// <value>Feed command.</value>
  static const char CMD_FEED = 'f';

  /// <value>Flash transfer.</value>
  static const char CMD_FLASH_TRANSFER = 'z';

public:
  char GetType(BYTE* msg, DWORD msgLen);
  bool IsUserDefinedTempl(short id);

  BYTE TemplID2Drv(short id);
  BYTE RegionID2Drv(short id);

  BYTE FontID2Drv(short id);
  BYTE GraphicID2Drv(short id);
  BYTE BarcodeID2Drv(short id);

  BYTE TemplID2PageID(short id);
  BYTE TemplID2PageIDPrint(short id);
};

/// <summary>Checks if specified ID refers to user-defined template.</summary>
/// <param name="id">Template ID.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="id"/> is out
/// of range.</exception>
/// <returns>True if <paramref name="id"/> refers to user-defined template,
/// false otherwise.</returns>
inline bool CMsgMgr::IsUserDefinedTempl(short id)
{
	if((id < 0) || (id > 999))
	{
		WCL_THROW_ARGUMENTEXCEPTION(L"id", L"must between 0 to 999");
	}
	return id >= 100;
}

/// <summary>Converts font ID to driver's range.</summary>
/// <param name="id">Font ID.</param>
/// <returns>Font ID mapped to driver's range.</returns>
/// <exception cref="wcl::CArgumentException">If <paramref name="id"/> is out
/// of range.</exception>
/// <remarks>This printer use '0' through '15' as font ID. Thus
/// this function map 0~999 to the printer range by truncating value > 15 to 15,
/// e.g. 0 will be mapeed to 0, and 15, 16, 17 all will be mapped to 15.</remarks>
inline BYTE CMsgMgr::FontID2Drv(short id)
{
  if((id < 0) || (id > 999))
  {
    WCL_THROW_ARGUMENTEXCEPTION(L"id", L"must between 0 to 999");
  }

  return __min(id, 15);
}

/// <summary>Converts graphic ID to driver's range.</summary>
/// <param name="id">Graphic ID.</param>
/// <returns>Graphic ID mapped to driver's range.</returns>
/// <exception cref="wcl::CArgumentException">If <paramref name="id"/> is out
/// of range.</exception>
/// <remarks>This printer use [0x45~0x5D, 0x5F~0x60] as graphic ID. Thus
/// this function map 1~255 to the printer range by truncating value > 27 to 27,
/// e.g. 1 will be mapeed to 0x45, and 27, 28, 29 all will be mapped to 0x60.</remarks>
inline BYTE CMsgMgr::GraphicID2Drv(short id)
{
  if((id < 1) || (id > 255))
  {
    WCL_THROW_ARGUMENTEXCEPTION(L"id", L"must between 0 to 999");
  }

  if(id < 26) { return id + 0x44; }
    
  return __min(id, 27) + 0x45;
}

/// <summary>Converts barcode ID to driver's range.</summary>
/// <param name="id">Barcode ID.</param>
/// <returns>Barcode ID mapped to driver's range.</returns>
/// <exception cref="wcl::CArgumentException">If <paramref name="id"/> is out
/// of range.</exception>
/// <remarks>This printer use ['a'~'{', '}'] as graphic ID. Thus
/// this function map 0~999 to the printer range by truncating value > 27 to 27,
/// e.g. 0 will be mapeed to 'a', and 27, 28, 29 all will be mapped to '}'.</remarks>
inline BYTE CMsgMgr::BarcodeID2Drv(short id)
{
  if((id < 0) || (id > 999))
  {
    WCL_THROW_ARGUMENTEXCEPTION(L"id", L"must between 0 to 999");
  }

  if(id < 27) { return id + 'a'; }

  return '}';
}

/// <summary>Determines memory page ID to store the defined template.</summary>
/// <param name="id">Template ID.</param>
/// <returns>Memory page ID.</returns>
/// <exception cref="wcl::CArgumentException">If <paramref name="id"/> is out
/// of range.</exception>
inline BYTE CMsgMgr::TemplID2PageID(short id)
{
	if((id < 0) || (id > 999))
	{
		WCL_THROW_ARGUMENTEXCEPTION(L"id", L"must between 0 to 999");
	}

	if(!IsUserDefinedTempl(id))
	{
		return 'A';
	}
	return min('B' + id - 100, 'J');
}

/// <summary>Determines memory page ID to be used in print command for user-defined
/// template.</summary>
/// <param name="id">Template ID.</param>
/// <returns>Memory page ID.</returns>
/// <exception cref="wcl::CArgumentException">If <paramref name="id"/> is out
/// of range.</exception>
inline BYTE CMsgMgr::TemplID2PageIDPrint(short id)
{
	if(!IsUserDefinedTempl(id))
	{
		WCL_THROW_ARGUMENTEXCEPTION(L"id", L"must user-defined template ID");
	}
	return min('1' + id - 100, '9');
}

/// <summary>Communication message.</summary>
class CMsg
{
public:
  /// <summary>Constructs command bytes.</summary>
  /// <param name="buffer">Buffer to receive constructed bytes. If NULL, function
  /// ignores all arguments and returns size of buffer required to contain the command bytes.</param>
  /// <param name="bufferSize">Size of <paramref name="buffer"/> in number of
  /// bytes. If zero, function ignores all arguments and returns size of buffer
  /// required to contain the command bytes.</param>
  /// <returns>Length of constructed command bytes, or size of buffer required to
  /// contain the command bytes if <paramref name="buffer"/> is NULL or
  /// <paramref name="bufferSize"/> is zero.</returns>
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize) { return 0; }
  
  /// <summary>Parses printer response.</summary>
  /// <param name="resp">Printer's response.</param>
  /// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
  /// <exception cref="wcl::CArgumentNullException">If <paramref name="resp"/> is
  /// NULL.</exception>
  /// <exception cref="wcl::CArgumentException">If <paramref name="resp"/> is not
  /// a valid response or if <paramref name="size"/> is zero.</exception>
  virtual void Parse(BYTE* resp, DWORD size) {}

  /// <summary>Tries to parses printer response.</summary>
  /// <param name="resp">Printer's response.</param>
  /// <param name="size">Size of <paramref name="resp"/>, in number of bytes.</param>
  /// <param name="errMsg">Pointer to <see cref="CWkString"/> object to receive
  /// error description if parse failed. If NULL, no error description is returned.</param>
  /// <returns>True if parse success, false otherwise.</returns>
  virtual bool TryParse(BYTE* resp, DWORD size, CWkString* errMsg) { return false; }
};

/// <summary>Define template command.</summary>
class CMsgDefineTempl : public CMsg
{
public:
  /// <value>True to define, false to delete.</value>
  bool m_bDefine;

  /// <value>Pointer to template to be defined.</value>
  const print::CTemplate *m_pTemplate;

public:
  CMsgDefineTempl();

public:
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize);

  void Dump(MSXML2::IXMLDOMElement* pElem);

protected:
  DWORD BuildDefine(BYTE* buffer, DWORD bufferSize);
  DWORD BuildDelete(BYTE* buffer, DWORD bufferSize);
};

/// <summary>Define print region command.</summary>
class CMsgDefineRegion : public CMsg
{
public:
  /// <value>True to define, false to delete.</value>
  bool m_bDefine;

  /// <value>Pointer to print region to be defined.</value>
  const print::CRegion *m_pRegion;

public:
  CMsgDefineRegion();

public:
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize);

  void Dump(MSXML2::IXMLDOMElement* pElem);

protected:
  DWORD BuildDefine(BYTE* buffer, DWORD bufferSize);
  DWORD BuildDelete(BYTE* buffer, DWORD bufferSize);
};

/// <summary>Print command.</summary>
class CMsgPrint : public CMsg
{
public:
  /// <value>Pointer to job to be printed.</value>
  print::CJob *m_pJob;

public:
  CMsgPrint();

public:
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize);

  void Dump(MSXML2::IXMLDOMElement* pElem);
};

/// <summary>Library management comamnd.</summary>
class CMsgLibManage : public CMsg
{
public:
  /// <value>True to define, false to delete.</value>
  bool m_bDefine;

  /// <value>Pointer to graphic to be defined.</value>
  const print::CGraphic *m_pGraphic;

public:
  CMsgLibManage();

public:
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize);

  void Dump(MSXML2::IXMLDOMElement* pElem);

protected:
  DWORD BuildDefine(BYTE* buffer, DWORD bufferSize);
  DWORD BuildDelete(BYTE* buffer, DWORD bufferSize);
};

/// <summary>Clear error status command.</summary>
class CMsgClearErr : public CMsg
{
public:
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize);
};

/// <summary>Obtain program CRC.</summary>
class CMsgObtainCRC : public CMsg
{
public:
  /// <value>Start address.</value>
  DWORD m_dwStartAddr;

  /// <value>Seed.</value>
  WORD m_wSeed;

public:
  CMsgObtainCRC();

public:
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize);

  void Dump(MSXML2::IXMLDOMElement* pElem);
};

/// <summary>Printer's response to obtain program CRC command.</summary>
class CMsgRespCRC : public CMsg
{
public:
  /// <value>Computed CRC.</value>
  WORD m_wCRC;

public:
  CMsgRespCRC();

public:
  virtual void Parse(BYTE* resp, DWORD size);
  virtual bool TryParse(BYTE* resp, DWORD size, CWkString* errMsg);

  void Dump(MSXML2::IXMLDOMElement* pElem);
};

/// <summary>Feed command.</summary>
class CMsgFeed : public CMsg
{
public:
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize);
};

/// <summary>Request printer status.</summary>
class CMsgStatus : public CMsg
{
public:
  virtual DWORD Build(BYTE* buffer, DWORD bufferSize);
};

/// <summary>Flash transfer.</summary>
class CMsgFlashTransfer : public CMsg
{
public:
	BYTE m_byPageID;

public:
	CMsgFlashTransfer() : m_byPageID(0) {}
	CMsgFlashTransfer(BYTE pageID) : m_byPageID(pageID) {}

public:
	virtual DWORD Build(BYTE* buffer, DWORD bufferSize);
};

/// <summary>Printer status.</summary>
class CStatus
{
public:
  /// <value>True if busy processing print or command, false otherwise.</value>
  bool m_bBusy;
  
  /// <value>True if error detected, false otherwise.</value>
  bool m_bError;
  
  /// <value>True if print head opened, false otherwise.</value>
  bool m_bPrintHeadOpen;
  
  /// <value>True if paper out, false otherwise.</value>
  bool m_bPaperOut;

  /// <value>True if print head error, false otherwise.</value>
  bool m_bPrintHeadErr;
  
  /// <value>True if voltage error, false otherwise.</value>
  bool m_bVoltageErr;
  
  /// <value>True if print head temperature error, false otherwise.</value>
  bool m_bTemperatureErr;
  
  /// <value>True if library reference in define template or define print region is
  /// invalid, false otherwise.</value>
  bool m_bLibRefErr;
  
  /// <value>True if there is an error in the define template or define print
  /// region command, or error in data of print command, false otherwise.</value>
  bool m_bRegionDataErr;
  
  /// <value>True if there is an error in library management command when loading
  /// graphics/fonts, false otherwise.</value>
  bool m_bLoadLibErr;
  
  /// <value>True if printer buffer overflow, false otherwise.</value>
  bool m_bBufferOverflow;
  
  /// <value>True if not enough memory to print current job, false otherwise.</value>
  bool m_bJobMemOverflow;
  
  /// <value>True if there is a syntax error with a receive command, or the command
  /// is unsupported, false otherwise.</value>
  bool m_bCmdErr;
  
  /// <value>True if no font is available to carry out printing, false otherwise.</value>
  bool m_bNoFont;
  
  /// <value>True if paper in chute (ticket not taken), false otherwise.</value>
  bool m_bPaperInChute;
  
  /// <value>True if flash error, false otherwise.</value>
  bool m_bFlashErr;
  
  /// <value>True if printer offline, false otherwise.</value>
  bool m_bOffLine;
 
  /// <value>True if wrong paper, false otherwise.</value>
  bool m_bWrongPaper;
  
  /// <value>True for journal mode, false for TCL mode.</value>
  bool m_bJournalMode;
  
  /// <value>True if cutter error, false otherwise.</value>
  bool m_bCutterErr;
  
  /// <value>True if paper jammed, false otherwise.</value>
  bool m_bPaperJam;
  
  /// <value>True if paper low, false otherwise.</value>
  bool m_bPaperLow;
  
  /// <value>True if last bar of barcode printed, false otherwise.</value>
  bool m_bLastBarPrinted;
  
  /// <value>True if paper at top of form, false otherwise.</value>
  bool m_bTopOfForm;
  
  /// <value>True if printer ready to receive command, false otherwise.</value>
  bool m_bReadyToRx;
  
  /// <value>True if printer door opened, false otherwise.</value>
  bool m_bDoorOpened;
  
  /// <value>True if printer has powered up or been reset, false otherwise.</value>
  bool m_bPowerUpReset;

public:
  CStatus();

public:
  bool ExtPowerLost() const;
  bool FirmwareErr() const;
  bool NVMErr() const;
  bool PrintHeadErr() const;
  bool TemperatureErr() const;
  bool GeneralErr() const;
  
  bool ChassisOpened() const;
  bool PrintHeadOpened() const;
  bool TopOfForm() const;
  bool PaperJam() const;
  bool PaperLow() const;
  bool PaperEmpty() const;

  bool ShouldSuspend() const;
  bool NeedClearErr() const;

public:
  void Dump(MSXML2::IXMLDOMElement* pElem);
};

/// <summary>Checks for external power lost.</summary>
/// <returns>True if external power lost, false otherwise.</returns>
inline bool CStatus::ExtPowerLost() const
{
  return m_bVoltageErr;
}

/// <summary>Checks for firmware error.</summary>
/// <returns>True if firmware error, false otherwise.</returns>
inline bool CStatus::FirmwareErr() const
{
  return m_bFlashErr;
}

/// <summary>Checks for non-volatile memory error.</summary>
/// <returns>True if non-volatile memory error, false otherwise.</returns>
inline bool CStatus::NVMErr() const
{
  return false;
}

/// <summary>Checks for print head error.</summary>
/// <returns>True if print head error, false otherwise.</returns>
inline bool CStatus::PrintHeadErr() const
{
  return m_bPrintHeadErr;
}

/// <summary>Checks for print head temperature error.</summary>
/// <returns>True if print head temperature error, false otherwise.</returns>
inline bool CStatus::TemperatureErr() const
{
  return m_bTemperatureErr;
}

/// <summary>Checks for chassis opened.</summary>
/// <returns>True if chassis opened, false otherwise.</returns>
inline bool CStatus::ChassisOpened() const
{
  return m_bDoorOpened;
}

/// <summary>Checks for print head opened.</summary>
/// <returns>True if print head opened, false otherwise.</returns>
inline bool CStatus::PrintHeadOpened() const
{
  return m_bPrintHeadOpen;
}

/// <summary>Checks for paper at top of form.</summary>
/// <returns>True if paper at top of form, false otherwise.</returns>
inline bool CStatus::TopOfForm() const
{
  return true;
}

/// <summary>Checks for paper jammed.</summary>
/// <returns>True if paper jammed, false otherwise.</returns>
inline bool CStatus::PaperJam() const
{
  return m_bPaperJam;
}

/// <summary>Checks for paper low.</summary>
/// <returns>True if paper low, false otherwise.</returns>
inline bool CStatus::PaperLow() const
{
  return m_bPaperLow;
}

/// <summary>Checks for paper empty.</summary>
/// <returns>True if paper empty, false otherwise.</returns>
inline bool CStatus::PaperEmpty() const
{
  return m_bPaperOut || m_bWrongPaper;
}

/// <summary>Printer status response.</summary>
class CMsgRespStatus : public CMsg
{
public:
  /// <value>Unit address.</value>
  DWORD m_dwUnitAddr;

  /// <value>Software version information.</value>
  CWkString m_strSoftwareVer;
  
  /// <value>Status flags.</value>
  CStatus m_Status;

  /// <value>If no system error reported, this is the template ID of the last
  /// print job processed.</value>
  BYTE m_byTemplateID;

public:
  CMsgRespStatus();

public:
  virtual void Parse(BYTE* resp, DWORD size);
  virtual bool TryParse(BYTE* resp, DWORD size, CWkString* errMsg);

  void Dump(MSXML2::IXMLDOMElement* pElem);

protected:
  void ParseStatus(BYTE flag1, BYTE flag2, BYTE flag3, BYTE flag4, BYTE flag5);
};
