#include "ui_chess_game_board.h"
#include <map>
#include "game/model/chess/actions/chess_game_action_factory.h"
#include "game/model/chess/chess_game_board.h"
#include "game/model/command_handler/command_handler.h"
#include "game/view/animation/ui_animation_layer.h"
#include "game/view/base/ui_chess_basket_base.h"
#include "game/view/base/ui_operational_layer.h"
#include "game/view/scale_line/ui_scale_line_layer.h"
#include "log/log.h"

GAME_NAMESPACE_BEGIN

UIChessGameBoard::UIChessGameBoard(QWidget* parent)
: UIGameBoardBase(parent)
{
	ForeachChess([this](UIChessPiece*& chess_piece){
		chess_piece = nullptr;
	});
}

void UIChessGameBoard::Init()
{
	this->InitModel();

	this->InitUI();
}

void UIChessGameBoard::InitModel()
{
	auto action_factory = std::make_shared<ChessGameActionFactory>();

	auto master_model = ChessGameBoard::Create();
	master_model->SetActionFacotry(action_factory);
	this->SetMasterModel(std::move(master_model));
}

void UIChessGameBoard::InitUI()
{
	setWindowFlags(Qt::FramelessWindowHint);
	setAutoFillBackground(true);

	QPalette palette;
	QPixmap pixmap(":/chess/images/chess/chess_board.png");
	palette.setBrush(backgroundRole(), QBrush(pixmap));
	setPalette(palette);

	this->setFixedSize(pixmap.size());

	this->CreateScaleLine();

	this->CreateChesses();
	this->UpdateChessesFromModel(master_model_);
}

void UIChessGameBoard::CreateChesses()
{
	for (int x = 0; x < Grid; ++x)
	{
		for (int y = 0; y < Grid; ++y)
		{
			auto& chess = chesses_[x][y];
			chess = new UIChessPiece(this);

			chess->setFixedSize(GridWidth(), GridWidth());

			chess->SetGridIndex(x, y);
			chess->move(this->PositionByGridIndex(chess->GetGridIndex()));

			chess->SetChessId(ChessInfo::EmptyId);
			chess->UpdateChess(UIChessPieceBase::UpdateReason::Forcely);
		}
	}
}

void UIChessGameBoard::CreateScaleLine()
{
	scale_line_layer_ = new UIScaleLineLayer(this);

	scale_line_layer_->setFixedSize(this->size());

	const int font_size = 12;
	const QSize size(20, 20);

	scale_line_layer_->SetXScaleLines(GridOffsetFromOrigin() + QPoint(GridWidth() * 0.5f - 1, -6), GridWidth(), { "A", "B", "C", "D", "E", "F", "G", "H" }, font_size, size);
	scale_line_layer_->SetYScaleLines(GridOffsetFromOrigin() + QPoint(-5, GridWidth() * 0.5f), GridWidth(), { "8", "7", "6", "5", "4", "3", "2", "1" }, font_size, size);

	scale_line_layer_->lower();
}

void UIChessGameBoard::UpdateChessesFromModel(const GameBoardPtr& model)
{
	if (!model) { return; }
	
	ForeachChess([&](UIChessPiece* chess_piece){
		if (chess_piece)
		{
			chess_piece->SetChessId(*model->ChessAt(chess_piece->GetGridIndex()));
		}
	});
}

void UIChessGameBoard::UpdateChessesFromModelAndUndoVariation(const GameBoardPtr& model, const GameActionResult& result)
{
	if (!model) { return; }

	std::map<GridIndex, ChessId>  map_chess_var;
	result.ForeachChessVariation([&map_chess_var](const ChessVariation& chess_var){
		map_chess_var.emplace(chess_var.GetGridIndex(), chess_var.GetOldChessId());
	});

	ForeachChess([&](UIChessPiece* chess_piece){
		if (chess_piece)
		{
			auto grid_index = chess_piece->GetGridIndex();
			auto itor = map_chess_var.find(grid_index);
			if (itor == map_chess_var.cend())
			{
				chess_piece->SetChessId(*model->ChessAt(grid_index));
			}
			else
			{
				chess_piece->SetChessId(itor->second);
			}
		}
	});
}

