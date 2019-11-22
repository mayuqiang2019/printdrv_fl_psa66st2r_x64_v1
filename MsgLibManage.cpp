#include "stdafx.h"
#include "message.h"

/// <summary>Constructor.</summary>
CMsgLibManage::CMsgLibManage() : m_bDefine(true), m_pGraphic(NULL)
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
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pGraphic"/>
/// not assigned.</exception>
DWORD CMsgLibManage::Build(BYTE* buffer, DWORD bufferSize)
{
  if(m_bDefine) { return BuildDefine(buffer, bufferSize); }
  return BuildDelete(buffer, bufferSize);
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CMsgLibManage::Dump(MSXML2::IXMLDOMElement* pElem)
{
  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bDefine", m_bDefine);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pGraphic", (DWORD)m_pGraphic);
    } // if...

  }
  catch(...) {}
}

/// <summary>Constructs command bytes to define graphic.</summary>
/// <param name="buffer">Buffer to receive constructed bytes. If NULL, function
/// ignores all arguments and returns size of buffer required to contain the
/// command bytes.</param>
/// <param name="bufferSize">Size of <paramref name="buffer"/> in number of bytes.
/// If zero, function ignores all arguments and returns size of buffer required to
/// contain the command bytes.</param>
/// <returns>Length of constructed command bytes, or size of buffer required to
/// contain the command bytes if <paramref name="buffer"/> is NULL or
/// <paramref name="bufferSize"/> is zero.</returns>
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pGraphic"/>
/// not assigned.</exception>
DWORD CMsgLibManage::BuildDefine(BYTE* buffer, DWORD bufferSize)
{
  BYTE byTmp;
  CMsgMgr mgr;
  DWORD len = 0;
  std::stringstream ss;
  std::string strTmp;
  std::string::const_iterator i;
  WORD j;

  if(m_pGraphic == NULL)
  {
    WCL_THROW_INVALIDOPERATIONEXCEPTION(L"no graphic assigned");
  }

  FILL_BUFFER(CMsgMgr::CMD_START);
  FILL_BUFFER('l');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER('A');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER('F');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  byTmp = mgr.GraphicID2Drv(m_pGraphic->m_byID);
  FILL_BUFFER(byTmp);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  ss.str("");
  ss << (DWORD)m_pGraphic->m_wSize;
  strTmp = ss.str();
  for(i = strTmp.begin();i != strTmp.end();i++) { FILL_BUFFER(*i); }
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  for(j = 0;j < m_pGraphic->m_wSize;j++) { FILL_BUFFER(m_pGraphic->m_pbyData[j]); }
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(CMsgMgr::CMD_END);

  return len;
}

/// <summary>Constructs command bytes to delete graphic.</summary>
/// <param name="buffer">Buffer to receive constructed bytes. If NULL, function
/// ignores all arguments and returns size of buffer required to contain the
/// command bytes.</param>
/// <param name="bufferSize">Size of <paramref name="buffer"/> in number of bytes.
/// If zero, function ignores all arguments and returns size of buffer required to
/// contain the command bytes.</param>
/// <returns>Length of constructed command bytes, or size of buffer required to
/// contain the command bytes if <paramref name="buffer"/> is NULL or
/// <paramref name="bufferSize"/> is zero.</returns>
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pGraphic"/>
/// not assigned.</exception>
DWORD CMsgLibManage::BuildDelete(BYTE* buffer, DWORD bufferSize)
{
  BYTE byTmp;
  CMsgMgr mgr;
  DWORD len = 0;

  if(m_pGraphic == NULL)
  {
    WCL_THROW_INVALIDOPERATIONEXCEPTION(L"no graphic assigned");
  }

  FILL_BUFFER(CMsgMgr::CMD_START);
  FILL_BUFFER('l');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER('D');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER('F');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  byTmp = mgr.GraphicID2Drv(m_pGraphic->m_byID);
  FILL_BUFFER(byTmp);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  
  FILL_BUFFER('G');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(CMsgMgr::CMD_END);

  return len;
}
