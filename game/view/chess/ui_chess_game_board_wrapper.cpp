#include "ui_chess_game_board_wrapper.h"
#include "game/model/chess/chess_game_board.h"
#include "game/model/chess/chess_trait.h"
#include "game/model/command_handler/command_handler.h"
#include "game/model/command_handler/game_board_wrapper.h"
#include "game/view/animation/ui_animation_layer.h"
#include "game/view/chess/baskets/ui_chess_basket_o2o.h"
#include "game/view/chess/operational/ui_chess_operational_layer.h"
#include "game/view/chess/ui_chess_piece.h"
#include "protocol/command/base/command.h"
#include "ui_chess_game_board.h"

using namespace proto;

GAME_NAMESPACE_BEGIN

class UIChessGameBoardWrapper::OperationalDelegate : public ChessOperationalDelegate
{
public:
	virtual bool BoardIsWithRule() override
	{
		return board_wrapper_->game_board_->LocalGameBoard()->IsRuleBased();
	}

	virtual bool CanOperateOnBoardNow() override
	{
		return board_wrapper_->game_board_->CanOperateOnBoardNow();
	}

	virtual void OnWillOperateOnBoard() override
	{
		board_wrapper_->game_board_->TryToRefreshLocal();
	}

	virtual GridRectInBoardMap GridsGlobalRectInBoard() override
	{
		auto game_board = board_wrapper_->game_board_;
		auto grid_width = game_board->GridWidth();
		QSize grid_size(grid_width, grid_width);

		int rows = game_board->RowsOfBoard();
		int cols = game_board->ColumnsOfBoard();

		std::map<GridIndex, QRect> map_grid_global_rect;
		for (int x = 0; x < cols; ++x)
		{
			for (int y = 0; y < rows; ++y)
			{
				GridIndex grid_index(x, y);
				map_grid_global_rect.emplace(grid_index, QRect(game_board->mapToGlobal(game_board->PositionByGridIndex(grid_index)), grid_size));
			}
		}

		return map_grid_global_rect;
	}

	virtual ChessRectInBasketMap ChessesGlobalRectInBasket() override
	{
		return board_wrapper_->ChessBasket()->ChessesGlobalRect();
	}

	virtual ChessReachableGridMap GridsReachableFrom(const GridIndex& grid_from) override
	{
		auto local_board_model = board_wrapper_->game_board_->LocalGameBoard();

		return local_board_model->GridsReachableFrom(grid_from);
	}

	virtual QPixmap	MovingPixmapOfChessId(ChessId chess_id) override
	{
		return QPixmap(QString::fromStdString(ChessTrait::MovingImagePathById(chess_id)));
	}

	virtual QPixmap	SelectedPixmapOfChessId(ChessId chess_id) override
	{
		return QPixmap(QString::fromStdString(ChessTrait::SelectedImagePathById(chess_id)));
	}

	virtual bool GridContainsChessInBoard(const GridIndex& grid_index, ChessId* chess_id) override
	{
		auto chess_piece = board_wrapper_->game_board_->ChessPieceAt(grid_index);
		
		if (chess_piece)
		{
			auto target_chess_id = chess_piece->GetChessId();
			if (ChessIdIsNotEmpty(target_chess_id))
			{
				if (chess_id)
				{
					*chess_id = target_chess_id;
				}

				return true;
			}
		}

		return false;
	}

	virtual void HideChessInBoardGrid(const GridIndex& grid_index) override
	{
		board_wrapper_->game_board_->HideChessAt(grid_index);
	}

	virtual void ShowChessInBoardGrid(const GridIndex& grid_index) override
	{
		board_wrapper_->game_board_->ShowChessAt(grid_index);
	}

	virtual void OnMouseWillMove(const GridIndex&, ChessId) override
	{
		board_wrapper_->animation_layer_->HideArrowLine();
	}

	virtual void HighlightGridInBoard(const GridIndex& grid_index) override
	{
		auto chess_piece = board_wrapper_->game_board_->ChessPieceAt(grid_index);

		if (chess_piece)
		{
			chess_piece->Highlight();
		}
	}

	virtual void UnhighlightGridInBoard(const GridIndex& grid_index) override
	{
		auto chess_piece = board_wrapper_->game_board_->ChessPieceAt(grid_index);

		if (chess_piece)
		{
			chess_piece->Unhighlight();
		}
	}

	virtual bool TryToMoveChessLocally(ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to) override
	{
		return board_wrapper_->game_board_->TryToMoveChessLocally(chess_id, grid_from, grid_to);
	}

	OperationalDelegate(UIChessGameBoardWrapper* board_wrapper)
		: board_wrapper_(board_wrapper)
	{}

	UIChessGameBoardWrapper* board_wrapper_{ nullptr };
};

UIChessGameBoardWrapper* UIChessGameBoardWrapper::Create(ClientType client_type, UsageType usage_type, QWidget* parent)
{
	UIChessGameBoardWrapper* board_wrapper = new (std::nothrow) UIChessGameBoardWrapper(client_type, usage_type, parent);

	if (board_wrapper)
	{
		board_wrapper->Init();
	}

	return board_wrapper;
}

UIChessGameBoardWrapper::UIChessGameBoardWrapper(ClientType client_type, UsageType usage_type, QWidget* parent)
: UIGameBoardWrapper(client_type, usage_type, parent)
{}

UIChessGameBoardWrapper::~UIChessGameBoardWrapper()
{
	delete operational_delegate_;
}

void UIChessGameBoardWrapper::Init()
{
	UIGameBoardWrapper::Init();

	game_board_ = static_cast<UIChessGameBoard*>(this->CurrentGameBoard());
	animation_layer_ = this->CurrentAnimationLayer();

	this->ZOrderUi();
}

GameBoardVector UIChessGameBoardWrapper::CreateGameBoards()
{
	GameBoardVector vec;

	auto game_board = game::CreateGameBoard<UIChessGameBoard>(this);
	vec.emplace_back(game_board);

	return vec;
}

UIChessBasketBase* UIChessGameBoardWrapper::CreateChessBasket()
{
	UIChessBasketBase* chess_basket = nullptr;

	if (UsageType::O2O == GetUsageType() || UsageType::Playback_O2O == GetUsageType())
	{
		chess_basket = game::CreateChessBasket<UIChessBasketO2O>(this);
	}

	return chess_basket;
}

UIOperationalLayer* UIChessGameBoardWrapper::InnerCreateOperationalLayer(QWidget* presentation_layer)
{
	auto operational_layer = new UIChessOperationalLayer(this, presentation_layer);

	operational_delegate_ = new OperationalDelegate(this);

	operational_layer->SetOperationalDelegate(operational_delegate_);

	return operational_layer;
}

void UIChessGameBoardWrapper::HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr)
{
	UIGameBoardWrapper::HandleMasterCommandSpecially(cmd_ptr);
}

GAME_NAMESPACE_END