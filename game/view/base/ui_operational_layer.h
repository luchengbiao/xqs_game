#ifndef __GAME_VIEW_BASE_UI_OPERATIONAL_LAYER_H__
#define __GAME_VIEW_BASE_UI_OPERATIONAL_LAYER_H__
#include <QWidget>
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

class UIOperationalLayer : public QWidget
{
public:
	explicit UIOperationalLayer(QWidget* parent = nullptr) : QWidget(parent) {}

	virtual void		Interrupt() = 0;
	virtual void		Clear() = 0;
	virtual QPoint		CurrentGlobalPosOfChessFaceCenter() const = 0;

protected:
};

GAME_NAMESPACE_END

#endif