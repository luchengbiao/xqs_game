#include "ui_go_game_board.h"
#include <map>
#include <math.h>
#include "common/uiutils/ui_utils.h"
#include "game/model/go/actions/go_game_action_factory.h"
#include "game/model/go/go_game_board.h"
#include "game/model/command_handler/command_handler.h"
#include "game/view/animation/ui_animation_layer.h"
#include "game/view/base/ui_chess_basket_base.h"
#include "game/view/base/ui_operational_layer.h"
#include "game/view/scale_line/ui_scale_line_layer.h"
#include "log/log.h"

GAME_NAMESPACE_BEGIN

UIGoGameBoard::UIGoGameBoard(int grid, QWidget* parent)
: UIGameBoardBase(parent)
, grid_(grid)
, board_type_(GoTrait::GoBoardInfoByGrid(grid_).board_type_)
{
	chesses_.resize(grid_ * grid_);
}

void UIGoGameBoard::Init()
{
	this->InitModel();

	this->InitUI();
}

void UIGoGameBoard::InitModel()
{
	auto action_factory = std::make_shared<GoGameActionFactory>();

	auto master_model = GoGameBoard::Create(grid_);
	master_model->SetActionFacotry(action_factory);
	this->SetMasterModel(std::move(master_model));
}

void UIGoGameBoard::InitUI()
{
	setWindowFlags(Qt::FramelessWindowHint);
	setAutoFillBackground(true);

	QPalette palette;
	QPixmap pixmap(GoTrait::GoBoardInfoByGrid(grid_).image_path_.c_str());
	palette.setBrush(backgroundRole(), QBrush(pixmap));
	setPalette(palette);

	this->setFixedSize(pixmap.size());

	this->CreateScaleLine();

	this->CreateChesses();
	this->UpdateChessesFromModel(master_model_);
}

void UIGoGameBoard::CreateChesses()
{
	const QSize sz_chess(InnerChessWidth(), InnerChessWidth());

	for (int x = 0; x < grid_; ++x)
	{
		for (int y = 0; y < grid_; ++y)
		{
			auto& chess = chesses_[ChessIndexByXY(x, y)];
			chess = UIGoPieceBase::Create(grid_, this);

			chess->setFixedSize(sz_chess);

			chess->SetGridIndex(x, y);
			UiUtils::MoveWidgetMiddleTo(chess, this->PositionByGridIndex(chess->GetGridIndex()));

			chess->SetChessId(ChessInfo::EmptyId);
			chess->UpdateChess(UIChessPieceBase::UpdateReason::Forcely);

			// test mark
			//chess->Mark(MarkShape::Triangle);
		}
	}
}

void UIGoGameBoard::CreateScaleLine()
{
	scale_line_layer_ = new UIScaleLineLayer(this);

	scale_line_layer_->setFixedSize(this->size());

	const int font_size = 12;
	const QSize size(20, 20);

	std::string x_texts[19] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S" };
	std::string y_texts[19] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19" };

	scale_line_layer_->SetXScaleLines(GridOffsetFromOrigin() + QPoint(0, -6), InnerGridWidth(), { x_texts, x_texts + grid_ }, font_size, size);
	scale_line_layer_->SetYScaleLines(GridOffsetFromOrigin() + QPoint(-5, -2), InnerGridWidth(), { y_texts, y_texts + grid_ }, font_size, size);

	scale_line_layer_->lower();
}

void UIGoGameBoard::UpdateChessesFromModel(const GameBoardPtr& model)
{
	if (!model) { return; }

	ForeachChess([&](UIGoPieceBase* chess_piece){
		if (chess_piece)
		{
			chess_piece->SetChessId(*model->ChessAt(chess_piece->GetGridIndex()));
		}
	});
}

