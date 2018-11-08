#include "chess_game_action_clear_board.h"
#include "chess_action_error.h"
#include "game/model/chess/chess_game_board.h"

GAME_NAMESPACE_BEGIN

IMPLEMENT_CHESS_GAME_ACTION_RUNTIME_INFO(ChessGameActionType::ClearBoard, ChessActionClearBoard)

GameActionResult ChessActionClearBoard::DoOnBoard(ChessGameBoard* game_board)
{
	GameActionResult result;

	do
	{
		if (game_board->IsRuleBased())
		{
			result.SetErrorInfo(chess_actoin_error::CannotClearBoardWithRule);
			break;
		}

		game_board->ClearBoard();

		result.SetValid(true);
	} while (false);

	if (result.IsValid())
	{
		this->AssociateWithActionResult(result);
	}

	return result;
}

GAME_NAMESPACE_END