#include "stdafx.h"
#include "message.h"

/// <summary>Retrieves message type.</summary>
/// <param name="msg">Message.</param>
/// <param name="msgLen">Size of <paramref name="msg"/>, in number of bytes.</param>
/// <returns>Message type.</returns>
char CMsgMgr::GetType(BYTE* msg, DWORD msgLen)
{
  if(msgLen >= 2)
  {
    switch(msg[1])
    {
    case CMD_DEFINE_TEMPL :
    case CMD_DEFINE_REGION :
    case CMD_PRINT :
    case CMD_LIBMANAGE :
    case CMD_CRC :
    case CMD_STATUS :
    case CMD_FEED :
      return msg[1];
    } // switch...
  } // if...

  return CMD_UNKNOWN;
}

/// <summary>Converts template ID to driver's range.</summary>
/// <param name="id">Template ID.</param>
/// <returns>Template ID mapped to driver's range.</returns>
/// <exception cref="wcl::CArgumentException">If <paramref name="id"/> is out
/// of range.</exception>
/// <remarks>This printer use [0x30~0x39, 0x41, 0x42, 0x4E] as predefined template,
/// use [0x3A~0x40, 0x43~0x5D, 0x5F~0x7B, 0x7D] as user-defined template. Thus
/// this function map 0~99 to predefined range by truncating value > 11 to 11,
// e.g. 0 will mapped to 0x30, and 11, 12, 13 all will be mapped to 0x42.
/// And 100~999 is mapped to user-defined range in similar manner.</remarks>
BYTE CMsgMgr::TemplID2Drv(short id)
{
  BYTE byTmp;
  
  if((id < 0) || (id > 999))
  {
    WCL_THROW_ARGUMENTEXCEPTION(L"id", L"must between 0 to 999");
  }

  if(!IsUserDefinedTempl(id))
  {
    byTmp = (BYTE)__min(id, 11);
    if((byTmp >= 0) && (byTmp <= 9)) { return byTmp + 0x30; }
	else if(id == 'N')	{ return 0x4E; }	// for template N, NSW 
    return byTmp + 0x37;
  }

  // else if(id >= 100)
  byTmp = (BYTE)__min(id - 100, 63);
  if((byTmp >= 0) && (byTmp <= 6)) { return byTmp + 0x3A; }
  else if((byTmp >= 7) && (byTmp <= 33)) { return byTmp + 0x3C; }
  else if((byTmp >= 34) && (byTmp <= 62)) { return byTmp + 0x3D; }

  return 0x7D;
}

/// <summary>Converts template ID to driver's range.</summary>
/// <param name="id">Template ID.</param>
/// <returns>Template ID mapped to driver's range.</returns>
/// <exception cref="wcl::CArgumentException">If <paramref name="id"/> is out
/// of range.</exception>
/// <remarks>This printer use [0x31~0x39, 0x41~0x47, 0x49~0x4C, 0x4E~0x55, 0x58,
/// 0x5A, 0x61~0x71] as predefined region, use [0x30, 0x3A~0x40, 0x48, 0x4D,
/// 0x56~0x57, 0x59, 0x5B~0x5D, 0x5F~0x60, 0x72~0x7B, 0x7D] as user-defined template. Thus
/// this function map 0~99 to the predefined range by truncating value > 46 to 46,
/// e.g. 0 will be mapeed to 0x31, and 46, 47, 48 all will be mapped to 0x71.
/// And 100~999 is mapped to user-defined range in similar manner.</remarks>
BYTE CMsgMgr::RegionID2Drv(short id)
{
  BYTE byTmp;
  
  if((id < 0) || (id > 999))
  {
    WCL_THROW_ARGUMENTEXCEPTION(L"id", L"must between 0 to 999");
  }

  if(id < 100)
  {
    byTmp = (BYTE)__min(id, 46);
    if((byTmp >= 0) && (byTmp <= 8)) { return byTmp + 0x31; }
    else if((byTmp >= 9) && (byTmp <= 15)) { return byTmp + 0x38; }
    else if((byTmp >= 16) && (byTmp <= 19)) { return byTmp + 0x39; }
    else if((byTmp >= 20) && (byTmp <= 27)) { return byTmp + 0x3A; }
    else if(byTmp == 28) { return 0x58; }
    else if(byTmp == 29) { return 0x5A; }
    return byTmp + 0x43;
  } // if...

  // else if(id >= 100)
  byTmp = (BYTE)__min(id - 100, 28);
  if(byTmp == 0) { return 0x30; }
  else if((byTmp >= 1) && (byTmp <= 7)) { return byTmp + 0x39; }
  else if(byTmp == 8) { return 0x48; }
  else if(byTmp == 9) { return 0x4D; }
  else if((byTmp >= 10) && (byTmp <= 11)) { return byTmp + 0x4C; }
  else if(byTmp == 12) { return 0x59; }
  else if((byTmp >= 13) && (byTmp <= 15)) { return byTmp + 0x4E; }
  else if((byTmp >= 16) && (byTmp <= 17)) { return byTmp + 0x4F; }
  else if((byTmp >= 18) && (byTmp <= 27)) { return byTmp + 0x60; }

  return 0x7D;
}

