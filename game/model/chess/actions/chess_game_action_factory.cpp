#include "chess_game_action_factory.h"
#include <assert.h>
#include <string>
#include "chess_game_action_class_runntime_info.h"
#include "chess_game_action_clear_board.h"
#include "chess_game_action_move_into_board.h"
#include "chess_game_action_move_outof_board.h"
#include "chess_game_action_move_insideof_board.h"
#include "chess_game_action_new_board.h"
#include "chess_game_action_new_board_ex.h"
#include "chess_game_action_notate_manually.h"
#include "protocol/command/base/command.h"

using proto::CommandType;

static void RegisterChessGameActionClassRuntimeInfo()
{
	REGISTER_CHESS_GAME_ACTION_RUNTIME_INFO(game::ChessActionMoveIntoBoard);
	REGISTER_CHESS_GAME_ACTION_RUNTIME_INFO(game::ChessActionMoveOutOfBoard);
	REGISTER_CHESS_GAME_ACTION_RUNTIME_INFO(game::ChessActionMoveInsideOfBoard);
	REGISTER_CHESS_GAME_ACTION_RUNTIME_INFO(game::ChessActionClearBoard);
	REGISTER_CHESS_GAME_ACTION_RUNTIME_INFO(game::ChessActionNewBoard);
	REGISTER_CHESS_GAME_ACTION_RUNTIME_INFO(game::ChessActionNewBoardEx);
	REGISTER_CHESS_GAME_ACTION_RUNTIME_INFO(game::ChessActionNotateManually);
}

static ChessGameActionClassRuntimeInfo*& FristChessGameActionClassRuntimeInfoPointer()
{
	static ChessGameActionClassRuntimeInfo* fristChessGameActionClassRuntimeInfoPointer = nullptr;

	return fristChessGameActionClassRuntimeInfoPointer;
}

ChessGameActionClassRuntimeInfo::ClassRuntimeInfoWithType(game::ChessGameActionType type, const std::string& className, PFuncChessGameActionCreator creator)
: _type(type), _className(className), _creator(creator), _next(FristChessGameActionClassRuntimeInfoPointer())
{
	ChessGameActionClassRuntimeInfo*& firstPtr = FristChessGameActionClassRuntimeInfoPointer();

#ifdef _DEBUG
	if (firstPtr && firstPtr->Type() == this->Type())
	{
		assert(false);
	}
#endif

	firstPtr = this;
}

GAME_NAMESPACE_BEGIN

ChessGameActionFactory::ChessGameActionFactory()
{
	RegisterChessGameActionClassRuntimeInfo();
}

std::shared_ptr<ChessGameAction> ChessGameActionFactory::CreateGameActionWithType(ChessGameActionType type) const
{
	std::shared_ptr<ChessGameAction> shared_ptr;

	ChessGameActionClassRuntimeInfo* ptr = FristChessGameActionClassRuntimeInfoPointer();

	while (ptr)
	{
		if (ptr->Type() == type)
		{
			shared_ptr = ptr->Create();
			break;
		}

		ptr = ptr->Next();
	}

	return shared_ptr;
}

static bool IsMovedIntoBoard(const proto::Command& cmd)
{
	return (cmd.IntOfVariantAt(1) == -1) && (cmd.IntOfVariantAt(2) == -1) && (cmd.IntOfVariantAt(3) >= 0) && (cmd.IntOfVariantAt(4) >= 0);
}

static bool IsMovedOutOfBoard(const proto::Command& cmd)
{
	return (cmd.IntOfVariantAt(1) >= 0) && (cmd.IntOfVariantAt(2) >= 0) && (cmd.IntOfVariantAt(3) == -1) && (cmd.IntOfVariantAt(4) == -1);
}

static bool IsMovedInsdieOfBoard(const proto::Command& cmd)
{
	return (cmd.IntOfVariantAt(1) >= 0) && (cmd.IntOfVariantAt(2) >= 0) && (cmd.IntOfVariantAt(3) >= 0) && (cmd.IntOfVariantAt(4) >= 0);
}

using CommandPredictor = std::function<bool(const proto::Command&)>;

static const struct
{
	const CommandType			cmd_type_;
	const CommandPredictor		predictor_;
	const ChessGameActionType	act_type_;
}
Command_Action_Map[] = 
{
	{ CommandType::TeacherMoveChess,		IsMovedIntoBoard,		ChessGameActionType::MoveIntoBoard },
	{ CommandType::StudentMoveChess,		IsMovedIntoBoard,		ChessGameActionType::MoveIntoBoard },
	{ CommandType::TeacherMoveChess,		IsMovedOutOfBoard,		ChessGameActionType::MoveOutOfBoard },
	{ CommandType::StudentMoveChess,		IsMovedOutOfBoard,		ChessGameActionType::MoveOutOfBoard },
	{ CommandType::TeacherMoveChess,		IsMovedInsdieOfBoard,	ChessGameActionType::MoveInsideOfBoard },
	{ CommandType::StudentMoveChess,		IsMovedInsdieOfBoard,	ChessGameActionType::MoveInsideOfBoard },
	{ CommandType::ClearGameBoard,			nullptr,				ChessGameActionType::ClearBoard },
	{ CommandType::TeacherNewGameBoard,		nullptr,				ChessGameActionType::NewBoard },
	{ CommandType::TeacherNewGameBoardEx,	nullptr,				ChessGameActionType::NewBoardEx },
	{ CommandType::TeacherMarkManualChess,	nullptr,				ChessGameActionType::NotateManually },
};

std::shared_ptr<GameAction> ChessGameActionFactory::CreateActionWithCommand(const CommandSharedPtr& cmd_ptr)
{
	std::shared_ptr<ChessGameAction> action_ptr;

	const auto cmd_type = cmd_ptr->Type();
	for (const auto& ca : Command_Action_Map)
	{
		if (cmd_type == ca.cmd_type_ && (!ca.predictor_ || ca.predictor_(*cmd_ptr)))
		{
			action_ptr = this->CreateGameActionWithType(ca.act_type_);
			break;
		}
	}

	if (action_ptr)
	{
		action_ptr->SetCommandPtr(cmd_ptr);
	}

	return action_ptr;
}

GAME_NAMESPACE_END