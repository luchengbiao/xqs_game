#include "chess_game_action_new_board_ex.h"
#include "game/model/chess/chess_game_board.h"

GAME_NAMESPACE_BEGIN

IMPLEMENT_CHESS_GAME_ACTION_RUNTIME_INFO(ChessGameActionType::NewBoardEx, ChessActionNewBoardEx)

GameActionResult ChessActionNewBoardEx::DoOnBoard(ChessGameBoard* game_board)
{
	GameActionResult result;

	game_board->NewBoard(command_ptr_->IntOfVariantAt(2) == 1);

	result.SetValid(true);

	this->AssociateWithActionResult(result);

	return result;
}

GAME_NAMESPACE_END