#include "ui_chess_path_marker_layer.h"
#include <QBrush>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>


GAME_NAMESPACE_BEGIN

UIChessPathMarkerLayer::UIChessPathMarkerLayer(QWidget* parent)
: QWidget(parent)
{}

void UIChessPathMarkerLayer::SetMarkerPoints(const Point2ColorMap& map_global_point_color)
{
	map_point_color_.clear();
	
	if (!map_global_point_color.empty())
	{
		for (const auto& pair : map_global_point_color)
		{
			map_point_color_.emplace(this->mapFromGlobal(pair.first), pair.second);
		}
	}
	
	this->update();
}

void  UIChessPathMarkerLayer::SetMarkerDotRadius(int marker_dot_radius)
{
	if (marker_dot_radius_ != marker_dot_radius)
	{
		marker_dot_radius_ = marker_dot_radius;

		this->update();
	}
}

void UIChessPathMarkerLayer::paintEvent(QPaintEvent* paint_event)
{
	QWidget::paintEvent(paint_event);

	if (map_point_color_.empty()) { return; }

	QPainter painter(this);

	QColor color = map_point_color_.begin()->second;
	QPen pen(color);
	pen.setWidth(2);
	painter.setPen(pen);

	int radius_half = marker_dot_radius_ * 0.5f;

	for (const auto& pair : map_point_color_)
	{
		if (pair.second != color)
		{
			color = pair.second;
			pen.setColor(color);
			painter.setPen(pen);
		}

		QPoint pos_local = this->mapFromGlobal(pair.first);
		
		painter.drawEllipse(pos_local, radius_half, radius_half);
		painter.drawEllipse(pos_local, marker_dot_radius_, marker_dot_radius_);
	}
}

GAME_NAMESPACE_END