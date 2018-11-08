#ifndef __GAME_MODEL_GO_ACTIONS_GO_GAME_ACTION_NOTATE_MANUALLY_H__
#define __GAME_MODEL_GO_ACTIONS_GO_GAME_ACTION_NOTATE_MANUALLY_H__
#include "go_game_action.h"

GAME_NAMESPACE_BEGIN

class GoActionNotateManually : public GoGameAction
{
	DECLARE_GO_GAME_ACTION_RUNTIME_INFO(GoActionNotateManually)

	virtual bool PropertyOfChangingBoard() const override { return false; }
};

GAME_NAMESPACE_END

#endif