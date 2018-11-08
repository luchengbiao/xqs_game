#include "go_game_board.h"
#include "actions/go_game_action_move_into_board.h"
#include "game/model/base/action/game_action.h"
#include "game/model/go/go_trait.h"

GAME_NAMESPACE_BEGIN

std::unique_ptr<GoGameBoard> GoGameBoard::Create(int grid)
{
	auto game_board = new (std::nothrow) GoGameBoard(grid);
	if (game_board)
	{
		game_board->Init();

		return std::unique_ptr<GoGameBoard>(game_board);
	}

	return std::unique_ptr<GoGameBoard>();
}

GoGameBoard::GoGameBoard(int grid)
: GameBoardBase(GoTrait::GoBoardInfoByGrid(grid).board_type_, grid, grid)
{
	current_record_ = std::make_shared<GoRecord>(false);
	CurrentCustomRecord() = current_record_;
}

void GoGameBoard::Init()
{
	this->SetBoardToOpening();
}

void GoGameBoard::SetBoardToOpening()
{
	this->EmptyAllChesses();
}

bool GoGameBoard::IsRuleBased() const
{
	return current_record_->IsRuleBased();
}

void GoGameBoard::SetRuleBased(bool rule_based)
{
	if (rule_based && !current_record_->IsRuleBased() && CurrentActionIndices()->empty())  // only support false-true change and no action on board yet
	{
		current_record_->SetRuleBased(true);
	}
}

void GoGameBoard::IncreaseHistory(const GameActionResult& result)
{
	if (result.BoardIsChangedByAction() && result.ActionIsInnerActionInOneGame())
	{
		current_record_->Increase(result);
	}
}

void GoGameBoard::RollbackHistory(int steps_to_rollback)
{
	current_record_->Rollback(steps_to_rollback);
}

void GoGameBoard::SyncWith(const GoGameBoard& rh)
{
	if (&rh == this) { return; }

	*current_record_ = *(rh.current_record_);
}

ChessId GoGameBoard::LastChessIdMovedIn() const
{
	return current_record_->LastChessIdMovedIn();
}

GoGameBoard::GridIndexVector GoGameBoard::LastGridsTaken(ChessId* chess_id_taken) const
{
	return current_record_->LastGridsTaken(chess_id_taken);
}

void GoGameBoard::NewBoard(bool rule_based)
{
	current_record_ = std::make_shared<GoRecord>(rule_based);
	this->CreateAndSwitchToNewGame(current_record_);

	this->SetBoardToOpening();
}

void GoGameBoard::ClearBoard()
{
	this->EmptyAllChesses();
}

void GoGameBoard::NotateManually()
{
	// require: the back action should be GoActionMoveIntoBoard
	if (!CurrentActionIndices()->empty()) 
	{
		int back_index = CurrentActionIndices()->back().Index();
		auto move_action = std::dynamic_pointer_cast<GoActionMoveIntoBoard>(GameActionByIndex(back_index));

		if (move_action)
		{
			//auto move_notation = GoTrait::NotationOfMove(move_action->ChessId(), move_action->GridFrom(), move_action->GridTo());
			//auto board_notation = GoTrait::NotationOfBoard(GetChesses());

			//this->PushNotation(Notation(move_notation, board_notation));
		}
	}
}

void GoGameBoard::OnGameSwitched(const GameRecordPtr&, const GameRecordPtr&)
{
	current_record_ = std::dynamic_pointer_cast<GoRecord>(CurrentCustomRecord());

	assert(current_record_);
}

GAME_NAMESPACE_END