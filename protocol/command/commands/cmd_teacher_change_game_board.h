#ifndef __PROTO_COMMANDS_CMD_TEACHER_CHANGE_GAME_BOARD_H__
#define __PROTO_COMMANDS_CMD_TEACHER_CHANGE_GAME_BOARD_H__
#include "../base/command_cell.h"

PROTO_NAMESPACE_BEGIN

class CmdTeacherChangeGameBoard : public CommandCell
{
	DECLARE_COMMAND_RUNTIME_INFO(CmdTeacherChangeGameBoard)
};

PROTO_NAMESPACE_END

#endif