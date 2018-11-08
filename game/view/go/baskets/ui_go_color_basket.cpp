#include "ui_go_color_basket.h"
#include "common/uiutils/ui_utils.h"
#include "game/view/widget_with_pixmap/widget_with_pixmap.h"
#include "ui_go_piece_in_basket.h"

GAME_NAMESPACE_BEGIN

UIGoColorBasket::UIGoColorBasket(GoTrait::Color color, QWidget* parent)
: QWidget(parent)
, color_(color)
{
	this->Init();
}

void UIGoColorBasket::Init()
{
	basket_ = new WidgetWithPixmap(this);
	QPixmap pxp_basket(color_ == GoColor::White ? ":/go/images/go/basket_white.png" : ":/go/images/go/basket_black.png");
	if (!pxp_basket.isNull())
	{
		basket_->setPixmap(pxp_basket);
		basket_->setFixedSize(pxp_basket.size());
	}

	QSize size = basket_->size() + QSize(0, YOffsetOfBasket());
	this->setFixedSize(size);

	basket_->move(0, YOffsetOfBasket());
}

QRect UIGoColorBasket::ChessGlobalArea() const
{
	return QRect(basket_->mapToGlobal(PosOfChessArea()), SizeOfChessArea());
}

QPoint UIGoColorBasket::GlobalPosOfChessAreaCenter() const
{
	return basket_->mapToGlobal(PosOfChessArea() + QPoint(SizeOfChessArea().width() * 0.5f, SizeOfChessArea().height() * 0.5f));
}

void UIGoColorBasket::ShowDisabledFlag()
{
	if (!disabled_flag_)
	{
		disabled_flag_ = new WidgetWithPixmap(this);

		QPixmap pxp_disabled(":/common/images/common/disabled_flag.png");
		if (!pxp_disabled.isNull())
		{
			disabled_flag_->setPixmap(pxp_disabled);
			disabled_flag_->setFixedSize(pxp_disabled.size());
		}

		UiUtils::MoveWidgetRightBottomTo(disabled_flag_, QPoint(185, 175 + basket_->y()));
	}

	disabled_flag_->show();
}

void UIGoColorBasket::HideDisabledFlag()
{
	if (disabled_flag_)
	{
		disabled_flag_->hide();
	}
}

void UIGoColorBasket::ShowFixedChess(const int grid)
{
	this->LazyCreateGoPieces();

	switch (grid)
	{
	case 9:
		go_piece_9_->show();
		break;

	case 13:
		go_piece_13_->show();
		break;

	case 19:
		go_piece_19_->show();
		break;

	default:
		break;
	}
}

void UIGoColorBasket::HideFixedChess()
{
	if (go_piece_9_)
	{
		go_piece_9_->hide();
	}

	if (go_piece_13_)
	{
		go_piece_13_->hide();
	}

	if (go_piece_19_)
	{
		go_piece_19_->hide();
	}
}

void UIGoColorBasket::LazyCreateGoPieces()
{
	if (go_piece_9_) { return; }

	go_piece_9_ = CreateGoPiece(9);
	go_piece_13_ = CreateGoPiece(13);
	go_piece_19_ = CreateGoPiece(19);
}

UIGoPieceBaseInBasket* UIGoColorBasket::CreateGoPiece(const int grid)
{
	auto go_piece = UIGoPieceBaseInBasket::Create(grid, this);

	int width = GoTrait::GoBoardInfoByGrid(grid).chess_width_;
	go_piece->setFixedSize(width, width);

	go_piece->SetChessId(color_ == GoColor::White ? GoTrait::IdOfWhitePiece() : GoTrait::IdOfBlackPiece());

	UiUtils::MoveWidgetMiddleTo(go_piece, QPoint(170, basket_->y() + 5));

	go_piece->hide();

	return go_piece;
}

GAME_NAMESPACE_END