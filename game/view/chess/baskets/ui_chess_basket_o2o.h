#ifndef __GAME_VIEW_CHESS_BASKETS_UI_CHESS_BASKET_O2O_H__
#define __GAME_VIEW_CHESS_BASKETS_UI_CHESS_BASKET_O2O_H__
#include <vector>
#include "game/view/base/ui_chess_basket_base.h"
#include "game/view/base/ui_chess_face_fwd.h"

GAME_NAMESPACE_BEGIN

class UIChessPieceInBasket;

class UIChessBasketO2O : public UIChessBasketBase
{
	Q_OBJECT

public:
	explicit UIChessBasketO2O(QWidget* parent);

	virtual void	SetBasketEnabled(bool enabled) override;

	virtual	QPoint	GlobalPositionOfChessCenter(ChessId chess_id) const override;
	virtual std::map<ChessId, QRect> ChessesGlobalRect() const override;

private:
	virtual void	Init() override;

	inline int		GridWidth() const { return 62; }

private:
	std::vector<UIChessPieceInBasket*> chess_pieces_;
};

GAME_NAMESPACE_END

#endif