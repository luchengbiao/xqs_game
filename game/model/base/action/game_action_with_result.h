#ifndef __GAME_MODEL_BASE_ACTION_GAME_ACTION_WITH_RESULT_H__
#define __GAME_MODEL_BASE_ACTION_GAME_ACTION_WITH_RESULT_H__
#include <memory>
#include "game_action.h"
#include "game_action_result.h"
#include "game/model/base/board/game_board_base_fwd.h"
#include "protocol/command/base/command_fwd.h"

GAME_NAMESPACE_BEGIN

class GameActionWithResult : public GameAction, public std::enable_shared_from_this<GameActionWithResult>
{
	using GameActionResultPtr = std::unique_ptr<GameActionResult>;

public:
	GameActionWithResult() = default;

	GameActionWithResult(const GameActionWithResult&) = delete;
	GameActionWithResult& operator=(const GameActionWithResult&) = delete;

	virtual	GameActionResult			Do(GameBoardBase*) override;
	virtual	void						Undo(GameBoardBase*) override;
	virtual	void						Redo(GameBoardBase*) override;

	virtual bool						PropertyOfChangingBoard() const override { return true; }
	virtual bool						PropertyOfBeingInnerActionInOneGame() const override { return true; }

	virtual std::string					ClassName() const { return "GameActionWithResult"; }

	void								SetCommandPtr(const CommandSharedPtr&);
	CommandSharedPtr					GetCommandPtr() const;

	void								AssociateWithActionResult(GameActionResult& result);

protected:
	virtual	GameActionResult			DoOnBoard(GameBoardBase*) = 0;

protected:
	CommandSharedPtr					command_ptr_;
	GameActionResultPtr					result_ptr_;
};

GAME_NAMESPACE_END

#endif