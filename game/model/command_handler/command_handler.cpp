#include "command_handler.h"
#include <vector>
#include "game_board_wrapper.h"
#include "base_control/painter/draw_board.h"
#include "game/model/base/action/game_action_result.h"
#include "game/model/base/board/game_board_base.h"
#include "protocol/command/base/command_cell_with_draw.h"
#include "protocol/command/base/command_group_with_draw_list.h"
#include "protocol/command/commands/cmd_student_draw_end.h"
#include "protocol/command/commands/cmd_student_draw_move.h"
#include "protocol/command/commands/cmd_student_draw_start.h"
#include "protocol/command/commands/cmd_teacher_draw_end.h"
#include "protocol/command/commands/cmd_teacher_draw_move.h"
#include "protocol/command/commands/cmd_teacher_draw_start.h"
#include "protocol/command/commands/cmd_teacher_eraser_draw_down.h"
#include "protocol/command/commands/cmd_teacher_eraser_draw_move.h"
#include "protocol/command/commands/cmd_teacher_eraser_draw_up.h"

using namespace proto;

GAME_NAMESPACE_BEGIN

template<typename CmdTypes>
static inline bool CommandIsInRanges(CmdTypes&& cmd_types, const CommandSharedPtr& cmd_ptr)
{
	const auto target_type = cmd_ptr->Type();
	for (const auto& cmd_type : cmd_types)
	{
		if (cmd_type == target_type)
		{
			return true;
		}
	}

	return false;
}

static bool CommandIsRelatedToChess(const CommandSharedPtr& cmd_ptr)
{
	static CommandType cmd_types[] =
	{
		CommandType::TeacherChangeGameBoard,
		CommandType::TeacherMoveChess,
		CommandType::StudentMoveChess,
		CommandType::TeacherNewGameBoard,
		CommandType::TeacherNewGameBoardEx,
		CommandType::TeacherMarkManualChess,
		CommandType::ClearGameBoard,
	};

	return CommandIsInRanges(cmd_types, cmd_ptr);
}

static bool CommandIsNeededToBeHandledInGameBoard(const CommandSharedPtr& cmd_ptr)
{
	static CommandType cmd_types[] =
	{
		CommandType::TeacherMoveChess,
		CommandType::StudentMoveChess,
		CommandType::TeacherNewGameBoard,
		CommandType::TeacherNewGameBoardEx,
		CommandType::TeacherMarkManualChess,
		CommandType::ClearGameBoard,
	};

	return CommandIsInRanges(cmd_types, cmd_ptr);
}

static bool CommandIsInfluencingDrawBoard(const CommandSharedPtr& cmd_ptr)
{
	static CommandType cmd_types[] =
	{
		CommandType::TeacherChangeGameBoard,
		CommandType::TeacherChangeBoardPager,
		CommandType::TeacherChangePhotoPage,
		CommandType::TeacherChangeToPpt,

		CommandType::StudentChangeGameBoard,
		CommandType::StudentChangeBoardPager,
		CommandType::StudentChangePhotoPager,
		CommandType::StudentChangePptPager,

		CommandType::TeacherDrawGroup,
		CommandType::TeacherDrawStart,
		CommandType::TeacherDrawMove,
		CommandType::TeacherDrawEnd,

		CommandType::TeacherEraserGroup,
		CommandType::TeacherEraserDrawDown,
		CommandType::TeacherEraserDrawMove,
		CommandType::TeacherEraserDrawUp,

		CommandType::StudentDrawGroup,
		CommandType::StudentDrawStart,
		CommandType::StudentDrawMove,
		CommandType::StudentDrawEnd,
	};

	return CommandIsInRanges(cmd_types, cmd_ptr);
}

struct GameBoardPair
{
	GameBoardType	board_type_{ GameBoardType::None };

	GameBoardPtr	master_model_;
	GameBoardPtr	local_model_;
};

class GameBoardPairs : public std::vector<GameBoardPair>
{
public:
	inline void PushMaster(const GameBoardPtr& game_board)
	{
		auto pair_ptr = PairByType(game_board->BoardType());

		if (pair_ptr)
		{
			pair_ptr->master_model_ = game_board;
		}
		else
		{
			GameBoardPair pair;
			pair.board_type_ = game_board->BoardType();
			pair.master_model_ = game_board;

			push_back(pair);
		}
	}

	inline void PushLocal(const GameBoardPtr& game_board)
	{
		auto pair_ptr = PairByType(game_board->BoardType());

		if (pair_ptr)
		{
			pair_ptr->local_model_ = game_board;
		}
		else
		{
			GameBoardPair pair;
			pair.board_type_ = game_board->BoardType();
			pair.local_model_ = game_board;

			push_back(pair);
		}
	}