// triggered originally by AgoraMessageChannel (remotely, passively)
// call stack roughly as following:
// 1 -- MessageProcessor::SlotAgoraMessageChannel: receive raw data and parse it into CommandCell entities which aggregate in a CommandGroup
// 2 -- MessageProcessor::HandleCommandGroup: dispatch(notify) each Command entity
// 3 -- MessageCommandObserver::OnCommand: receive Command entity
// 4 -- UIGameBoardBase::HandleCommandOnMasterModel
// 5 -- GameBoardBase::DoCommand: genarate GameAction in terms of Command above
// 6 -- GameBoardBase::DoAction
// 7 -- GameActionWithResult::Do: do act on a concrete GameBoard to generate GameActionResult
// 8 -- GameBoardBase::NotifyGameActionDone
// 9 -- MasterModelObserver::OnGameActionDone
// 10 -- UIChessGameBoard::OnMasterDone
void UIChessGameBoard::OnMasterDone(const GameActionResult& result)
{
	bool play_this_result = UsageTypeIsPlayback() || command_handler_->TestSyncLocalWithMasterWhenMasterMoved(master_model_->BoardType(), result);

	if (play_this_result)
	{ 
		this->SetLocalDirty(true);
		
		if (!operational_layer_.isNull())
		{
			operational_layer_->Interrupt();
		}

		if (result.IsValid() && result.BoardIsChangedByAction())
		{
			if (animation_layer_) { animation_layer_->Clear(); }

			if (NeedToPlayVariationImmediately(result))
			{
				this->UpdateChessesFromModel(master_model_);
			}
			else
			{
				this->PlayMasterVariationOnAnimationLayer(result);
			}
		}
	}
}

// triggered originally by UIOperationalLayer::TryToMoveChess (locally, actively)
// call stack roughly as following:
// 1 -- UIOperationalLayer::TryToMoveChess
// 2 -- UIOperationalDelegate::TryToMoveChessLocally
// 3 -- UIGameBoardBase::TryToMoveChessLocally: generate Command
// 4 -- GameBoardBase::TryToDoCommand: genarate GameAction in terms of Command above
// 5 -- GameBoardBase::TryToDoAction
// 6 -- GameActionWithResult::Do: do act on a concrete GameBoard to generate GameActionResult
// 7 -- GameBoardBase::NotifyGameActionDone
// 8 -- LocalModelObserver::OnGameActionDone
// 9 -- UIChessGameBoard::OnLocalDone
void  UIChessGameBoard::OnLocalDone(const GameActionResult& result)
{
	assert(result.IsValid());

	operational_layer_->Clear();

	this->SendCommand(result.GetCommandPtr());

	if (result.BoardIsChangedByAction())
	{
		if (animation_layer_) { animation_layer_->Clear(); }

		if (NeedToPlayVariationImmediately(result))
		{
			this->UpdateChessesFromModel(local_model_);
		}
		else
		{
			this->PlayLocalVariationOnAnimationLayer(result);
		}
	}
}

void UIChessGameBoard::SyncLocalWithMaster()
{
	if (!master_model_ || !local_model_) { return; }

	LocalGameBoard()->SyncWith(*MasterGameBoard());

	UIGameBoardBase::SyncLocalWithMaster();
}

bool UIChessGameBoard::NeedToPlayVariationImmediately(const GameActionResult& result)
{
	bool ret = true;
	
	do 
	{
		if (animation_layer_.isNull()) { break; }

		const auto cmd_type = result.GetCommandTypeOfAction();
		if (cmd_type == proto::CommandType::StudentMoveChess || cmd_type == proto::CommandType::TeacherMoveChess)
		{
			ret = false;
		}

	} while (false);

	return ret;
}

void UIChessGameBoard::PlayMasterVariationOnAnimationLayer(const GameActionResult& result)
{
	this->UpdateChessesFromModelAndUndoVariation(master_model_, result);

	std::function<void()> callback = chess_move_callback_flag_.ToWeakCallback([=]{
		this->UpdateChessesFromModel(master_model_);

		result.ForeachChessVariation([=](const ChessVariation& chess_var){
			if (chess_var.IsCapturedPassively())
			{
				animation_layer_->PlayChessVariation(chess_var);
			}
		});
	});

	int moved_actively = 0;
	result.ForeachChessVariation([&](const ChessVariation& chess_var){
		if (chess_var.IsMovedActively())
		{
			moved_actively += 1;
			animation_layer_->PlayChessVariation(chess_var, callback, moved_actively == 1);

			callback = nullptr; // only callback once
		}
	});

	if (!moved_actively)
	{
		callback();
	}
}

