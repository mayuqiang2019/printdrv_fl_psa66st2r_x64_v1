#include "stdafx.h"
#include "message.h"

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
DWORD CMsgFlashTransfer::Build(BYTE* buffer, DWORD bufferSize)
{
	if(buffer && (bufferSize >= 8))
	{
		buffer[0] = CMsgMgr::CMD_START;
		buffer[1] = CMsgMgr::CMD_FLASH_TRANSFER;
		buffer[2] = CMsgMgr::CMD_DELIMITER;
		buffer[3] = '$';
		buffer[4] = CMsgMgr::CMD_DELIMITER;
		buffer[5] = m_byPageID;
		buffer[6] = CMsgMgr::CMD_DELIMITER;
		buffer[7] = CMsgMgr::CMD_END;
	}

	return 8;
}
