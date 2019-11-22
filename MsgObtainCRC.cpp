#include "stdafx.h"
#include "message.h"

/// <summary>Constructor.</summary>
CMsgObtainCRC::CMsgObtainCRC() : m_dwStartAddr(0), m_wSeed(0)
{

}

#define FILL_BUFFER(i, c) if((buffer != NULL) && (bufferSize > i))\
                          {\
                            buffer[i] = c;\
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
DWORD CMsgObtainCRC::Build(BYTE* buffer, DWORD bufferSize)
{
  union
  {
    DWORD value;
    BYTE array[4];
  } convert1;
  union
  {
    WORD value;
    BYTE array[2];
  } convert2;

  FILL_BUFFER(0, CMsgMgr::CMD_START);
  FILL_BUFFER(1, 'G');
  FILL_BUFFER(2, CMsgMgr::CMD_DELIMITER);
  
  convert1.value = m_dwStartAddr;
  FILL_BUFFER(3, convert1.array[0]);
  FILL_BUFFER(4, convert1.array[1]);
  FILL_BUFFER(5, convert1.array[2]);
  FILL_BUFFER(6, convert1.array[3]);

  convert2.value = m_wSeed;
  FILL_BUFFER(7, convert2.array[0]);
  FILL_BUFFER(8, convert2.array[1]);

  FILL_BUFFER(9, CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(10, CMsgMgr::CMD_END);

  return 11;
}

/// <summary>Dumps object's state into XML DOM element for debug purposes.</summary>
/// <param name="pElem">Pointer to XML DOM element.</param>
void CMsgObtainCRC::Dump(MSXML2::IXMLDOMElement* pElem)
{
  try
  {

    if(pElem != NULL)
    {
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_dwStartAddr", m_dwStartAddr);
      wcl::CDumpHelper::DumpAttr<DWORD>(pElem, L"m_wSeed", m_wSeed);
    } // if...

  }
  catch(...) {}
}

