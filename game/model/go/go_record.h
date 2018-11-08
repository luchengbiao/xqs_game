#ifndef __GAME_MODEL_GO_GO_RECORD_H__
#define __GAME_MODEL_GO_GO_RECORD_H__
#include <deque>
#include "game/model/base/action/game_action_result.h"
#include "game/model/base/chess/chess_info.h"
#include "game/model/base/grid/grid_index.h"
#include "game/model/base/record/custom_record.h"

GAME_NAMESPACE_BEGIN

typedef std::vector<GridIndex> GridIndexVector;

class GoMoveRecord
{
public:
	GoMoveRecord() = default;
	GoMoveRecord(GoMoveRecord&& rh);
	GoMoveRecord& operator=(GoMoveRecord&& rh);

	ChessId			chess_id_last_moved_in_{ ChessInfo::EmptyId };
	GridIndex		grid_last_moved_in_;

	ChessId			chess_id_last_taken_{ ChessInfo::EmptyId };
	GridIndexVector	grids_last_taken_;
};

class GoRecord : public CustomRecord
{
public:
	GoRecord(bool rule_based);
	GoRecord(const GoRecord&);
	GoRecord& operator=(const GoRecord&);

	bool						IsRuleBased() const { return rule_based_; }

	void						Increase(const GameActionResult& result);
	void						Rollback(const int steps_to_rollback);

	ChessId						LastChessIdMovedIn() const;
	GridIndexVector				LastGridsTaken(ChessId* chess_id_taken = nullptr) const;

private:
	void						SetRuleBased(const bool rule_based) { rule_based_ = rule_based; }

private:
	bool						rule_based_{ false };
	std::deque<GoMoveRecord>	go_moves_;

	friend class GoGameBoard;
};

GAME_NAMESPACE_END

#endif