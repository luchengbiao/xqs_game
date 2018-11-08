#include "ui_go_basket_o2o.h"
#include "common/uiutils/ui_utils.h"
#include "game/model/go/go_trait.h"
#include "game/view/widget_with_pixmap/widget_with_pixmap.h"
#include "ui_go_color_basket.h"
#include "ui_go_piece_in_basket.h"

GAME_NAMESPACE_BEGIN

UIGoBasketO2O::UIGoBasketO2O(QWidget* parent) 
: UIGoBasketBase(parent)
{}

void UIGoBasketO2O::Init()
{
	UIGoBasketBase::Init();

	const int width = 200;
	const int height = 518;
	const int half_width = width * 0.5;

	this->setFixedSize(width, height);

	auto wgt_title = new WidgetWithPixmap(this);
	QPixmap pxp_title(":/common/images/common/basket_title.png");
	if (!pxp_title.isNull())
	{
		wgt_title->setPixmap(pxp_title);
		wgt_title->setFixedSize(pxp_title.size());
	}

	UiUtils::MoveWidgetMiddleTopTo(wgt_title, QPoint(half_width, 30));
	UiUtils::MoveWidgetMiddleTopTo(white_basket_, UiUtils::WidgetPosOfMiddleBottom(wgt_title) + QPoint(13, 20 - UIGoColorBasket::YOffsetOfBasket()));
	UiUtils::MoveWidgetMiddleTopTo(black_basket_, UiUtils::WidgetPosOfMiddleBottom(white_basket_) + QPoint(0, 10 - UIGoColorBasket::YOffsetOfBasket()));

	wgt_title->lower();
	white_basket_->raise();
	black_basket_->raise();
}

void UIGoBasketO2O::SetBasketEnabled(bool enabled)
{
	if (enabled)
	{
		white_basket_->HideDisabledFlag();
		black_basket_->HideDisabledFlag();
	}
	else
	{
		white_basket_->ShowDisabledFlag();
		black_basket_->ShowDisabledFlag();
	}
}

std::map<ChessId, QRect>  UIGoBasketO2O::ChessesGlobalRect() const
{
	std::map<ChessId, QRect> id2Rects;

	id2Rects.emplace(GoTrait::IdOfWhitePiece(), white_basket_->ChessGlobalArea());
	id2Rects.emplace(GoTrait::IdOfBlackPiece(), black_basket_->ChessGlobalArea());

	return id2Rects;
}

QPoint  UIGoBasketO2O::GlobalPositionOfChessCenter(ChessId chess_id) const
{
	QPoint pos = this->mapToGlobal(QPoint(0, 0));

	if (chess_id == GoTrait::IdOfWhitePiece())
	{
		pos = white_basket_->GlobalPosOfChessAreaCenter();
	}
	else if (chess_id == GoTrait::IdOfBlackPiece())
	{
		pos = black_basket_->GlobalPosOfChessAreaCenter();
	}

	return pos;
}

GAME_NAMESPACE_END