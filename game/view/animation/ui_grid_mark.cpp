#include "ui_grid_mark.h"
#include <QPainter>

GAME_NAMESPACE_BEGIN

UIGridMark::UIGridMark(QWidget* parent)
: QWidget(parent)
{}

void UIGridMark::SetColor(const QColor& color)
{
	if (color_ != color)
	{
		color_ = color;

		update();
	}
}

void UIGridMark::paintEvent(QPaintEvent* paint_event)
{
	QWidget::paintEvent(paint_event);

	QPainter painter(this);

	QPen pen(color_);
	pen.setWidth(1);
	pen.setStyle(Qt::PenStyle::DotLine);
	painter.setPen(pen);

	painter.drawEllipse(1, 1, width() - 2, height() - 2);
}

GAME_NAMESPACE_END