#ifndef __GAME_MODEL_CHESS_CHESS_RECORD_H__
#define __GAME_MODEL_CHESS_CHESS_RECORD_H__
#include "game/model/base/record/custom_record.h"
#include "stockfish/adapter/position_adapter_fwd.h"

GAME_NAMESPACE_BEGIN

class ChessRecord : public CustomRecord
{
public:
	ChessRecord(bool rule_based);
	ChessRecord(const ChessRecord&);
	ChessRecord& operator=(const ChessRecord&);

	bool				IsRuleBased() const { return rule_based_; }

	PositionAdapterPtr	RuleBoard() const { return rule_board_; }

private:
	void				SetRuleBased(const bool rule_based);

private:
	bool				rule_based_{ false };
	PositionAdapterPtr	rule_board_{ nullptr };

	friend class ChessGameBoard;
};

GAME_NAMESPACE_END

#endif