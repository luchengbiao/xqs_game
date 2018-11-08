#include "chess_game_action_move_into_board.h"
#include "chess_action_error.h"
#include "game/model/chess/chess_game_board.h"
#include "game/model/chess/chess_trait.h"

GAME_NAMESPACE_BEGIN

IMPLEMENT_CHESS_GAME_ACTION_RUNTIME_INFO(ChessGameActionType::MoveIntoBoard, ChessActionMoveIntoBoard)

GameActionResult ChessActionMoveIntoBoard::DoOnBoard(ChessGameBoard* game_board)
{
	GameActionResult result;

	do 
	{
		if (game_board->IsRuleBased())
		{
			result.SetErrorInfo(chess_actoin_error::CannotMovePieceIntoBoardActivelyWithRule);
			break;
		}

		auto ope_chess_id = this->ChessId();
		auto grid_to = this->GridTo();

		ChessInfoAdapter chess_info_to(game_board->ChessAt(grid_to));

		if (!chess_info_to)
		{
			result.SetErrorInfo(chess_actoin_error::ParamErrorOnToGrid);
			break;
		}

		if (chess_info_to.IsNotEmpty())
		{
			result.SetErrorInfo(chess_actoin_error::ExistingChessOnToGrid);
			break;
		}

		{
			chess_info_to.SetId(ope_chess_id);

			result.AddChessVariation({ GridIndex::InvalidGrid, grid_to, chess_info_to, ChessVariation::Reason::MovedIntoBoardActively });
		}

		result.SetValid(true);
	} while (false);

	if (result.IsValid())
	{
		this->AssociateWithActionResult(result);
	}

	return result;
}

GAME_NAMESPACE_END