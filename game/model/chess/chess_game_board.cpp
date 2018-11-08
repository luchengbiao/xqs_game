#include "chess_game_board.h"
#include <assert.h>
#include "actions/chess_game_action_move_insideof_board.h"
#include "game/model/chess/chess_board_info_adapter_for_rule.h"
#include "game/model/chess/chess_trait.h"
#include "game/model/chess/simple_path_rule/chess_path_rule.h"
#include "game/model/chess/stockfish_converter.h"
#include "game/model/base/action/game_action.h"
#include "stockfish/adapter/position_adapter.h"

GAME_NAMESPACE_BEGIN

std::unique_ptr<ChessGameBoard> ChessGameBoard::Create()
{
	auto game_board = new (std::nothrow) ChessGameBoard();
	if (game_board)
	{
		game_board->Init();

		return std::unique_ptr<ChessGameBoard>(game_board);
	}

	return std::unique_ptr<ChessGameBoard>();
}

ChessGameBoard::ChessGameBoard()
: GameBoard(GameBoardType::Chess)
{
	current_record_ = std::make_shared<ChessRecord>(false);
	CurrentCustomRecord() = current_record_;
}

void ChessGameBoard::Init()
{
	this->SetBoardToOpening();
}

void ChessGameBoard::SetBoardToOpening()
{
	this->EmptyAllChesses();

	ChessType chess_types[MyXGrid] =
	{
		ChessType::Rook,
		ChessType::Knight,
		ChessType::Bishop,
		ChessType::Queen,
		ChessType::King,
		ChessType::Bishop,
		ChessType::Knight,
		ChessType::Rook,
	};

	for (int i = 0; i < MyXGrid; ++i)
	{
		ChessAt(i, 0)->SetId(ChessTrait::IdOfBlackType(chess_types[i]));
		ChessAt(i, 1)->SetId(ChessTrait::IdOfBlackPawn());

		ChessAt(i, MyYGrid - 1)->SetId(ChessTrait::IdOfWhiteType(chess_types[i]));
		ChessAt(i, MyYGrid - 2)->SetId(ChessTrait::IdOfWhitePawn());
	}
}

bool ChessGameBoard::IsRuleBased() const 
{ 
	return current_record_->IsRuleBased();
}

void ChessGameBoard::SetRuleBased(bool rule_based) 
{
	if (rule_based && !current_record_->IsRuleBased() && CurrentActionIndices()->empty())  // only support false-true change and no action on board yet
	{
		current_record_->SetRuleBased(true);
	}
}

void ChessGameBoard::SyncWith(const ChessGameBoard& rh)
{
	if (&rh == this) { return; }

	*current_record_ = *rh.current_record_;
}

void ChessGameBoard::NewBoard(bool rule_based)
{
	current_record_ = std::make_shared<ChessRecord>(rule_based);
	this->CreateAndSwitchToNewGame(current_record_);

	this->SetBoardToOpening();
}

void ChessGameBoard::ClearBoard()
{
	this->EmptyAllChesses();
}

void ChessGameBoard::NotateManually()
{
	// require: the back action should be ChessActionMoveInsideOfBoard
	if (!CurrentActionIndices()->empty()) 
	{
		int back_index = CurrentActionIndices()->back().Index();
		auto move_action = std::dynamic_pointer_cast<ChessActionMoveInsideOfBoard>(GameActionByIndex(back_index));

		if (move_action)
		{
			auto move_notation = ChessTrait::NotationOfMove(move_action->ChessId(), move_action->GridFrom(), move_action->GridTo());
			auto board_notation = ChessTrait::NotationOfBoard(GetChesses());

			this->PushNotation(Notation(move_notation, board_notation));
		}
	}
}

std::map<GridIndex, bool>  ChessGameBoard::GridsReachableFrom(const GridIndex& grid_from) const
{
	return IsRuleBased() ? GridsReachableFromWithRule(grid_from) : GridsReachableFromWithoutRule(grid_from);
}

std::map<GridIndex, bool>  ChessGameBoard::GridsReachableFromWithoutRule(const GridIndex& grid_from) const
{
	BoardInfoAdapter board_info(*this);
	return chess_path_rule::GridsReachableFrom(board_info, grid_from);
}

std::map<GridIndex, bool>  ChessGameBoard::GridsReachableFromWithRule(const GridIndex& grid_from) const
{
	assert(CurrentRuleBoard());

	std::map<GridIndex, bool> grids;
	auto moves = CurrentRuleBoard()->CurrentLegalMoves();

	if (!moves.empty())
	{
		auto square_from = stk_cvter::GridToSquare(grid_from);

		for (const auto& move : moves)
		{
			if (move.square_from_ == square_from)
			{
				bool captured = (move.is_captured_ || move.is_short_castling_ || move.is_long_castling_);

				grids.emplace(stk_cvter::SquareToGrid(move.square_to_), captured);
			}
		}
	}

	return grids;
}

void ChessGameBoard::OnGameSwitched(const GameRecordPtr&, const GameRecordPtr&)
{
	current_record_ = std::dynamic_pointer_cast<ChessRecord>(CurrentCustomRecord());

	assert(current_record_);
}

GAME_NAMESPACE_END