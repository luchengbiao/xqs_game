#include "ui_chess_basket_base.h"

GAME_NAMESPACE_BEGIN

UIChessBasketBase::UIChessBasketBase(QWidget* parent)
: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint);
	setAutoFillBackground(true);

	QPalette palette;
	palette.setBrush(backgroundRole(), Qt::BrushStyle::NoBrush);
	setPalette(palette);
}

GAME_NAMESPACE_END