#include "stdafx.h"
#include "printer.h"

/// <summary>Constructor.</summary>
CPrinterPort::CPrinterPort() :
  m_Buffer(3000),
  m_strHandshake(L"x")
{
  m_nPort = 1;
  m_nBaudRate = 38400;
  m_nParity = CComPort::PNONE;
  m_nDataBit = 8;
  m_nStopBit = 1;
  m_nTimeOut = 5;
  m_nBufferSize = 2048;
}

/// <summary>Destructor.</summary>
CPrinterPort::~CPrinterPort()
{

}

/// <summary>Extracts port related parameters.</summary>
/// <param name="parameters">Parameters string.</param>
/// <exception cref="wcl::CArgumentException">If <paramref name="parameters"/>
/// is invalid.</exception>
void CPrinterPort::Parse(const wchar_t* parameters)
{
	CWkString value;
	CWkMapStr<CWkString> pair;

  if(parameters == NULL) { WCL_THROW_ARGUMENTNULLEXCEPTION(L"parameters"); }
  ::Parse(parameters, pair);

	if(pair.Get(L"port", value)) { m_nPort = wcstol(value, NULL, 10); }
  else { WCL_THROW_ARGUMENTEXCEPTION(L"parameters", L"missing 'port' parameter"); }

  if(pair.Get(L"baudrate", value)) { m_nBaudRate = wcstol(value, NULL, 10); }

	if(pair.Get(L"parity", value))
	{
		if(value == L"none")			{ m_nParity = CComPort::PNONE; }
		else if(value == L"even")	{ m_nParity = CComPort::PEVEN; }
		else if(value == L"odd")	{ m_nParity = CComPort::PODD; }
	}	// if...

	if(pair.Get(L"databit", value)) { m_nDataBit = wcstol(value, NULL, 10); }
	if(pair.Get(L"timeout", value)) { m_nTimeOut = wcstol(value, NULL, 10); }
	if(pair.Get(L"buffer_size", value)) { m_nBufferSize = wcstol(value, NULL, 10); }
  pair.Get(L"handshake", m_strHandshake);
}

/// <summary>Opens communication port.</summary>
/// <returns>True if port opened successfully, false otherwise.</returns>
bool CPrinterPort::Open()
{
  DCB dcb;

  try
  {

    if(!CComPort::Open(true)) { return false; }
    GetState(dcb);

    if(m_strHandshake == L"rtsx")
    {
      dcb.fOutxCtsFlow = TRUE;
      dcb.fOutxDsrFlow = TRUE;
      dcb.fDtrControl = DTR_CONTROL_DISABLE;
      dcb.fDsrSensitivity = TRUE;
      dcb.fTXContinueOnXoff = TRUE;
      dcb.fOutX = TRUE;
      dcb.fInX = FALSE;
      dcb.fErrorChar = FALSE;
      dcb.fNull = FALSE;
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
    }
    else if(m_strHandshake == L"rts")
    {
      dcb.fOutxCtsFlow = TRUE;
      dcb.fOutxDsrFlow = TRUE;
      dcb.fDtrControl = DTR_CONTROL_DISABLE;
      dcb.fDsrSensitivity = TRUE;
      dcb.fTXContinueOnXoff = TRUE;
      dcb.fOutX = FALSE;
      dcb.fInX = FALSE;
      dcb.fErrorChar = FALSE;
      dcb.fNull = FALSE;
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
    }
    else // "x"
    {
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fDtrControl = DTR_CONTROL_DISABLE;
      dcb.fDsrSensitivity = FALSE;
      dcb.fTXContinueOnXoff = TRUE;
      dcb.fOutX = TRUE;
      dcb.fInX = FALSE;
      dcb.fErrorChar = FALSE;
      dcb.fNull = FALSE;
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
    } // if...else...
    SetState(dcb);

    Break(100);
    ClearError();
    Purge();

  }
  catch(...)
  {
    Close();
    return false;
  }

  return true;
}

/// <summary>Poll bytes from communication port buffer.</summary>
void CPrinterPort::Poll()
{
  int i, cnt = 1;
  BYTE byIn[256];
  COMSTAT stat;

  if(m_hComm == INVALID_HANDLE_VALUE)
  {
    WCL_THROW_INVALIDOPERATIONEXCEPTION(L"comm. port not opened");
  }

  ClearError(&stat);
  if(stat.cbInQue > 0) { cnt = __min(256, stat.cbInQue); }
  cnt = Read(byIn, cnt);

  m_csBuffer.Enter();
  for(i = 0;i < cnt;i++)
  {
    if(m_Buffer.IsFull())
    {
      break;
    }
    m_Buffer.Push(byIn[i]);
  } // for...
  m_csBuffer.Leave();
}

