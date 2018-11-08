#ifndef __GAME_MODEL_GO_ACTIONS_GO_GAME_ACTION_MOVE_OUTOF_BOARD_H__
#define __GAME_MODEL_GO_ACTIONS_GO_GAME_ACTION_MOVE_OUTOF_BOARD_H__
#include "go_game_action.h"
#include "game/model/base/action/game_action_move_base.h"

GAME_NAMESPACE_BEGIN

class GoActionMoveOutOfBoard : public GameActionMoveBase<GoGameAction>
{
	DECLARE_GO_GAME_ACTION_RUNTIME_INFO(GoActionMoveOutOfBoard)
};

GAME_NAMESPACE_END

#endif