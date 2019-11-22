#include "stdafx.h"
#include "message.h"

/// <summary>Constructor.</summary>
CMsgDefineRegion::CMsgDefineRegion() : m_bDefine(true), m_pRegion(NULL)
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
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pRegion"/>
// is not assigned.</exception>
/// <exception cref="CInvalidRegionException">If <see cref="m_pRegion"/> is invalid.
/// </exception>
DWORD CMsgDefineRegion::Build(BYTE* buffer, DWORD bufferSize)
{
  if(m_bDefine) { return BuildDefine(buffer, bufferSize); }
  return BuildDelete(buffer, bufferSize);
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CMsgDefineRegion::Dump(MSXML2::IXMLDOMElement* pElem)
{
  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<bool>(pElem, L"m_bDefine", m_bDefine);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_pRegion", (DWORD)m_pRegion);
    } // if...

  }
  catch(...) {}
}

/// <summary>Constructs command bytes to define region.</summary>
/// <param name="buffer">Buffer to receive constructed bytes. If NULL, function
/// ignores all arguments and returns size of buffer required to contain the
/// command bytes.</param>
/// <param name="bufferSize">Size of <paramref name="buffer"/> in number of bytes.
/// If zero, function ignores all arguments and returns size of buffer required to
/// contain the command bytes.</param>
/// <returns>Length of constructed command bytes, or size of buffer required to
/// contain the command bytes if <paramref name="buffer"/> is NULL or
/// <paramref name="bufferSize"/> is zero.</returns>
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pRegion"/>
// is not assigned.</exception>
/// <exception cref="CInvalidRegionException">If <see cref="m_pRegion"/> is invalid.
/// </exception>
DWORD CMsgDefineRegion::BuildDefine(BYTE* buffer, DWORD bufferSize)
{
  BYTE byTmp;
  CMsgMgr mgr;
  DWORD len = 0;
  std::stringstream ss;
  std::string strTmp;
  std::string::const_iterator i;

  if(m_pRegion == NULL)
  {
    WCL_THROW_INVALIDOPERATIONEXCEPTION(L"no region assigned");
  }

  FILL_BUFFER(CMsgMgr::CMD_START);
  FILL_BUFFER('R');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  byTmp = mgr.RegionID2Drv(m_pRegion->m_nsID);
  FILL_BUFFER(byTmp);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  FILL_BUFFER('R');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  
  ss.str("");
  ss << m_pRegion->m_dwX;
  strTmp = ss.str();
  for(i = strTmp.begin();i != strTmp.end();i++) { FILL_BUFFER(*i); }
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  ss.str("");
  ss << m_pRegion->m_dwY;
  strTmp = ss.str();
  for(i = strTmp.begin();i != strTmp.end();i++) { FILL_BUFFER(*i); }
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  ss.str("");
  ss << m_pRegion->m_dwWidth;
  strTmp = ss.str();
  for(i = strTmp.begin();i != strTmp.end();i++) { FILL_BUFFER(*i); }
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  ss.str("");
  ss << m_pRegion->m_dwHeight;
  strTmp = ss.str();
  for(i = strTmp.begin();i != strTmp.end();i++) { FILL_BUFFER(*i); }
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  FILL_BUFFER('0');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  switch(m_pRegion->m_cRotation)
  {
  case print::CRegion::ROT_0 : FILL_BUFFER('0');break;
  case print::CRegion::ROT_90 : FILL_BUFFER('1');break;
  case print::CRegion::ROT_180 : FILL_BUFFER('2');break;
  case print::CRegion::ROT_270 : FILL_BUFFER('3');break;
  default : THROW_INVALIDREGIONEXCEPTION(L"invalid m_cRotate");
  } // switch...
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  switch(m_pRegion->m_cJustify)
  {
  case print::CRegion::JUSTIFY_LEFT : FILL_BUFFER('0');break;
  case print::CRegion::JUSTIFY_CENTER : FILL_BUFFER('1');break;
  case print::CRegion::JUSTIFY_RIGHT : FILL_BUFFER('2');break;
  default : THROW_INVALIDREGIONEXCEPTION(L"invalid m_cJustify");
  } // switch...
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  ss.str("");
  switch(m_pRegion->m_cType)
  {
  case print::CRegion::TYPE_FONT :
    ss << (DWORD)mgr.FontID2Drv(m_pRegion->m_nsTypeIndex);
    strTmp = ss.str();
    for(i = strTmp.begin();i != strTmp.end();i++) { FILL_BUFFER(*i); }
    break;
  case print::CRegion::TYPE_GRAPHIC :
    byTmp = mgr.GraphicID2Drv(m_pRegion->m_nsTypeIndex);
    FILL_BUFFER(byTmp);
    break;
  case print::CRegion::TYPE_BARCODE :
    byTmp = mgr.BarcodeID2Drv(m_pRegion->m_nsTypeIndex);
    FILL_BUFFER(byTmp);
    break;
  default : THROW_INVALIDREGIONEXCEPTION(L"invalid m_cType");
  } // switch...
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  FILL_BUFFER(m_pRegion->m_cMul1 + '0');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(m_pRegion->m_cMul2 + '0');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  switch(m_pRegion->m_cType)
  {
  case print::CRegion::TYPE_FONT :
    switch(m_pRegion->m_nsAttr)
    {
    case print::CRegion::TXTATTR_NORMAL : FILL_BUFFER('0');break;
    case print::CRegion::TXTATTR_INVERSE : FILL_BUFFER('1');break;
    default : THROW_INVALIDREGIONEXCEPTION(L"invalid m_nsAttr");
    } // switch...
    break;
  case print::CRegion::TYPE_GRAPHIC : FILL_BUFFER('0');break;
  case print::CRegion::TYPE_BARCODE :
    ss.str("");
    ss << m_pRegion->m_nsAttr;
    strTmp = ss.str();
    for(i = strTmp.begin();i != strTmp.end();i++) { FILL_BUFFER(*i); }
    break;
  default : THROW_INVALIDREGIONEXCEPTION(L"invalid m_cType");
  } // switch...
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  FILL_BUFFER('0');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(CMsgMgr::CMD_END);

  return len;
}

/// <summary>Constructs command bytes to delete region.</summary>
/// <param name="buffer">Buffer to receive constructed bytes. If NULL, function
/// ignores all arguments and returns size of buffer required to contain the
/// command bytes.</param>
/// <param name="bufferSize">Size of <paramref name="buffer"/> in number of bytes.
/// If zero, function ignores all arguments and returns size of buffer required to
/// contain the command bytes.</param>
/// <returns>Length of constructed command bytes, or size of buffer required to
/// contain the command bytes if <paramref name="buffer"/> is NULL or
/// <paramref name="bufferSize"/> is zero.</returns>
/// <exception cref="wcl::CInvalidOperationException">If <see cref="m_pRegion"/>
// is not assigned.</exception>
DWORD CMsgDefineRegion::BuildDelete(BYTE* buffer, DWORD bufferSize)
{
  BYTE byTmp;
  CMsgMgr mgr;
  DWORD len = 0;

  if(m_pRegion == NULL)
  {
    WCL_THROW_INVALIDOPERATIONEXCEPTION(L"no region assigned");
  }

  FILL_BUFFER(CMsgMgr::CMD_START);
  FILL_BUFFER('R');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  byTmp = mgr.RegionID2Drv(m_pRegion->m_nsID);
  FILL_BUFFER(byTmp);
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);

  FILL_BUFFER('D');
  FILL_BUFFER('R');
  FILL_BUFFER(CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(CMsgMgr::CMD_END);

  return len;
}
