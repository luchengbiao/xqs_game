#ifndef __GAME_VIEW_CHESS_BASKETS_CHESS_PIECE_IN_BASKET_H__
#define __GAME_VIEW_CHESS_BASKETS_CHESS_PIECE_IN_BASKET_H__
#include "game/model/chess/chess_trait.h"
#include "game/view/base/chess_piece/ui_chess_piece_with_trait.h"

class WidgetWithPixmap;

GAME_NAMESPACE_BEGIN

class UIChessPieceInBasket : public UIChessPieceWithTrait<ChessTrait>
{
	Q_OBJECT

public:
	explicit UIChessPieceInBasket(QWidget* parent = nullptr) : UIChessPieceWithTrait(parent) {}

	void	ShowDisabledFlag();
	void	HideDisabledFlag();

private:
	WidgetWithPixmap* disabled_flag_{ nullptr };
};

GAME_NAMESPACE_END

#endif