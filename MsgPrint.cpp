#include "stdafx.h"
#include "message.h"

/// <summary>Constructor.</summary>
CMsgPrint::CMsgPrint() : m_pJob(NULL)
{
}

#define FILL_BUFFER(c)  len++;\
                        if((buffer != NULL) && (bufferSize >= len))\
                        {\
                          buffer[len - 1] = c;\
                        }

/// <summary>Constructs command bytes.</summary>
/// <param name="buffer">Buffer to receive constructed bytes. If NULL, function
/// ignores all arguments and returns size of buffer required to contain the
/// command bytes.</param>
/// <param name="bufferSize">Size of <paramref name="buffer"/> in number of bytes.
/// If zero, function ignores all arguments and returns size of buffer required to
/// contain the command bytes.</param>
/// <returns>Length of constructed command bytes, or size of buffer required to
/// contain the command bytes if <paramref name="buffer"/> is NULL or
/// <paramref name="bufferSize"/> is zero.</returns>
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pJob"/> not
/// assigned.</exception>
DWORD CMsgPrint::Build(BYTE* buffer, DWORD bufferSize)
{
  POS pos;
  BYTE byTmp;
  CMsgMgr mgr;
  DWORD len = 0, asciiLen, i;
  char *asciiBuffer;
  print::CData data;

  if(m_pJob == NULL)
  {
    WCL_THROW_INVALIDOPERATIONEXCEPTION(L"no job assigned");
  }

  FILL_BUFFER(CMsgMgr::CMD_START);
  FILL_BUFFER('P');
  if(mgr.IsUserDefinedTempl(m_pJob->m_nsTemplateID))
  {
	  FILL_BUFFER(mgr.TemplID2PageIDPrint(m_pJob->m_nsTemplateID));
  }
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  byTmp = mgr.TemplID2Drv(m_pJob->m_nsTemplateID);
  FILL_BUFFER(byTmp);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  FILL_BUFFER('1');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  pos = m_pJob->GetHeadPos();
  while(pos != NULL)
  {
    data = m_pJob->GetNext(pos);
    
    asciiLen = data.m_strData.ToMultiByte(NULL, 0);
    asciiBuffer = new char[asciiLen];
    if(asciiBuffer == NULL) { throw wcl::COutOfMemoryException(); }
    data.m_strData.ToMultiByte(asciiBuffer, asciiLen);

    for(i = 0;i < (asciiLen - 1);i++) { FILL_BUFFER(asciiBuffer[i]); }
    delete[] asciiBuffer;
    FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  } // while...
  FILL_BUFFER(CMsgMgr::CMD_END);

  return len;
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CMsgPrint::Dump(MSXML2::IXMLDOMElement* pElem)
{
  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pJob", (DWORD)m_pJob);
    } // if...

  }
  catch(...) {}
}