	inline GameBoardPtr MasterModelOfGameBoard(GameBoardType board_type)
	{
		auto pair_ptr = PairByType(board_type);

		return pair_ptr ? pair_ptr->master_model_ : GameBoardPtr();
	}

private:
	inline GameBoardPair* PairByType(GameBoardType board_type)
	{
		for (auto& pair : *this)
		{
			if (pair.board_type_ == board_type)
			{
				return &pair;
			}
		}

		return nullptr;
	}
};

class CommandCache : public std::vector<CommandSharedPtr>
{
public:
	inline void PushCommand(const CommandSharedPtr& cmd_ptr)
	{
		push_back(cmd_ptr);
	}

	inline void PopFrontCommand()
	{
		if (!empty())
		{
			erase(cbegin());
		}
	}

	inline CommandSharedPtr FrontCommand() const { return empty() ? CommandSharedPtr() : front(); }
};

class GameDrawBoardInfo
{
public:
	inline GameBoardType	BoardType() const { return board_type_; }
	inline void				SetDrawBoard(GameBoardType board_type, DrawBoard* draw_board)
	{
		board_type_ = board_type;
		draw_board_ = draw_board;
	}
	inline QPointer<DrawBoard> GetDrawBoard() const { return draw_board_; }

private:
	GameBoardType		board_type_{ GameBoardType::None };
	QPointer<DrawBoard> draw_board_;
};

CommandHandler::CommandHandler()
: board_pairs_(new GameBoardPairs)
, master_chess_cmd_cache_(new CommandCache)
, local_chess_cmd_cache_(new CommandCache)
, draw_board_by_tea_(new GameDrawBoardInfo)
, draw_board_by_stu_(new GameDrawBoardInfo)
{}

CommandHandler::~CommandHandler()
{
	delete board_pairs_;
	delete master_chess_cmd_cache_;
	delete local_chess_cmd_cache_;
}

void CommandHandler::SendCommand(const CommandSharedPtr& cmd_ptr)
{
	this->HandleLocalCommand(cmd_ptr);

	if (sender_) { sender_(cmd_ptr); }
}

void CommandHandler::SendDrawCommand(const DrawOpInfo& draw_info)
{
	CommandSharedPtr cmd_ptr;
	const bool is_tea = GetClientType() == ClientType::Teacher;

	if (draw_info.draw_op_type_ == DrawOpStart)
	{
		cmd_ptr = is_tea ? CmdTeacherDrawStart::CreateHomogeneously() : CmdStudentDrawStart::CreateHomogeneously();
	}
	else if (draw_info.draw_op_type_ == DrawOpMove)
	{
		cmd_ptr = is_tea ? CmdTeacherDrawMove::CreateHomogeneously() : CmdStudentDrawMove::CreateHomogeneously();
	}
	else if (draw_info.draw_op_type_ == DrawOpEnd)
	{
		cmd_ptr = is_tea ? CmdTeacherDrawEnd::CreateHomogeneously() : CmdStudentDrawEnd::CreateHomogeneously();
	}
	else if (draw_info.draw_op_type_ == EraserOpStart)
	{
		cmd_ptr = CmdTeacherEraserDrawDown::CreateHomogeneously();
	}
	else if (draw_info.draw_op_type_ == EraserOpMove)
	{
		cmd_ptr = CmdTeacherEraserDrawMove::CreateHomogeneously();
	}
	else if (draw_info.draw_op_type_ == EraserOpEnd)
	{
		cmd_ptr = CmdTeacherEraserDrawUp::CreateHomogeneously();
	}

	if (cmd_ptr)
	{
		cmd_ptr->PushFloatVariant(draw_info.x_);
		cmd_ptr->PushFloatVariant(draw_info.y_);

		this->SendCommand(cmd_ptr);
	}
}

void CommandHandler::SetGameBoardWrapper(GameBoardWrapper* game_board_wrapper)
{
	game_board_wrapper_ = game_board_wrapper;
}

void CommandHandler::SetClientAndUsageType(ClientType client_type, UsageType usage_type)
{
	client_type_ = client_type;
	usage_type = usage_type;
}

void CommandHandler::HandleLocalCommand(const CommandSharedPtr& cmd_ptr)
{
	if (CommandIsRelatedToChess(cmd_ptr))
	{
		local_chess_cmd_cache_->PushCommand(cmd_ptr);
	}
	else
	{}
}