void UIChessGameBoard::PlayLocalVariationOnAnimationLayer(const GameActionResult& result)
{
	this->UpdateChessesFromModelAndUndoVariation(local_model_, result);

	std::function<void()> callback = chess_move_callback_flag_.ToWeakCallback([=]{
		this->UpdateChessesFromModel(local_model_);

		result.ForeachChessVariation([=](const ChessVariation& chess_var){
			if (chess_var.IsCapturedPassively())
			{
				animation_layer_->PlayChessVariation(chess_var);
			}
		});
	});

	auto passing_point = operational_layer_->CurrentGlobalPosOfChessFaceCenter();
	
	int moved_actively = 0;
	result.ForeachChessVariation([&](const ChessVariation& chess_var){
		if (chess_var.IsMovedActively())
		{
			moved_actively += 1;
			animation_layer_->PlayChessVariationWithPassingPoint(chess_var, passing_point, callback, moved_actively == 1);

			callback = nullptr; // only callback once
		}
	});

	if (!moved_actively)
	{
		callback();
	}
}

void UIChessGameBoard::SetRuleBased(bool rule_based)
{
	auto local_board = LocalGameBoard();
	auto master_board = MasterGameBoard();

	if (local_board)
	{
		local_board->SetRuleBased(rule_based);
	}

	if (master_board)
	{
		master_board->SetRuleBased(rule_based);
	}
}

void UIChessGameBoard::OnOperationalLayerAssociated()
{
	auto local_model = ChessGameBoard::Create();

	local_model->SetActionFacotry(master_model_->ActionFacotry());
	local_model->SetRuleBased(MasterGameBoard()->IsRuleBased());

	this->SetLocalModel(std::move(local_model));
}

void UIChessGameBoard::OnLayoutChanged()
{
	ForeachChess([this](UIChessPiece* chess_piece){
		if (chess_piece)
		{
			chess_piece->move(this->PositionByGridIndex(chess_piece->GetGridIndex()));
		}
	});

	if (animation_layer_)
	{
		animation_layer_->RefreshArrowLine();
	}
}

QPoint UIChessGameBoard::PositionByGridIndex(const GridIndex& grid_index) const
{ 
	if (GetLayout() == GameBoardLayout::Flipped)
	{
		return GridOffsetFromOrigin() + QPoint(GridWidth() * (7 - grid_index.X()), GridWidth() * (7 - grid_index.Y()));
	}
	
	return GridOffsetFromOrigin() + QPoint(GridWidth() * grid_index.X(), GridWidth() * grid_index.Y());
}

void UIChessGameBoard::SetScaleLineVisible(bool visible)
{
	if (scale_line_layer_)
	{
		scale_line_layer_->setVisible(visible);
	}
}

void UIChessGameBoard::HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr)
{
	UIGameBoardBase::HandleMasterCommandSpecially(cmd_ptr);
}

ChessGameBoard* UIChessGameBoard::MasterGameBoard() const 
{ 
	return static_cast<ChessGameBoard*>(master_model_.get()); 
}

ChessGameBoard* UIChessGameBoard::LocalGameBoard() const
{
	return static_cast<ChessGameBoard*>(local_model_.get());
}

// override UIAnimationDelegate -- begin
QPixmap	UIChessGameBoard::MovingPixmapOfChessId(ChessId chess_id)
{
	return QPixmap(QString::fromStdString(ChessTrait::MovingImagePathById(chess_id)));
}

QPoint UIChessGameBoard::GlobalPositionOfGridCenterInBoard(const GridIndex& grid_index)
{
	auto grid_width_half = GridWidth() * 0.5f;
	return mapToGlobal(PositionByGridIndex(grid_index) + QPoint(grid_width_half, grid_width_half));
}

QPoint UIChessGameBoard::GlobalPositionOfChessCenterInBasket(ChessId chess_id)
{
	assert(chess_basket_);

	return chess_basket_->GlobalPositionOfChessCenter(chess_id);
}

void UIChessGameBoard::HideChessInBoardGrid(const GridIndex& grid_index)
{
	this->HideChessAt(grid_index);
}

int	 UIChessGameBoard::MilliSecondsOfMovingAnimation()
{
	return 200;
}
// override UIAnimationDelegate -- end

GAME_NAMESPACE_END