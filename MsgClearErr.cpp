#include "stdafx.h"
#include "message.h"

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
DWORD CMsgClearErr::Build(BYTE* buffer, DWORD bufferSize)
{
  FILL_BUFFER(0, CMsgMgr::CMD_START);
  FILL_BUFFER(1, 'C');
  FILL_BUFFER(2, CMsgMgr::CMD_DELIMITER);
  FILL_BUFFER(3, CMsgMgr::CMD_END); 

  return 4;
}
