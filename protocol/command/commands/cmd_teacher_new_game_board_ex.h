#ifndef __PROTO_COMMANDS_CMD_TEACHER_NEW_GAME_BOARD_EX_H__
#define __PROTO_COMMANDS_CMD_TEACHER_NEW_GAME_BOARD_EX_H__
#include "../base/command_cell.h"

PROTO_NAMESPACE_BEGIN

class CmdTeacherNewGameBoardEx : public CommandCell
{
	DECLARE_COMMAND_RUNTIME_INFO(CmdTeacherNewGameBoardEx)
};

PROTO_NAMESPACE_END

#endif