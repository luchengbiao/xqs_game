#ifndef __PROTO_COMMANDS_CMD_TEACHER_NEW_GAME_BOARD_H__
#define __PROTO_COMMANDS_CMD_TEACHER_NEW_GAME_BOARD_H__
#include "../base/command_cell.h"

PROTO_NAMESPACE_BEGIN

class CmdTeacherNewGameBoard : public CommandCell
{
	DECLARE_COMMAND_RUNTIME_INFO(CmdTeacherNewGameBoard)
};

PROTO_NAMESPACE_END

#endif