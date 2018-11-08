#ifndef __GAME_VIEW_BASE_UI_CHESS_BASKET_BASE_H__
#define __GAME_VIEW_BASE_UI_CHESS_BASKET_BASE_H__
#include <map>
#include <QWidget>
#include "game/model/base/chess/chess_info.h"
#include "protocol/command/base/command_fwd.h"

GAME_NAMESPACE_BEGIN

class UIChessBasketBase : public QWidget
{
	Q_OBJECT

	template<typename UIChessBasketT, typename... Args>
	friend UIChessBasketT* CreateChessBasket(Args&&... args);

public:
	explicit UIChessBasketBase(QWidget* parent = nullptr);

	virtual void	SetBasketEnabled(bool enabled) = 0;

	virtual	QPoint	GlobalPositionOfChessCenter(ChessId chess_id) const = 0;

	virtual std::map<ChessId, QRect> ChessesGlobalRect() const = 0;
	
	virtual void	HandleMasterCommandSpecially(const CommandSharedPtr&) {}

protected:
	virtual void	Init() {}
};

template<typename UIChessBasketT, typename... Args>
UIChessBasketT* CreateChessBasket(Args&&... args)
{
	UIChessBasketBase* chess_basket = new (std::nothrow) UIChessBasketT(std::forward<Args>(args)...);

	if (chess_basket) { chess_basket->Init(); }

	return static_cast<UIChessBasketT*>(chess_basket);
}

GAME_NAMESPACE_END

#endif