#ifndef __GAME_MODEL_CHESS_CHESS_GAME_BOARD_H__
#define __GAME_MODEL_CHESS_CHESS_GAME_BOARD_H__
#include <map>
#include "actions/chess_action_friends.h"
#include "stockfish/adapter/position_adapter_fwd.h"
#include "game/model/base/board/game_board.h"
#include "game/model/chess/chess_record.h"

GAME_NAMESPACE_BEGIN

typedef std::shared_ptr<ChessRecord> ChessRecordPtr;

class ChessGameBoard : public GameBoard<8>
{
	CHESS_ACTION_FRIENDS

public:
	static std::unique_ptr<ChessGameBoard> Create();

	bool						IsRuleBased() const;
	void						SetRuleBased(bool rule_based);

	void						SyncWith(const ChessGameBoard& rh);

	std::map<GridIndex, bool>	GridsReachableFrom(const GridIndex& grid_from) const;

protected:
	ChessGameBoard();

	void						Init();

	void						NewBoard(bool rule_based);
	void						ClearBoard();

	virtual	void				NotateManually() override;

	inline PositionAdapterPtr&	CurrentRuleBoard() const { return current_record_->rule_board_; }
	virtual	void				OnGameSwitched(const GameRecordPtr& last_game, const GameRecordPtr& current_game) override;

	void						SetBoardToOpening();

	std::map<GridIndex, bool>	GridsReachableFromWithRule(const GridIndex& grid_from) const;
	std::map<GridIndex, bool>	GridsReachableFromWithoutRule(const GridIndex& grid_from) const;

private:
	ChessRecordPtr				current_record_;
};

GAME_NAMESPACE_END

#endif