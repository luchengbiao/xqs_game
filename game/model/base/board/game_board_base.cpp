#include "game_board_base.h"
#include "game/model/base/action/game_action.h"
#include "game/model/base/action/game_action_facotry.h"
#include "game/model/base/action/game_action_result.h"
#include "game/model/base/board/game_board_observer.h"
#include "log/log.h"

#define PRINT_THE_NUMBER_OF_EXISTING_ACTIONS()  //CHECK_THE_NUMBER_OF_EXISTING_ACTIONS()

GAME_NAMESPACE_BEGIN

GameBoardBase::GameBoardBase(GameBoardType board_type, int rows, int cols)
: board_type_(board_type)
, rows_(rows)
, cols_(cols)
, all_actions_(std::make_shared<GameActions>())
, all_games_(std::make_shared<GameRecords>())
, game_observers_(new GameBoardObserverList()) // compiler not support std::make_unique yet
{
	assert(IsValidBoardType(board_type) && rows_ > 0 && cols_ > 0);

	this->CreateAndSwitchToNewGame(nullptr);
}

void GameBoardBase::CreateAndSwitchToNewGame(const CustomRecordPtr& custom_record)
{
	last_game_ = current_game_;
	current_game_ = std::make_shared<GameRecord>();
	all_games_->emplace_back(current_game_);

	CurrentChesses() = std::make_shared<ChessInfoMatrix>(rows_, cols_);
	CurrentActionIndices() = std::make_shared<GameActionIndices>();
	CurrentNotations() = std::make_shared<Notations>();
	CurrentCustomRecord() = custom_record;
}

void GameBoardBase::SwitchToGame(int index)
{
	if (index >= 0 && index < all_games_->size())
	{
		auto last_game = current_game_;

		current_game_ = all_games_->at(index);

		if (last_game != current_game_)
		{
			last_game_ = last_game;
			
			this->OnGameSwitched(last_game_, current_game_);
		}
	}
}

void GameBoardBase::OnGameSwitched(const GameRecordPtr& last_game_, const GameRecordPtr& current_game)
{}

void GameBoardBase::AddGameObserver(GameBoardObserver* observer)
{
	game_observers_->AddObserver(observer);
}

void GameBoardBase::RemoveGameObserver(GameBoardObserver* observer)
{
	game_observers_->RemoveObserver(observer);
}

int GameBoardBase::ChessRows() const
{
	return CurrentChesses()->Rows();
}

int GameBoardBase::ChessColumns() const
{
	return CurrentChesses()->Columns();
}

ChessInfo* GameBoardBase::ChessAt(const GridIndex& grid) const
{
	return CurrentChesses()->ChessAt(grid.X(), grid.Y());
}

ChessInfo* GameBoardBase::ChessAt(int x, int y) const
{
	return CurrentChesses()->ChessAt(x, y);
}

const ChessInfoMatrix& GameBoardBase::GetChesses() const
{
	return *CurrentChesses();
}

void GameBoardBase::EmptyAllChesses()
{
	CurrentChesses()->ForeachChess([](ChessInfo* chess){
		if (chess)
		{
			chess->SetToEmpty();
		}
	});
}

void GameBoardBase::SyncWith(const GameBoardBase& game_board)
{
	if (this == &game_board) { return; }

	*all_actions_ = *game_board.all_actions_;

	CurrentChesses()->CopyChessesFrom(*game_board.CurrentChesses());

	if (CurrentActionIndices() && game_board.CurrentActionIndices())
	{
		*CurrentActionIndices() = *game_board.CurrentActionIndices();
	}
}

void GameBoardBase::SetActionFacotry(const GameActionFactoryPtr& game_action_factory)
{
	game_action_factory_ = game_action_factory;
}

void GameBoardBase::DoCommand(const CommandSharedPtr& cmd_ptr, bool cache_and_notify)
{
	if (game_action_factory_)
	{
		PRINT_THE_NUMBER_OF_EXISTING_ACTIONS();

		GameActionPtr action_ptr = game_action_factory_->CreateActionWithCommand(cmd_ptr);
		if (action_ptr)
		{
			this->DoAction(action_ptr, cache_and_notify);
		}

		PRINT_THE_NUMBER_OF_EXISTING_ACTIONS();
	}
}

bool GameBoardBase::TryToDoCommand(const CommandSharedPtr& cmd_ptr, bool cache_and_notify_if_valid)
{
	if (game_action_factory_)
	{
		PRINT_THE_NUMBER_OF_EXISTING_ACTIONS();

		GameActionPtr action_ptr = game_action_factory_->CreateActionWithCommand(cmd_ptr);
		if (action_ptr)
		{
			return this->TryToDoAction(action_ptr, cache_and_notify_if_valid);
		}

		PRINT_THE_NUMBER_OF_EXISTING_ACTIONS();
	}

	return false;
}

void GameBoardBase::DoAction(const GameActionPtr& action, bool cache_and_notify)
{
	auto result = action->Do(this);
	if (cache_and_notify)
	{
		this->CacheAndNotifyGameActionDone(action, result);
	}
}

bool GameBoardBase::TryToDoAction(const GameActionPtr& action, bool cache_and_notify_if_valid)
{
	auto result = action->Do(this);
	if (result.IsValid())
	{
		if (cache_and_notify_if_valid)
		{
			this->CacheAndNotifyGameActionDone(action, result);
		}

		return true;
	}

	return false;
}

void GameBoardBase::CacheAndNotifyGameActionDone(const GameActionPtr& action, const GameActionResult& result)
{
	all_actions_->emplace_back(action);

	if (action->PropertyOfBeingInnerActionInOneGame()) // some action is not belonging to one game, such as new game(board) action
	{
		CurrentActionIndices()->emplace_back(all_actions_->size() - 1);
	}

	this->NotifyGameActionDone(result);
}

void GameBoardBase::Undo()
{
}

void GameBoardBase::Redo()
{
}

GameActionPtr GameBoardBase::GameActionByIndex(int index) const
{
	return (index >= 0 && index < all_actions_->size()) ? all_actions_->at(index) : GameActionPtr();
}

void GameBoardBase::PushNotation(const Notation& notation)
{
	CurrentNotations()->push_back(notation);
}

void GameBoardBase::NotifyGameActionDone(const GameActionResult& result)
{
	game_observers_->NotifyObservers(&GameBoardObserver::OnGameActionDone, result);
}

void GameBoardBase::NotifyGameActionUndone(const GameActionResult& result)
{
	game_observers_->NotifyObservers(&GameBoardObserver::OnGameActionUndone, result);
}

void GameBoardBase::NotifyGameActionRedone(const GameActionResult& result)
{
	game_observers_->NotifyObservers(&GameBoardObserver::OnGameActionRedone, result);
}

GAME_NAMESPACE_END

