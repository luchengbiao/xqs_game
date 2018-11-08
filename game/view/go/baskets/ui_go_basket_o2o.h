#ifndef __GAME_VIEW_GO_BASKETS_UI_GO_BASKET_O2O_H__
#define __GAME_VIEW_GO_BASKETS_UI_GO_BASKET_O2O_H__
#include "game/view/base/ui_chess_face_fwd.h"
#include "ui_go_basket_base.h"

GAME_NAMESPACE_BEGIN

class UIGoBasketO2O : public UIGoBasketBase
{
	Q_OBJECT

public:
	explicit UIGoBasketO2O(QWidget* parent);

	virtual void		SetBasketEnabled(bool enabled) override;

	virtual	QPoint		GlobalPositionOfChessCenter(ChessId chess_id) const override;
	virtual std::map<ChessId, QRect> ChessesGlobalRect() const override;

private:
	virtual void		Init() override;

private:

};

GAME_NAMESPACE_END

#endif