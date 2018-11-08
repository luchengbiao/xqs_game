#include "chess_record.h"
#include "stockfish/adapter/position_adapter.h"

GAME_NAMESPACE_BEGIN

ChessRecord::ChessRecord(bool rule_based)
: rule_based_(rule_based)
{
	if (rule_based_)
	{
		rule_board_ = std::make_shared<stk::PositionAdapter>();
	}
}

ChessRecord::ChessRecord(const ChessRecord& rh)
{
	*this = rh;
}

ChessRecord& ChessRecord::operator=(const ChessRecord& rh)
{
	if (&rh != this)
	{
		rule_based_ = rh.rule_based_;

		if (rule_board_ && rh.rule_board_)
		{
			*rule_board_ = *(rh.rule_board_);
		}
	}

	return (*this);
}

void ChessRecord::SetRuleBased(const bool rule_based)
{
	if (rule_based && !rule_based_) // only support false-true change
	{
		rule_based_ = true;

		rule_board_ = std::make_shared<stk::PositionAdapter>();
	}
}

GAME_NAMESPACE_END