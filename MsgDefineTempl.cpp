#include "stdafx.h"
#include "message.h"

/// <summary>Constructor.</summary>
CMsgDefineTempl::CMsgDefineTempl() : m_bDefine(true), m_pTemplate(NULL)
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
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pTemplate"/>
/// is not assigned.</exception>
DWORD CMsgDefineTempl::Build(BYTE* buffer, DWORD bufferSize)
{
  if(m_bDefine) { return BuildDefine(buffer, bufferSize); }
  return BuildDelete(buffer, bufferSize);
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CMsgDefineTempl::Dump(MSXML2::IXMLDOMElement* pElem)
{
  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bDefine", m_bDefine);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pTemplate", (DWORD)m_pTemplate);
    } // if...

  }
  catch(...) {}
}

/// <summary>Constructs command bytes to define template.</summary>
/// <param name="buffer">Buffer to receive constructed bytes. If NULL, function
/// ignores all arguments and returns size of buffer required to contain the
/// command bytes.</param>
/// <param name="bufferSize">Size of <paramref name="buffer"/> in number of bytes.
/// If zero, function ignores all arguments and returns size of buffer required to
/// contain the command bytes.</param>
/// <returns>Length of constructed command bytes, or size of buffer required to
/// contain the command bytes if <paramref name="buffer"/> is NULL or
/// <paramref name="bufferSize"/> is zero.</returns>
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pTemplate"/>
/// is not assigned.</exception>
DWORD CMsgDefineTempl::BuildDefine(BYTE* buffer, DWORD bufferSize)
{
  POS pos;
  short regionID;
  BYTE byTmp;
  CMsgMgr mgr;
  DWORD len = 0;

  if(m_pTemplate == NULL)
  {
    WCL_THROW_INVALIDOPERATIONEXCEPTION(L"no template assigned.");
  }

  FILL_BUFFER(CMsgMgr::CMD_START);
  FILL_BUFFER(CMsgMgr::CMD_DEFINE_TEMPL);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  byTmp = mgr.TemplID2Drv(m_pTemplate->m_nsID);
  FILL_BUFFER(byTmp);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  FILL_BUFFER('R');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER('5');
  FILL_BUFFER('0');
  FILL_BUFFER('0');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER('1');
  FILL_BUFFER('2');
  FILL_BUFFER('4');
  FILL_BUFFER('0');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  pos = m_pTemplate->GetHeadPos();
  while(pos != NULL)
  {
    regionID = m_pTemplate->GetNext(pos);
    byTmp = mgr.RegionID2Drv(regionID);
    FILL_BUFFER(byTmp);
    FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  } // while...

  FILL_BUFFER(CMsgMgr::CMD_END);

  return len;
}

/// <summary>Constructs command bytes to delete template.</summary>
/// <param name="buffer">Buffer to receive constructed bytes. If NULL, function
/// ignores all arguments and returns size of buffer required to contain the
/// command bytes.</param>
/// <param name="bufferSize">Size of <paramref name="buffer"/> in number of bytes.
/// If zero, function ignores all arguments and returns size of buffer required to
/// contain the command bytes.</param>
/// <returns>Length of constructed command bytes, or size of buffer required to
/// contain the command bytes if <paramref name="buffer"/> is NULL or
/// <paramref name="bufferSize"/> is zero.</returns>
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pTemplate"/>
/// is not assigned.</exception>
DWORD CMsgDefineTempl::BuildDelete(BYTE* buffer, DWORD bufferSize)
{
  BYTE byTmp;
  CMsgMgr mgr;
  DWORD len = 0;

  if(m_pTemplate == NULL)
  {
    WCL_THROW_INVALIDOPERATIONEXCEPTION(L"no template assigned.");
  }

  FILL_BUFFER(CMsgMgr::CMD_START);
  FILL_BUFFER(CMsgMgr::CMD_DEFINE_TEMPL);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  byTmp = mgr.TemplID2Drv(m_pTemplate->m_nsID);
  FILL_BUFFER(byTmp);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  FILL_BUFFER('D');
  FILL_BUFFER('R');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(CMsgMgr::CMD_END);

  return len;
}
