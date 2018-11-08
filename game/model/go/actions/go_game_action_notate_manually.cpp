#include "go_game_action_notate_manually.h"
#include "game/model/go/go_game_board.h"

GAME_NAMESPACE_BEGIN

IMPLEMENT_GO_GAME_ACTION_RUNTIME_INFO(GoGameActionType::NotateManually, GoActionNotateManually)

GameActionResult GoActionNotateManually::DoOnBoard(GoGameBoard* game_board)
{
	GameActionResult result;

	game_board->NotateManually();

	result.SetValid(true);

	this->AssociateWithActionResult(result);

	return result;
}

GAME_NAMESPACE_END