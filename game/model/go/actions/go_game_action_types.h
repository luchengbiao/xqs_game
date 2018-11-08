#ifndef __GAME_MODEL_GO_ACTIONS_GO_GAME_ACTION_TYPES_H__
#define __GAME_MODEL_GO_ACTIONS_GO_GAME_ACTION_TYPES_H__
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

enum class GoGameActionType
{
	MoveIntoBoard,
	MoveInsideOfBoard,
	MoveOutOfBoard,
	NewBoard,
	NewBoardEx,
	ClearBoard,
	NotateManually,
};

GAME_NAMESPACE_END

#endif