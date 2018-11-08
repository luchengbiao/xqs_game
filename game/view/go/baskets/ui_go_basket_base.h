#ifndef __GAME_VIEW_GO_BASKETS_UI_GO_BASKET_BASE_H__
#define __GAME_VIEW_GO_BASKETS_UI_GO_BASKET_BASE_H__
#include "game/model/base/chess/chess_info.h"
#include "game/view/base/ui_chess_basket_base.h"

GAME_NAMESPACE_BEGIN

class UIGoColorBasket;

class UIGoBasketBase : public UIChessBasketBase
{
	Q_OBJECT

public:
	explicit UIGoBasketBase(QWidget* parent = nullptr);

	void				FixChessId(const int grid, const ChessId chess_id);
	void				CancelFixedChessId();
	ChessId				ChessIdFixed() const { return chess_id_fixed_; }

protected:
	virtual void		Init() override;
	
protected:
	UIGoColorBasket*	white_basket_{ nullptr };
	UIGoColorBasket*	black_basket_{ nullptr };

	ChessId				chess_id_fixed_{ ChessInfo::EmptyId };
};

GAME_NAMESPACE_END

#endif