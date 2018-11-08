#include "go_game_action_clear_board.h"
#include "game/model/go/go_game_board.h"
#include "go_action_error.h"

GAME_NAMESPACE_BEGIN

IMPLEMENT_GO_GAME_ACTION_RUNTIME_INFO(GoGameActionType::ClearBoard, GoActionClearBoard)

GameActionResult GoActionClearBoard::DoOnBoard(GoGameBoard* game_board)
{
	GameActionResult result;

	do
	{
		if (game_board->IsRuleBased())
		{
			result.SetErrorInfo(go_actoin_error::CannotClearBoardWithRule);
			break;
		}

		game_board->ClearBoard();

		result.SetValid(true);
	} while (false);

	if (result.IsValid())
	{
		this->AssociateWithActionResult(result);
		game_board->IncreaseHistory(result);
	}

	return result;
}

GAME_NAMESPACE_END