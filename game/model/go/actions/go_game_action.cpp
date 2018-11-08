#include "go_game_action.h"
#include "game/model/base/board/game_board_base.h"
#include "game/model/go/go_trait.h"
#include "game/model/go/go_game_board.h"

GAME_NAMESPACE_BEGIN

GoGameActionType GoGameAction::Type() const
{
	return this->GetClassRuntimeInfo().Type();
}

std::string GoGameAction::ClassName() const
{
	return this->GetClassRuntimeInfo().ClassName();
}

GameActionResult GoGameAction::DoOnBoard(GameBoardBase* board_base)
{
	auto go_board = dynamic_cast<GoGameBoard*>(board_base); // should be GoGameBoard

	assert(go_board);

	return this->DoOnBoard(go_board);
}

GAME_NAMESPACE_END