#ifndef __GAME_MODEL_GO_GO_GAME_BOARD_H__
#define __GAME_MODEL_GO_GO_GAME_BOARD_H__
#include "actions/go_action_friends.h"
#include "go_trait.h"
#include "game/model/base/board/game_board_base.h"
#include "game/model/base/chess/chess_info.h"
#include "game/model/base/grid/grid_index.h"
#include "game/model/go/go_record.h"

GAME_NAMESPACE_BEGIN

typedef std::shared_ptr<GoRecord> GoRecordPtr;

class GoGameBoard : public GameBoardBase
{
	GO_ACTION_FRIENDS

	typedef std::vector<GridIndex> GridIndexVector;

public:
	static std::unique_ptr<GoGameBoard> Create(int grid);

	bool				IsRuleBased() const;
	void				SetRuleBased(bool rule_based);

	void				IncreaseHistory(const GameActionResult&);
	void				RollbackHistory(int steps_to_rollback);

	void				SyncWith(const GoGameBoard& rh);

	ChessId				LastChessIdMovedIn() const;
	GridIndexVector		LastGridsTaken(ChessId* chess_id_taken = nullptr) const;

protected:
	GoGameBoard(int grid);

	void				Init();

	void				NewBoard(bool rule_based);
	void				ClearBoard();
	virtual	void		NotateManually() override;

	virtual	void		OnGameSwitched(const GameRecordPtr& last_game, const GameRecordPtr& current_game) override;

	void				SetBoardToOpening();

private:
	GoRecordPtr			current_record_;
};

GAME_NAMESPACE_END

#endif