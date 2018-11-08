#ifndef __GAME_MODEL_COMMAND_HANDLER_COMMAND_CONFLICT_LEVEL_H__
#define __GAME_MODEL_COMMAND_HANDLER_COMMAND_CONFLICT_LEVEL_H__
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

enum class CmdConflictLevel
{
	None,

	InsideOfCurrentGame, // commands conflict among: TeacherMoveChess, StudentMoveChess, TeacherClearBoard, TeacherBackwardOrForward

	WhenTeacherChangingGameBoard, // commands conflict among: TeacherChangeGameBoard, ...

	WhenTeacherNewingGameBoard, // commands conflict among: TeacherNewGameBoard, ...
};

GAME_NAMESPACE_END

#endif