#ifndef __GAME_MODEL_CHESS_ACTIONS_CHESS_GAME_ACTION_MOVE_INSIDEOF_BOARD_H__
#define __GAME_MODEL_CHESS_ACTIONS_CHESS_GAME_ACTION_MOVE_INSIDEOF_BOARD_H__
#include "chess_game_action.h"
#include "game/model/base/action/game_action_move_base.h"

GAME_NAMESPACE_BEGIN

class ChessActionMoveInsideOfBoard : public GameActionMoveBase<ChessGameAction>
{
	DECLARE_CHESS_GAME_ACTION_RUNTIME_INFO(ChessActionMoveInsideOfBoard)

private:
	GameActionResult DoOnBoardWithoutRule(ChessGameBoard* game_board);
	GameActionResult DoOnBoardWithRule(ChessGameBoard* game_board);
};

GAME_NAMESPACE_END

#endif