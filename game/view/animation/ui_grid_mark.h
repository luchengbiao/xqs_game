#ifndef __GAME_VIEW_ANIMATION_UI_GRID_MARK_H__
#define __GAME_VIEW_ANIMATION_UI_GRID_MARK_H__
#include <QColor>
#include <QWidget>
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

class UIGridMark : public QWidget
{
	Q_OBJECT

public:
	explicit UIGridMark(QWidget* parent = nullptr);

	void			SetColor(const QColor& color);

protected:
	virtual void	paintEvent(QPaintEvent* paint_event) override;

private:
	QColor			color_;
};

GAME_NAMESPACE_END

#endif