void CommandHandler::HandleMasterCommand(const CommandSharedPtr& cmd_ptr)
{
	if (CommandIsRelatedToChess(cmd_ptr))
	{ 
		master_chess_cmd_cache_->PushCommand(cmd_ptr);

		//TODO: check commands conflict level here

		if (CommandIsNeededToBeHandledInGameBoard(cmd_ptr))
		{
			// first switch to target game board before new
			this->TryToSwitchGameBoardBeforeNew(cmd_ptr);

			auto master_board = board_pairs_->MasterModelOfGameBoard(current_board_type_);
			if (master_board) { master_board->DoCommand(cmd_ptr); }
		}
	}

	if (CommandIsInfluencingDrawBoard(cmd_ptr))
	{
		this->HandleMasterCommandInfluencingDrawBoard(cmd_ptr);
	}
	
	this->HandleMasterCommandSpecially(cmd_ptr);
}

CmdConflictLevel  CommandHandler::CheckCommandConflictBetweenMasterAndLocal(const CommandSharedPtr& master_cmd_ptr)
{
	auto conflict = CmdConflictLevel::None;

	// core thought:
	// check whether the master command is equal to first local command(if exist)
	// if equal: command sequence is OK
	// if not equal: command sequence is in conflict
	do
	{
		if (local_chess_cmd_cache_->empty()) { break; }

		auto front_local_command = local_chess_cmd_cache_->FrontCommand();

		if (front_local_command->EqualTo(*master_cmd_ptr)) { break; }

		const auto master_cmd_type = master_cmd_ptr->Type();

		switch (master_cmd_type)
		{
		case CommandType::TeacherChangeGameBoard:
			conflict = CmdConflictLevel::WhenTeacherChangingGameBoard;
			break;

		case CommandType::TeacherNewGameBoard:
		case CommandType::TeacherNewGameBoardEx:
			conflict = CmdConflictLevel::WhenTeacherNewingGameBoard;
			break;

		default:
			conflict = CmdConflictLevel::InsideOfCurrentGame;
			break;
		}

	} while (false);

	return conflict;
}

void CommandHandler::TryToSwitchGameBoardBeforeNew(const CommandSharedPtr& cmd_ptr)
{
	const auto cmd_type = cmd_ptr->Type();
	if (cmd_type == CommandType::TeacherNewGameBoard || cmd_type == CommandType::TeacherNewGameBoardEx)
	{
		const auto game_board_type = static_cast<GameBoardType>(cmd_ptr->IntOfVariantAt(0));
		this->SwitchToGameBoard(game_board_type);
		if (game_board_wrapper_) { game_board_wrapper_->SwithToGameBoard(game_board_type); }
	}
}

void CommandHandler::HandleMasterCommandInfluencingDrawBoard(const CommandSharedPtr& cmd_ptr)
{
	const auto cmd_tpye = cmd_ptr->Type();
	if (cmd_tpye == CommandType::TeacherChangeGameBoard)
	{
		const auto game_board_type = static_cast<GameBoardType>(cmd_ptr->IntOfVariantAt(0));

		this->SwitchToGameBoard(game_board_type);
		if (game_board_wrapper_) { game_board_wrapper_->SwithToGameBoard(game_board_type); }

		DrawBoard* draw_board = this->DrawBoardOnGameBoard(game_board_type);

		draw_board_by_tea_->SetDrawBoard(game_board_type, draw_board);
		draw_board_by_stu_->SetDrawBoard(game_board_type, draw_board);
	}
	else if (cmd_tpye == CommandType::TeacherChangeBoardPager || cmd_tpye == CommandType::TeacherChangePhotoPage || cmd_tpye == CommandType::TeacherChangeToPpt)
	{
		draw_board_by_tea_->SetDrawBoard(GameBoardType::None, nullptr);
		draw_board_by_stu_->SetDrawBoard(GameBoardType::None, nullptr);
	}
	else if (cmd_tpye == CommandType::StudentChangeGameBoard)
	{
		const auto game_board_type = static_cast<GameBoardType>(cmd_ptr->IntOfVariantAt(0));

		DrawBoard* draw_board = this->DrawBoardOnGameBoard(game_board_type);

		draw_board_by_stu_->SetDrawBoard(game_board_type, draw_board);
	}
	else if (cmd_tpye == CommandType::StudentChangeBoardPager || cmd_tpye == CommandType::StudentChangePhotoPager || cmd_tpye == CommandType::StudentChangePptPager)
	{
		draw_board_by_stu_->SetDrawBoard(GameBoardType::None, nullptr);
	}
	else if (cmd_tpye == CommandType::TeacherDrawGroup || cmd_tpye == CommandType::TeacherEraserGroup)
	{
		this->TeacherDraw(std::static_pointer_cast<CommandGroupWithDrawList>(cmd_ptr)->GetDrawList());
	}
	else if (cmd_tpye == CommandType::TeacherDrawStart || cmd_tpye == CommandType::TeacherDrawMove || cmd_tpye == CommandType::TeacherDrawEnd || 
		cmd_tpye == CommandType::TeacherEraserDrawDown || cmd_tpye == CommandType::TeacherEraserDrawMove || cmd_tpye == CommandType::TeacherEraserDrawUp )
	{
		this->TeacherDraw({ std::static_pointer_cast<CommandCellWithDraw>(cmd_ptr)->GetDrawInfo() });
	}
	else if (cmd_tpye == CommandType::StudentDrawGroup)
	{
		this->StudentDraw(std::static_pointer_cast<CommandGroupWithDrawList>(cmd_ptr)->GetDrawList());
	}
	else if (cmd_tpye == CommandType::StudentDrawStart || cmd_tpye == CommandType::StudentDrawMove || cmd_tpye == CommandType::StudentDrawEnd)
	{
		this->StudentDraw({ std::static_pointer_cast<CommandCellWithDraw>(cmd_ptr)->GetDrawInfo() });
	}
}

