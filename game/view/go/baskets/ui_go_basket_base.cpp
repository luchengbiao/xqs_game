#include "ui_go_basket_base.h"
#include "game/model/go/go_enum.h"
#include "ui_go_color_basket.h"


GAME_NAMESPACE_BEGIN

UIGoBasketBase::UIGoBasketBase(QWidget* parent) 
: UIChessBasketBase(parent) 
{}

void UIGoBasketBase::Init()
{
	UIChessBasketBase::Init();

	white_basket_ = new UIGoColorBasket(GoColor::White, this);
	black_basket_ = new UIGoColorBasket(GoColor::Black, this);
}

void UIGoBasketBase::FixChessId(const int grid, const ChessId chess_id)
{
	chess_id_fixed_ = chess_id;

	white_basket_->HideFixedChess();
	black_basket_->HideFixedChess();

	if (GoTrait::IdOfWhitePiece() == chess_id)
	{
		white_basket_->ShowFixedChess(grid);
	}
	else
	{
		black_basket_->ShowFixedChess(grid);
	}
}

void UIGoBasketBase::CancelFixedChessId()
{
	chess_id_fixed_ = ChessInfo::EmptyId;

	white_basket_->HideFixedChess();
	black_basket_->HideFixedChess();
}

GAME_NAMESPACE_END