/// <summary>Retrieves message from buffer.</summary>
/// <param name="buffer">Buffer to contain retrieved message. If NULL, function
/// ignores the arguments and returns size of buffer required to contain the
/// message.</param>
/// <param name="bufferSize">Size of <paramref name="buffer"/>, in number of bytes.
/// If less than required size, function ignores the arguments and returns size
/// of buffer required to contain the message.</param>
/// <returns>Size of buffer required to contain the message, in number of bytes.
/// 0 if no message available.</returns>
DWORD CPrinterPort::GetMsg(BYTE* buffer, DWORD bufferSize)
{
  DWORD i, len = 0;
  CMsgRespCRC respCRC;
  CMsgRespStatus respStatus;
  CWkString strTmp;
  BYTE tmpBuffer[3000];

  m_csBuffer.Enter();
  while(!m_Buffer.IsEmpty())
  {
    // discard non-header start bytes.
    if(m_Buffer.GetHead() != CMsgMgr::RESP_START)
    {
      TRACE(L"[printdrv_fl_psa66st2r][CPrinterPort::GetMsg] discard non-header.\n");
      m_Buffer.Pop();
    }
    else
    {
      break;
    }
  } // while...

  if(!m_Buffer.IsEmpty())
  {
    // copy circular buffer into temporary continuous buffer
    for(i = 0;i < m_Buffer.GetCount();i++)
    {
      tmpBuffer[i] = m_Buffer.GetAt(i);
    }

    // first bytes in buffer must be header.
    for(i = 1;i < m_Buffer.GetCount();i++)
    {
      if(tmpBuffer[i] == CMsgMgr::RESP_END)
      {
        if(respCRC.TryParse(tmpBuffer, i + 1, NULL)) { break; }
        if(respStatus.TryParse(tmpBuffer, i + 1, NULL)) { break; }
      }
    } // for...

    if(i < m_Buffer.GetCount())
    {
      // match found.
      len = i + 1;
      if((buffer != NULL) && (bufferSize >= len))
      {
        for(i = 0;i < len;i++) { buffer[i] = m_Buffer.Pop(); }
      }
    }
    else if(m_Buffer.IsFull() || (m_Buffer.GetCount() > 44))
    {
      TRACE(L"[printdrv_fl_psa66st2r][CPrinterPort::GetMsg] discard full.\n");

      // pop only when full to prevent popping of unfinished received message,
      // or when buffer too long for a messsage to prevent parsing always start
      // at corrupted bytes, assuming longest possible message is 44.
      m_Buffer.Pop();
    } // if...else...
  } // if...
  m_csBuffer.Leave();

  return len;
}

/// <summary>Writes data to communication port.</summary>
/// <param name="buffer">Data to be written.</param>
/// <param name="dataSize">Size of <paramref name="buffer"/>, in number of bytes.</param>
/// <returns>Number of bytes written.</returns>
int CPrinterPort::Write(BYTE* data, int dataSize)
{
  int written, timeOut = __max(1, m_nTimeOut) * dataSize * 3;
  timeOut = __max(1000, timeOut);

  written = CComPort::Write(data, dataSize, timeOut);
  if(written != dataSize)
  {
    throw CCommException(m_nPort, L"Write to comm. port timeout.");
  }

  /*TRACE(L"[printdrv_fl_psa66st2r] SEND ");
  for(int i = 0;i < dataSize;i++)
  {
      TRACE(L"%c", data[i]);
  }
  TRACE(L"\n");*/

  return written;
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CPrinterPort::Dump(MSXML2::IXMLDOMElement* pElem)
{
  MSXML2::IXMLDOMElement *pChild = NULL;
  try
  {

    if(pElem != NULL)
    {
      if(!CXmlUtil::AppendChild(pElem, L"m_Buffer", &pChild)) { throw false; }
      wcl::CDumpHelper::DumpWkCirBuffer<BYTE>(m_Buffer, pChild);
      SAFE_RELEASE(pChild);

      wcl::CDumpHelper::DumpAttr<const wchar_t*>(pElem, L"m_strHandshake",
        m_strHandshake);
    } // if...

  }
  catch(...) {}
  SAFE_RELEASE(pChild);
}
