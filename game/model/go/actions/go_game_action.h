#ifndef __GAME_MODEL_GO_ACTIONS_GO_GAME_ACTION_H__
#define __GAME_MODEL_GO_ACTIONS_GO_GAME_ACTION_H__
#include "go_game_action_class_runntime_info.h"
#include "game/model/base/action/game_action_with_result.h"

GAME_NAMESPACE_BEGIN

class GameBoardBase;
class GoGameBoard;

class GoGameAction : public GameActionWithResult
{
public:
	using ClassRuntimeInfo = GoGameActionClassRuntimeInfo;

public:
	GoGameAction() = default;

	GoGameActionType				Type() const;
	virtual	std::string				ClassName() const override;

	virtual const ClassRuntimeInfo&	GetClassRuntimeInfo() const = 0;

protected:
	virtual	GameActionResult		DoOnBoard(GameBoardBase*) override final;
	virtual	GameActionResult		DoOnBoard(GoGameBoard*) = 0;
};

GAME_NAMESPACE_END

#endif