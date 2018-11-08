#ifndef __GAME_MODEL_CHESS_ACTIONS_CHESS_GAME_ACTION_NOTATE_MANUALLY_H__
#define __GAME_MODEL_CHESS_ACTIONS_CHESS_GAME_ACTION_NOTATE_MANUALLY_H__
#include "chess_game_action.h"

GAME_NAMESPACE_BEGIN

class ChessActionNotateManually : public ChessGameAction
{
	DECLARE_CHESS_GAME_ACTION_RUNTIME_INFO(ChessActionNotateManually)

	virtual bool PropertyOfChangingBoard() const override { return false; }
};

GAME_NAMESPACE_END

#endif