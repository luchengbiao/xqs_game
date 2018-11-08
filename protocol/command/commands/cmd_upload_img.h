#ifndef __PROTO_COMMANDS_CMD_UPLOAD_IMG_H__
#define __PROTO_COMMANDS_CMD_UPLOAD_IMG_H__
#include "../base/command_cell.h"

PROTO_NAMESPACE_BEGIN

class CmdUploadImg : public CommandCell
{
	DECLARE_COMMAND_RUNTIME_INFO(CmdUploadImg)
};

PROTO_NAMESPACE_END

#endif