#include "ui_game_board_base.h"
#include "game/model/base/board/game_board_base.h"
#include "game/model/base/board/game_board_observer.h"
#include "game/model/command_handler/command_handler.h"
#include "game/view/animation/ui_animation_layer.h"
#include "game/view/base/chess_piece/ui_chess_piece_base.h"
#include "game/view/base/ui_chess_basket_base.h"
#include "game/view/base/ui_operational_layer.h"
#include "protocol/command/commands/cmd_student_move_chess.h"
#include "protocol/command/commands/cmd_teacher_move_chess.h"

GAME_NAMESPACE_BEGIN

class MasterModelObserver : public GameBoardObserver
{
	friend class UIGameBoardBase;

	MasterModelObserver(UIGameBoardBase* game_board)
		: game_board_(game_board)
	{}

	virtual void OnGameActionDone(const GameActionResult& result) override
	{
		game_board_->OnMasterDone(result);
	}

	virtual void OnGameActionUndone(const GameActionResult& result) override
	{
		game_board_->OnMasterUndone(result);
	}

	virtual void OnGameActionRedone(const GameActionResult& result) override
	{
	}

	UIGameBoardBase* game_board_;
};

class LocalModelObserver : public GameBoardObserver
{
	friend class UIGameBoardBase;

	LocalModelObserver(UIGameBoardBase* game_board)
		: game_board_(game_board)
	{}

	virtual void OnGameActionDone(const GameActionResult& result) override
	{
		game_board_->OnLocalDone(result);
	}

	virtual void OnGameActionUndone(const GameActionResult& result) override
	{
		game_board_->OnLocalRetracted(result);
	}

	virtual void OnGameActionRedone(const GameActionResult& result) override
	{
	}

	UIGameBoardBase* game_board_;
};

static bool Game_Resource_Inited{ false };

UIGameBoardBase::UIGameBoardBase(QWidget* parent)
: QWidget(parent)
{
	if (!Game_Resource_Inited)
	{
		Q_INIT_RESOURCE(game);
		Game_Resource_Inited = true;
	}
}

UIGameBoardBase::~UIGameBoardBase()
{
	if (master_model_observer_)
	{
		if (master_model_)
		{
			master_model_->RemoveGameObserver(master_model_observer_);
		}
		
		delete master_model_observer_;
	}

	if (local_model_observer_)
	{
		if (local_model_)
		{
			local_model_->RemoveGameObserver(local_model_observer_);
		}
		
		delete local_model_observer_;
	}
}

void  UIGameBoardBase::Init()
{}

void UIGameBoardBase::SetMasterModel(const GameBoardPtr& model)
{
	if (master_model_ && master_model_observer_)
	{
		master_model_->RemoveGameObserver(master_model_observer_);
	}

	master_model_ = model;

	if (!master_model_observer_)
	{
		master_model_observer_ = new MasterModelObserver(this);
	}

	if (master_model_)
	{
		master_model_->AddGameObserver(master_model_observer_);
	}

	if (command_handler_)
	{
		command_handler_->DelegateMasterGameBoard(master_model_);
	}
}

void UIGameBoardBase::SetLocalModel(const GameBoardPtr& model)
{
	if (local_model_ && local_model_observer_)
	{
		local_model_->RemoveGameObserver(local_model_observer_);
	}

	local_model_ = model;

	if (!local_model_observer_)
	{
		local_model_observer_ = new LocalModelObserver(this);
	}
	
	if (local_model_)
	{
		local_model_->AddGameObserver(local_model_observer_);
	}

	if (command_handler_)
	{
		command_handler_->DelegateLocalGameBoard(local_model_);
	}
}

void UIGameBoardBase::SyncLocalWithMaster()
{
	if (!master_model_ || !local_model_) { return; }

	local_model_->SyncWith(*master_model_);
}

void UIGameBoardBase::OnMasterDone(const GameActionResult&)
{}

void UIGameBoardBase::OnMasterUndone(const GameActionResult&)
{}