void CommandHandler::HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr)
{
	if (game_board_wrapper_) { game_board_wrapper_->HandleMasterCommandSpecially(cmd_ptr); }
}

void CommandHandler::TeacherDraw(const std::list<DrawOpInfo>& draw_list)
{
	if (!ClientTypeIsTeacher() || UsageTypeIsPlayback())
	{
		auto draw_board = draw_board_by_tea_->GetDrawBoard();

		if (!draw_board && draw_board_by_tea_->BoardType() != GameBoardType::None && game_board_wrapper_) // maybe not initialized if TeacherChangeGameBoard command lost
		{
			draw_board = game_board_wrapper_->DrawBoardOnGameBoard(draw_board_by_tea_->BoardType());
			draw_board_by_tea_->SetDrawBoard(draw_board_by_tea_->BoardType(), draw_board);
		}

		if (draw_board)
		{
			draw_board->OnTeaDrawInfos(draw_list);
		}
	}
}

void CommandHandler::StudentDraw(const std::list<DrawOpInfo>& draw_list)
{
	if (!ClientTypeIsStudent() || UsageTypeIsPlayback())
	{
		auto draw_board = draw_board_by_stu_->GetDrawBoard();

		if (!draw_board && draw_board_by_stu_->BoardType() != GameBoardType::None && game_board_wrapper_) // maybe not initialized if TeacherChangeGameBoard command lost
		{
			draw_board = game_board_wrapper_->DrawBoardOnGameBoard(draw_board_by_stu_->BoardType());
			draw_board_by_stu_->SetDrawBoard(draw_board_by_stu_->BoardType(), draw_board);
		}

		if (draw_board)
		{
			draw_board->OnStuDrawInfos(draw_list);
		}
	}
}

DrawBoard* CommandHandler::DrawBoardOnGameBoard(GameBoardType board_type)
{
	return game_board_wrapper_ ? game_board_wrapper_->DrawBoardOnGameBoard(board_type) : nullptr;
}

void CommandHandler::DelegateMasterGameBoard(const GameBoardPtr& game_board)
{
	if (!game_board) { return; }

	board_pairs_->PushMaster(game_board);

	if (current_board_type_ == GameBoardType::None)
	{
		current_board_type_ = game_board->BoardType();

		draw_board_by_tea_->SetDrawBoard(current_board_type_, nullptr);
		draw_board_by_stu_->SetDrawBoard(current_board_type_, nullptr);
	}
}

void CommandHandler::DelegateLocalGameBoard(const GameBoardPtr& game_board)
{
	if (!game_board) { return; }

	board_pairs_->PushLocal(game_board);
}

void CommandHandler::SwitchToGameBoard(GameBoardType board_type)
{
	if (current_board_type_ != board_type)
	{
		current_board_type_ = board_type;
	}
}

bool CommandHandler::TestSyncLocalWithMasterWhenMasterMoved(GameBoardType board_type, const GameActionResult& result_on_master)
{
	bool need_to_sync = true;

	// core thought:
	// check whether the master command with this GameActionResult is equal to first local command(if exist)
	// if equal: command sequence is OK, just pop first local command AND return
	// if not equal(meaning command sequence is in conflict) or not exist: need to sync local with master
	do
	{
		if (local_chess_cmd_cache_->empty()) { break; }

		auto front_local_command = local_chess_cmd_cache_->FrontCommand();

		auto master_command = result_on_master.GetCommandPtr();
		if (!master_command) { break; } // this condition should never happen

		if (front_local_command->EqualTo(*master_command))
		{
			need_to_sync = false;

			local_chess_cmd_cache_->PopFrontCommand();
		}
		else
		{
			need_to_sync = true; // command sequence is in MESS, so need to sync
		}

	} while (false);

	if (need_to_sync)
	{
		local_chess_cmd_cache_->clear();
	}

	return need_to_sync;
}

GAME_NAMESPACE_END