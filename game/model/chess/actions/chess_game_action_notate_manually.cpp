#include "chess_game_action_notate_manually.h"
#include "game/model/chess/chess_game_board.h"

GAME_NAMESPACE_BEGIN

IMPLEMENT_CHESS_GAME_ACTION_RUNTIME_INFO(ChessGameActionType::NotateManually, ChessActionNotateManually)

GameActionResult ChessActionNotateManually::DoOnBoard(ChessGameBoard* game_board)
{
	GameActionResult result;

	game_board->NotateManually();

	result.SetValid(true);

	this->AssociateWithActionResult(result);

	return result;
}

GAME_NAMESPACE_END