#include "ui_go_game_board_wrapper.h"
#include "game/model/go/go_trait.h"
#include "game/model/command_handler/command_handler.h"
#include "game/model/command_handler/game_board_wrapper.h"
#include "game/view/animation/ui_animation_layer.h"
#include "game/view/go/baskets/ui_go_basket_o2o.h"
#include "game/view/go/operational/ui_go_operational_layer.h"
#include "game/view/go/ui_go_piece.h"
#include "protocol/command/base/command.h"
#include "ui_go_game_board.h"

using namespace proto;

GAME_NAMESPACE_BEGIN

class UIGoGameBoardWrapper::OperationalDelegate : public GoOperationalDelegate
{
public:
	virtual bool CanOperateOnBoardNow() override
	{
		return board_wrapper_->CurrentGameBoard()->CanOperateOnBoardNow();
	}

	virtual void OnWillOperateOnBoard() override
	{
		board_wrapper_->CurrentGameBoard()->TryToRefreshLocal();
	}

	virtual GridRectInBoardMap GridsGlobalRectInBoard() override
	{
		auto game_board = board_wrapper_->CurrentGameBoard();
		auto grid_width = game_board->GridWidth();
		QSize grid_size(grid_width, grid_width);
		QPoint offset(grid_width * 0.5f, grid_width * 0.5f);

		int rows = game_board->RowsOfBoard();
		int cols = game_board->ColumnsOfBoard();

		std::map<GridIndex, QRect> map_grid_global_rect;
		for (int x = 0; x < cols; ++x)
		{
			for (int y = 0; y < rows; ++y)
			{
				GridIndex grid_index(x, y);
				auto pos = game_board->PositionByGridIndex(grid_index) - offset;
				map_grid_global_rect.emplace(grid_index, QRect(game_board->mapToGlobal(pos), grid_size));
			}
		}

		return map_grid_global_rect;
	}

	virtual ChessRectInBasketMap ChessesGlobalRectInBasket() override
	{
		return board_wrapper_->ChessBasket()->ChessesGlobalRect();
	}

	virtual QPixmap	MovingPixmapOfChessId(ChessId chess_id) override
	{
		return QPixmap(QString::fromStdString(GoTrait::GoPieceInfoByIdAndGrid(chess_id, board_wrapper_->CurrentGameBoard()->RowsOfBoard()).moving_image_));
	}

	virtual QPixmap	SelectedPixmapOfChessId(ChessId chess_id) override
	{
		return QPixmap(QString::fromStdString(GoTrait::GoPieceInfoByIdAndGrid(chess_id, board_wrapper_->CurrentGameBoard()->RowsOfBoard()).selected_image_));
	}

	virtual bool GridContainsChessInBoard(const GridIndex& grid_index, ChessId* chess_id) override
	{
		auto go_piece = board_wrapper_->CurrentGameBoard()->ChessPieceAt(grid_index);

		if (go_piece && go_piece->ChessIdIsNotEmpty())
		{
			if (chess_id) { *chess_id = go_piece->GetChessId(); }

			return true;
		}

		return false;
	}

	virtual void HideChessInBoardGrid(const GridIndex& grid_index) override
	{
		board_wrapper_->CurrentGameBoard()->HideChessAt(grid_index);
	}

	virtual void ShowChessInBoardGrid(const GridIndex& grid_index) override
	{
		board_wrapper_->CurrentGameBoard()->ShowChessAt(grid_index);
	}

	virtual void OnMousePressed(const GridIndex& grid_selected, ChessId chess_id_selected) override
	{
		auto go_board = static_cast<UIGoGameBoard*>(board_wrapper_->CurrentGameBoard());

		if (grid_selected.IsValid()) // in board
		{
			if (ChessIdIsNotEmpty(chess_id_selected))
			{
				board_wrapper_->go_basket_->CancelFixedChessId();

				go_board->SetAllChessToNormalState();
				go_board->SetChessToSelectedState(grid_selected);
			}
		}
		else // from basket
		{
			//board_wrapper_->go_basket_->FixChessId(go_board->RowsOfBoard(), chess_id_selected);
		}
	}

	virtual void OnMouseWillMove(const GridIndex&, ChessId) override
	{
		auto animation_layer = board_wrapper_->CurrentAnimationLayer();
		animation_layer->HideArrowLine();
		//animation_layer->HideGridMarks();
	}