void UIGameBoardBase::OnLocalDone(const GameActionResult&)
{}

void UIGameBoardBase::OnLocalRetracted(const GameActionResult&)
{}

void UIGameBoardBase::SetCommandHandler(const CommandHandlerPtr& command_handler)
{
	command_handler_ = command_handler;

	command_handler_->DelegateMasterGameBoard(master_model_);

	command_handler_->DelegateLocalGameBoard(local_model_);
}

void UIGameBoardBase::AssociateAnimationLayer(UIAnimationLayer* animation_layer)
{
	animation_layer_ = animation_layer;
}

void UIGameBoardBase::AssociateChessBasket(UIChessBasketBase* chess_basket)
{
	chess_basket_ = chess_basket;
}

void UIGameBoardBase::AssociateOperationalLayer(UIOperationalLayer* operational_layer)
{
	operational_layer_ = operational_layer;

	this->OnOperationalLayerAssociated();
}

bool UIGameBoardBase::TryToHandleCommandOnLocalModel(const CommandSharedPtr& cmd_ptr)
{
	return local_model_ ? local_model_->TryToDoCommand(cmd_ptr) : false;
}

void UIGameBoardBase::TryToRefreshLocal()
{
	if (IsLocalDirty())
	{
		SyncLocalWithMaster();

		SetLocalDirty(false);
	}
}

bool UIGameBoardBase::TryToMoveChessLocally(ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to)
{
	bool ret = false;

	CommandSharedPtr cmd_ptr;

	if (client_type_ == ClientType::Student)
	{
		cmd_ptr = proto::CmdStudentMoveChess::Create();
	}
	else if (client_type_ == ClientType::Teacher)
	{
		cmd_ptr = proto::CmdTeacherMoveChess::Create();
	}

	if (cmd_ptr)
	{
		cmd_ptr->PushIntVariant(chess_id);
		cmd_ptr->PushIntVariant(grid_from.X());
		cmd_ptr->PushIntVariant(grid_from.Y());
		cmd_ptr->PushIntVariant(grid_to.X());
		cmd_ptr->PushIntVariant(grid_to.Y());

		ret = TryToHandleCommandOnLocalModel(cmd_ptr);
		if (!ret && operational_layer_)
		{
			operational_layer_->Clear();
		}
	}

	return ret;
}

void UIGameBoardBase::SendCommand(const CommandSharedPtr& cmd_ptr)
{
	if (command_handler_)
	{
		command_handler_->SendCommand(cmd_ptr);
	}
}

ChessId UIGameBoardBase::LocalChessIdAt(const GridIndex& grid_from) const
{
	auto chess_info = local_model_? local_model_->ChessAt(grid_from) : nullptr;

	return chess_info? chess_info->GetId() : ChessInfo::EmptyId;
}

bool UIGameBoardBase::CanOperateOnBoardNow() const
{
	if (!animation_layer_.isNull() && animation_layer_->HasAnimationPlaying())
	{
		return false;
	}

	return true;
}

void UIGameBoardBase::HideChessAt(const GridIndex& grid_index)
{
	auto chess_piece = this->ChessPieceAt(grid_index);
	if (chess_piece)
	{
		chess_piece->HideFace();
	}
}

void UIGameBoardBase::ShowChessAt(const GridIndex& grid_index)
{
	auto chess_piece = this->ChessPieceAt(grid_index);
	if (chess_piece && chess_piece->ChessIdIsNotEmpty())
	{
		chess_piece->ShowFace();
	}
}

void UIGameBoardBase::SetLayout(const GameBoardLayout layout)
{
	if (layout_ != layout)
	{
		layout_ = layout;

		this->OnLayoutChanged();
	}
}

void UIGameBoardBase::HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr)
{
	const auto cmd_type = cmd_ptr->Type();

	if (cmd_type == proto::CommandType::TeacherShowBoardRule)
	{
		this->SetScaleLineVisible(static_cast<bool>(cmd_ptr->IntOfVariantAt(0)));
	}
}

GAME_NAMESPACE_END