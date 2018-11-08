#ifndef __GAME_MODEL_BASE_BOARD_GAME_BOARD_BASE_H__
#define __GAME_MODEL_BASE_BOARD_GAME_BOARD_BASE_H__
#include <vector>
#include "base/framework/observer_list.h"
#include "game_board_type.h"
#include "game_board_base_fwd.h"
#include "game/model/base/chess/chess_info_matrix.h"
#include "game/model/base/notation/notation.h"
#include "game/model/base/record/game_record.h"
#include "protocol/command/base/command.h"

GAME_NAMESPACE_BEGIN

class ChessInfo;
class GameActionFactory;
class GameActionResult;
class GameBoardObserver;
class GridIndex;

typedef ObserverList<GameBoardObserver>			GameBoardObserverList;
typedef std::unique_ptr<GameBoardObserverList>	GameBoardObserverListPtr;
typedef std::shared_ptr<GameActionFactory>		GameActionFactoryPtr;

class GameBoardBase
{
public:
	GameBoardBase(const GameBoardBase&) = delete;
	GameBoardBase& operator=(const GameBoardBase&) = delete;

	virtual ~GameBoardBase() = default;

	inline GameBoardType				BoardType() const { return board_type_; }

	void								SetActionFacotry(const GameActionFactoryPtr& game_action_factory);
	GameActionFactoryPtr				ActionFacotry() const { return game_action_factory_; }

	void								DoCommand(const CommandSharedPtr& cmd_ptr, bool cache_and_notify = true);
	bool								TryToDoCommand(const CommandSharedPtr& cmd_ptr, bool cache_and_notify_if_valid = true);

	void								Undo();
	void								Redo();

	void								NotifyGameActionDone(const GameActionResult&);
	void								NotifyGameActionUndone(const GameActionResult&);
	void								NotifyGameActionRedone(const GameActionResult&);

	void								AddGameObserver(GameBoardObserver* observer);
	void								RemoveGameObserver(GameBoardObserver* observer);

	int									ChessRows() const;
	int									ChessColumns() const;

	ChessInfo*							ChessAt(const GridIndex&) const;
	ChessInfo*							ChessAt(int x, int y) const;
	const ChessInfoMatrix&				GetChesses() const;
	template<int GridX, int GridY = GridX>
	void								ChessIds(ChessIdArr2D<GridX, GridY>& arr2d) const;

	void								SyncWith(const GameBoardBase&);

	GameActionPtr						GameActionByIndex(int index) const;

	virtual void						NotateManually() = 0;
	void								PushNotation(const Notation& notation);

protected:
	GameBoardBase(GameBoardType board_type, int rows, int cols);

	void								DoAction(const GameActionPtr& action, bool cache_and_notify);
	bool								TryToDoAction(const GameActionPtr& action, bool cache_and_notify_if_valid);
	void								CacheAndNotifyGameActionDone(const GameActionPtr& action, const GameActionResult& result);

	inline ChessMatrixPtr&				CurrentChesses() const { return current_game_->chesses_; }
	inline GameActionIndicesPtr&		CurrentActionIndices() const { return current_game_->action_indices_; }
	inline NotationsPtr&				CurrentNotations() const { return current_game_->notations_; }
	inline CustomRecordPtr&				CurrentCustomRecord() const { return current_game_->custom_record_; }

	void								CreateAndSwitchToNewGame(const CustomRecordPtr& custom_record);
	void								SwitchToGame(int index);
	virtual	void						OnGameSwitched(const GameRecordPtr& last_game, const GameRecordPtr& current_game);

	void								EmptyAllChesses();

protected:
	const GameBoardType					board_type_{ GameBoardType::None };
	const int							rows_{ 0 };
	const int							cols_{ 0 };

	GameActionsPtr						all_actions_;
	GameRecordsPtr						all_games_;

	GameRecordPtr						last_game_;
	GameRecordPtr						current_game_; // tuple of <current_chesses, current_actions, current_notations, ...>

	GameActionFactoryPtr				game_action_factory_;
	GameBoardObserverListPtr			game_observers_;
};

template<int GridX, int GridY>
void GameBoardBase::ChessIds(ChessIdArr2D<GridX, GridY>& arr2d) const
{
	CurrentChesses()->ChessIds(arr2d);
}

GAME_NAMESPACE_END

#endif