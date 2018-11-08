#include "go_record.h"

GAME_NAMESPACE_BEGIN

GoMoveRecord::GoMoveRecord(GoMoveRecord&& rh)
{
	*this = std::move(rh);
}

GoMoveRecord& GoMoveRecord::operator=(GoMoveRecord&& rh)
{
	if (&rh != this)
	{
		chess_id_last_moved_in_ = rh.chess_id_last_moved_in_;
		grid_last_moved_in_ = rh.grid_last_moved_in_;
		chess_id_last_taken_ = rh.chess_id_last_taken_;
		grids_last_taken_ = std::move(rh.grids_last_taken_);

		rh.chess_id_last_moved_in_ = ChessInfo::EmptyId;
		rh.grid_last_moved_in_ = GridIndex::InvalidGrid;
		rh.chess_id_last_taken_ = ChessInfo::EmptyId;
	}

	return (*this);
}

GoRecord::GoRecord(bool rule_based)
: rule_based_(rule_based)
{}

GoRecord::GoRecord(const GoRecord& rh)
{
	*this = rh;
}

GoRecord& GoRecord::operator=(const GoRecord& rh)
{
	if (&rh != this)
	{
		rule_based_ = rh.rule_based_;
		go_moves_ = rh.go_moves_;
	}

	return (*this);
}

void GoRecord::Increase(const GameActionResult& result)
{
	GoMoveRecord go_move;

	result.ForeachChessVariation([&](const ChessVariation& chess_var){
		if (chess_var.IsMovedIntoBoardActively())
		{
			go_move.chess_id_last_moved_in_ = chess_var.GetNewChessId();
			go_move.grid_last_moved_in_ = chess_var.GetGridIndex();
		}
		else if (chess_var.IsCapturedPassively() && ChessIdIsEmpty(chess_var.GetNewChessId()) && ChessIdIsNotEmpty(chess_var.GetOldChessId()))
		{
			go_move.chess_id_last_taken_ = chess_var.GetOldChessId();
			go_move.grids_last_taken_.push_back(chess_var.GetGridIndex());
		}
	});

	go_moves_.emplace_back(std::move(go_move));
}

void GoRecord::Rollback(const int steps_to_rollback)
{}

ChessId GoRecord::LastChessIdMovedIn() const
{
	return go_moves_.empty() ? ChessInfo::EmptyId : go_moves_.back().chess_id_last_moved_in_;
}

GridIndexVector GoRecord::LastGridsTaken(ChessId* chess_id_taken) const
{
	if (!go_moves_.empty())
	{
		if (chess_id_taken) { *chess_id_taken = go_moves_.back().chess_id_last_taken_; }

		return go_moves_.back().grids_last_taken_;
	}

	return GridIndexVector();
}

GAME_NAMESPACE_END