void UIGoGameBoard::UpdateChessesFromModelAndUndoVariation(const GameBoardPtr& model, const GameActionResult& result)
{
	if (!model) { return; }

	std::map<GridIndex, ChessId>  map_chess_var;
	result.ForeachChessVariation([&map_chess_var](const ChessVariation& chess_var){
		map_chess_var.emplace(chess_var.GetGridIndex(), chess_var.GetOldChessId());
	});

	ForeachChess([&](UIGoPieceBase* chess_piece){
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
// 10 -- UIGoGameBoard::OnMasterDone
void UIGoGameBoard::OnMasterDone(const GameActionResult& result)
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
			this->UpdateSelectedChessByActionResult(result);

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
// 4 -- GameBoardBase::DoCommand: genarate GameAction in terms of Command above
// 5 -- GameBoardBase::DoAction
// 6 -- GameActionWithResult::Do: do act on a concrete GameBoard to generate GameActionResult
// 7 -- GameBoardBase::NotifyGameActionDone
// 8 -- LocalModelObserver::OnGameActionDone
// 9 -- UIGoGameBoard::OnLocalDone
void  UIGoGameBoard::OnLocalDone(const GameActionResult& result)
{
	assert(result.IsValid());

	operational_layer_->Clear();

	this->SendCommand(result.GetCommandPtr());

	if (result.BoardIsChangedByAction())
	{
		this->UpdateSelectedChessByActionResult(result);

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

void UIGoGameBoard::SyncLocalWithMaster()
{
	if (!master_model_ || !local_model_) { return; }

	LocalGameBoard()->SyncWith(*MasterGameBoard());

	UIGameBoardBase::SyncLocalWithMaster();
}

bool UIGoGameBoard::NeedToPlayVariationImmediately(const GameActionResult& result)
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

void UIGoGameBoard::PlayMasterVariationOnAnimationLayer(const GameActionResult& result)
{
	this->UpdateChessesFromModelAndUndoVariation(master_model_, result);

	std::function<void()> callback = chess_move_callback_flag_.ToWeakCallback([=]{
		this->UpdateChessesFromModel(master_model_);

		if (MasterGameBoard()->IsRuleBased())
		{
			this->MarkGridsCaptured(result);
		}

		result.ForeachChessVariation([=](const ChessVariation& chess_var){
			if (chess_var.IsCapturedPassively())
			{
				animation_layer_->PlayChessVariation(chess_var);
			}
		});
	});

	bool moved_actively = false;

	bool move_actively_once = true;
	result.ForeachChessVariation([&](const ChessVariation& chess_var){
		if (chess_var.IsMovedActively())
		{
			animation_layer_->PlayChessVariation(chess_var, callback, move_actively_once && !MasterGameBoard()->IsRuleBased());

			if (move_actively_once)
			{
				move_actively_once = false;
				callback = nullptr;
			}

			moved_actively = true;
		}
	});

	if (!moved_actively)
	{
		callback();
	}
}

void UIGoGameBoard::PlayLocalVariationOnAnimationLayer(const GameActionResult& result)
{
	this->UpdateChessesFromModelAndUndoVariation(local_model_, result);

	std::function<void()> callback = chess_move_callback_flag_.ToWeakCallback([=]{
		this->UpdateChessesFromModel(local_model_);

		if (LocalGameBoard()->IsRuleBased())
		{
			this->MarkGridsCaptured(result);
		}

		result.ForeachChessVariation([=](const ChessVariation& chess_var){
			if (chess_var.IsCapturedPassively())
			{
				animation_layer_->PlayChessVariation(chess_var);
			}
		});
	});

	auto passing_point = operational_layer_->CurrentGlobalPosOfChessFaceCenter();

	bool moved_actively = false;

	for (const auto& chess_var : result.ChessVariations())
	{
		if (chess_var.IsMovedActively())
		{
			animation_layer_->PlayChessVariationWithPassingPoint(chess_var, passing_point, callback, !LocalGameBoard()->IsRuleBased());

			moved_actively = true;

			break;
		}
	}

	if (!moved_actively)
	{
		callback();
	}
}

void UIGoGameBoard::MarkGridsCaptured(const GameActionResult& result)
{
	ChessId chess_id_taken = ChessInfo::EmptyId;
	std::vector<GridIndex> grids_captured;
	result.ForeachChessVariation([&](const ChessVariation& chess_var){
		if (chess_var.IsCapturedPassively() && ChessIdIsEmpty(chess_var.GetNewChessId()) && ChessIdIsNotEmpty(chess_var.GetOldChessId()))
		{
			chess_id_taken = chess_var.GetOldChessId();
			grids_captured.push_back(chess_var.GetGridIndex());
		}
	});

	if (!grids_captured.empty())
	{
		auto mark_width = GridWidth() * 0.66f;
		auto mark_color = chess_id_taken == GoTrait::IdOfBlackPiece() ? Qt::GlobalColor::black : Qt::GlobalColor::white;
		animation_layer_->MarkOnGridsInBoard(grids_captured, QSize(mark_width, mark_width), mark_color);
	}
}

void UIGoGameBoard::SetRuleBased(bool rule_based)
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

void UIGoGameBoard::OnOperationalLayerAssociated()
{
	auto local_model = GoGameBoard::Create(grid_);

	local_model->SetActionFacotry(master_model_->ActionFacotry());
	local_model->SetRuleBased(MasterGameBoard()->IsRuleBased());

	this->SetLocalModel(std::move(local_model));
}

void UIGoGameBoard::OnLayoutChanged()
{
	ForeachChess([this](UIGoPieceBase* chess){
		if (chess)
		{
			UiUtils::MoveWidgetMiddleTo(chess, this->PositionByGridIndex(chess->GetGridIndex()));
		}
	});

	if (animation_layer_)
	{
		animation_layer_->RefreshArrowLine();
	}
}

QPoint UIGoGameBoard::PositionByGridIndex(const GridIndex& grid_index) const
{
	const auto width = InnerGridWidth();

	if (GetLayout() == GameBoardLayout::Flipped)
	{
		return GridOffsetFromOrigin() + QPoint(GridWidth() * (grid_ - grid_index.X()), GridWidth() * (grid_ - grid_index.Y()));
	}

	return GridOffsetFromOrigin() + QPoint(std::round(width * grid_index.X()), std::round(width * grid_index.Y()));
}

void UIGoGameBoard::SetScaleLineVisible(bool visible)
{
	if (scale_line_layer_)
	{
		scale_line_layer_->setVisible(visible);
	}
}

void UIGoGameBoard::HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr)
{
	UIGameBoardBase::HandleMasterCommandSpecially(cmd_ptr);
}

GoGameBoard* UIGoGameBoard::MasterGameBoard() const
{
	return static_cast<GoGameBoard*>(master_model_.get());
}

GoGameBoard* UIGoGameBoard::LocalGameBoard() const
{
	return static_cast<GoGameBoard*>(local_model_.get());
}

// override UIAnimationDelegate -- begin
QPixmap	UIGoGameBoard::MovingPixmapOfChessId(ChessId chess_id)
{
	return QPixmap(QString::fromStdString(GoTrait::GoPieceInfoByIdAndGrid(chess_id, grid_).moving_image_));
}

QPoint UIGoGameBoard::GlobalPositionOfGridCenterInBoard(const GridIndex& grid_index)
{
	return mapToGlobal(PositionByGridIndex(grid_index));
}

QPoint UIGoGameBoard::GlobalPositionOfChessCenterInBasket(ChessId chess_id)
{
	assert(chess_basket_);

	return chess_basket_->GlobalPositionOfChessCenter(chess_id);
}

void UIGoGameBoard::HideChessInBoardGrid(const GridIndex& grid_index)
{
	this->HideChessAt(grid_index);
}

int	 UIGoGameBoard::MilliSecondsOfMovingAnimation()
{
	return 200;
}
// override UIAnimationDelegate -- end

void UIGoGameBoard::SetAllChessToNormalState()
{
	ForeachChess([&](UIGoPieceBase* chess_piece){
		if (chess_piece)
		{
			chess_piece->SetToNormalState();
		}
	});
}

void UIGoGameBoard::SetChessToSelectedState(const GridIndex& grid)
{
	auto chess_piece = this->InnerChessPieceAt(grid.X(), grid.Y());
	if (chess_piece)
	{
		chess_piece->SetToSelectedState();
	}
}

void UIGoGameBoard::UpdateSelectedChessByActionResult(const GameActionResult& result)
{
	this->SetAllChessToNormalState();
	result.ForeachChessVariation([=](const ChessVariation& chess_var){
		if (chess_var.IsMovedInsideOfBoardActively())
		{
			this->SetChessToSelectedState(chess_var.GetGridIndexRelative());
		}
		else if (chess_var.IsMovedIntoBoardActively())
		{
			this->SetChessToSelectedState(chess_var.GetGridIndex());
		}
	});
}

bool UIGoGameBoard::HasChessSelected(GridIndex* grid, ChessId* chess_id)
{
	bool picked = false;

	for (auto chess : chesses_)
	{
		if (chess && chess->ChessIdIsNotEmpty() && chess->IsSelectedState())
		{
			if (grid) { *grid = chess->GetGridIndex(); }
			
			if (chess_id) { *chess_id = chess->GetChessId(); }
			
			picked = true;
			break;
		}
	}

	return picked;
}

GAME_NAMESPACE_END