	virtual void OnMouseReleased(const GridIndex& grid_from, ChessId chess_id_from, const GridIndex& grid_to, ChessId chess_id_to) override
	{
		auto go_board = static_cast<UIGoGameBoard*>(board_wrapper_->CurrentGameBoard());

		if (!grid_from.IsValid() && !grid_to.IsValid())
		{
			if (chess_id_from == chess_id_to && ChessIdIsNotEmpty(chess_id_from)) // from & to are in a same basket
			{
				if (board_wrapper_->go_basket_->ChessIdFixed() == chess_id_from)
				{
					board_wrapper_->go_basket_->CancelFixedChessId();
				}
				else
				{
					board_wrapper_->go_basket_->FixChessId(go_board->RowsOfBoard(), chess_id_from);
				}
			}
		}
		else if (!grid_from.IsValid() && grid_to.IsValid())
		{
			if (ChessIdIsNotEmpty(chess_id_from)) // from a basket to a grid in board
			{
				board_wrapper_->go_basket_->FixChessId(go_board->RowsOfBoard(), chess_id_from);
			}
		}
	}

	virtual bool TryToMoveChessLocally(ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to) override
	{
		return board_wrapper_->CurrentGameBoard()->TryToMoveChessLocally(chess_id, grid_from, grid_to);
	}

	virtual bool IsChessFixedInBasket(ChessId* chess_id_fixed) override
	{
		auto chess_id = board_wrapper_->go_basket_->ChessIdFixed();

		if (chess_id_fixed)
		{
			*chess_id_fixed = chess_id;
		}

		return ChessIdIsNotEmpty(chess_id);
	}

	virtual bool IsChessSelectedInBoard(GridIndex* grid_selected, ChessId* chess_id_selected) override
	{
		auto go_board = static_cast<UIGoGameBoard*>(board_wrapper_->CurrentGameBoard());

		return go_board->HasChessSelected(grid_selected, chess_id_selected);
	}

	OperationalDelegate(UIGoGameBoardWrapper* board_wrapper)
		: board_wrapper_(board_wrapper)
	{}

	UIGoGameBoardWrapper* board_wrapper_{ nullptr };
};

UIGoGameBoardWrapper* UIGoGameBoardWrapper::Create(ClientType client_type, UsageType usage_type, QWidget* parent)
{
	UIGoGameBoardWrapper* board_wrapper = new (std::nothrow) UIGoGameBoardWrapper(client_type, usage_type, parent);

	if (board_wrapper)
	{
		board_wrapper->Init();
	}

	return board_wrapper;
}

UIGoGameBoardWrapper::UIGoGameBoardWrapper(ClientType client_type, UsageType usage_type, QWidget* parent)
: UIGameBoardWrapper(client_type, usage_type, parent)
{}

UIGoGameBoardWrapper::~UIGoGameBoardWrapper()
{
	delete operational_delegate_;
}

void UIGoGameBoardWrapper::Init()
{
	UIGameBoardWrapper::Init();

	this->SwithToGameBoard(GameBoardType::Go19);

	this->ZOrderUi();
}

GameBoardVector UIGoGameBoardWrapper::CreateGameBoards()
{
	GameBoardVector vec;

	UIGameBoardBase* game_board = game::CreateGameBoard<UIGoGameBoard>(9, this);
	vec.emplace_back(game_board);

	game_board = game::CreateGameBoard<UIGoGameBoard>(13, this);
	vec.emplace_back(game_board);

	game_board = game::CreateGameBoard<UIGoGameBoard>(19, this);
	vec.emplace_back(game_board);

	return vec;
}

UIChessBasketBase* UIGoGameBoardWrapper::CreateChessBasket()
{
	if (UsageType::O2O == GetUsageType() || UsageType::Playback_O2O == GetUsageType())
	{
		go_basket_ = game::CreateChessBasket<UIGoBasketO2O>(this);
	}

	return go_basket_;
}

UIOperationalLayer* UIGoGameBoardWrapper::InnerCreateOperationalLayer(QWidget* presentation_layer)
{
	auto operational_layer = new UIGoOperationalLayer(this, presentation_layer);

	operational_delegate_ = new OperationalDelegate(this);

	operational_layer->SetOperationalDelegate(operational_delegate_);

	return operational_layer;
}

void UIGoGameBoardWrapper::HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr)
{
	UIGameBoardWrapper::HandleMasterCommandSpecially(cmd_ptr);

	const auto cmd_tpye = cmd_ptr->Type();
	if (cmd_tpye == CommandType::TeacherChangeGameBoard)
	{
		if (ClientTypeIsStudent())
		{
			if (operational_layer_)
			{
				operational_layer_->Interrupt();
			}

			go_basket_->CancelFixedChessId();
		}
	}
}

GAME_NAMESPACE_END