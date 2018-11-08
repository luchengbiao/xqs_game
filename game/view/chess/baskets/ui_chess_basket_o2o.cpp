#include "ui_chess_basket_o2o.h"
#include "common/uiutils/ui_utils.h"
#include "game/view/widget_with_pixmap/widget_with_pixmap.h"
#include "ui_chess_piece_in_basket.h"

GAME_NAMESPACE_BEGIN

UIChessBasketO2O::UIChessBasketO2O(QWidget* parent) 
: UIChessBasketBase(parent)
{}

void UIChessBasketO2O::Init()
{
	UIChessBasketBase::Init();

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

	auto wgt_basket = new WidgetWithPixmap(this);
	QPixmap pxp_basket(":/chess/images/chess/chess_basket.png");
	if (!pxp_basket.isNull())
	{
		wgt_basket->setPixmap(pxp_basket);
		wgt_basket->setFixedSize(pxp_basket.size());
	}
	UiUtils::MoveWidgetMiddleTopTo(wgt_basket, UiUtils::WidgetPosOfMiddleBottom(wgt_title) + QPoint(0, 20));

	ChessId chess_ids[4][3] = 
	{
		{ ChessTrait::IdOfBlackPawn(), ChessTrait::IdOfBlackRook(), ChessTrait::IdOfBlackKnight() },
		{ ChessTrait::IdOfBlackBishop(), ChessTrait::IdOfBlackQueen(), ChessTrait::IdOfBlackKing() },
		{ ChessTrait::IdOfWhitePawn(), ChessTrait::IdOfWhiteRook(), ChessTrait::IdOfWhiteKnight() },
		{ ChessTrait::IdOfWhiteBishop(), ChessTrait::IdOfWhiteQueen(), ChessTrait::IdOfWhiteKing() },
	};

	const QSize sz_chess(GridWidth(), GridWidth());
	const QPoint pos_lt = UiUtils::WidgetPosOfLeftTop(wgt_basket);
	const int h_gap = GridWidth();
	const int v_gap = GridWidth() + 1;

	for (int r = 0; r < 4; ++r) // row
	{
		for (int c = 0; c < 3; ++c) // column
		{
			auto chess_piece = new UIChessPieceInBasket(this);
			chess_piece->setFixedSize(sz_chess);
			chess_piece->move(pos_lt.x() + c * h_gap, pos_lt.y() + r * v_gap);

			chess_piece->SetChessId(chess_ids[r][c]);

			chess_pieces_.emplace_back(chess_piece);
		}
	}
}

void UIChessBasketO2O::SetBasketEnabled(bool enabled)
{
	for (auto chess_piece : chess_pieces_)
	{
		enabled ? chess_piece->HideDisabledFlag() : chess_piece->ShowDisabledFlag();
	}
}

std::map<ChessId, QRect>  UIChessBasketO2O::ChessesGlobalRect() const
{
	std::map<ChessId, QRect> id2Rects;

	for (auto chess_piece : chess_pieces_)
	{
		id2Rects.emplace(chess_piece->GetChessId(), QRect(chess_piece->mapToGlobal(QPoint(0, 0)), chess_piece->size()));
	}

	return id2Rects;
}

QPoint  UIChessBasketO2O::GlobalPositionOfChessCenter(ChessId chess_id) const
{
	QPoint pos = this->mapToGlobal(QPoint(0, 0));

	for (auto chess_piece : chess_pieces_)
	{
		if (chess_piece->GetChessId() == chess_id)
		{
			pos = chess_piece->mapToGlobal(QPoint(chess_piece->width() * 0.5f, chess_piece->height() * 0.5f));
		}
	}

	return pos;
}

GAME_NAMESPACE_END