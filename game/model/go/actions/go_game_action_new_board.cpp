#include "go_game_action_new_board.h"
#include "game/model/go/go_game_board.h"

GAME_NAMESPACE_BEGIN

IMPLEMENT_GO_GAME_ACTION_RUNTIME_INFO(GoGameActionType::NewBoard, GoActionNewBoard)

GameActionResult GoActionNewBoard::DoOnBoard(GoGameBoard* game_board)
{
	GameActionResult result;

	game_board->NewBoard(game_board->IsRuleBased());

	result.SetValid(true);

	this->AssociateWithActionResult(result);

	return result;
}

GAME_NAMESPACE_END