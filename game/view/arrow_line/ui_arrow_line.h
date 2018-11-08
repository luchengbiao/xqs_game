#ifndef __GAME_VIEW_ARROW_LINE_UI_ARROW_LINE_H__
#define __GAME_VIEW_ARROW_LINE_UI_ARROW_LINE_H__
#include <QColor>
#include <QWidget>
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

class UIArrowLine : public QWidget
{
	Q_OBJECT

public:
	explicit UIArrowLine(QWidget* parent = nullptr);
	~UIArrowLine();

	void			SetStartAndEndPosition(const QPoint& pos_start_global, const QPoint& pos_end_global);

	void			SetStartPosition(const QPoint& pos_global);

	void			SetEndPosition(const QPoint& pos_global);

	void			SetColor(const QColor& color);

	void			SetLineWidth(int line_width);

	void			SetLineStyle(Qt::PenStyle line_style);

	void			SetArrowWingLength(int arrow_wing_length);

private:
	virtual void	paintEvent(QPaintEvent*) override;

	void			CalcArrowWingPositions();

private:
	QPoint*			pos_start_{ nullptr };
	QPoint*			pos_end_{ nullptr };
	QColor			color_{ Qt::GlobalColor::green };
	int				line_width_{ 2 };
	Qt::PenStyle	line_style_{ Qt::PenStyle::SolidLine };

	int				arrow_wing_length_{ 12 };
	QPoint			pos_arrow_wing0_;
	QPoint			pos_arrow_wing1_;
};

GAME_NAMESPACE_END

#endif