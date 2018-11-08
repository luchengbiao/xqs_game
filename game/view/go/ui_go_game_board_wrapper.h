#ifndef __GAME_VIEW_GO_UI_GO_GAME_BOARD_WRAPPER_H__
#define __GAME_VIEW_GO_UI_GO_GAME_BOARD_WRAPPER_H__
#include <functional>
#include <memory>
#include "game/view/base/ui_game_board_wrapper.h"

GAME_NAMESPACE_BEGIN

class GridIndex;
class UIGoBasketBase;

class UIGoGameBoardWrapper : public UIGameBoardWrapper
{
	Q_OBJECT

	class OperationalDelegate;

public:
	static UIGoGameBoardWrapper* Create(ClientType client_type, UsageType usage_type, QWidget* parent);

	~UIGoGameBoardWrapper();

	// override GameBoardWrapper
	virtual void				HandleMasterCommandSpecially(const CommandSharedPtr&) override;

protected:
	explicit UIGoGameBoardWrapper(ClientType client_type, UsageType usage_type, QWidget* parent = nullptr);

	virtual void				Init() override;

	virtual GameBoardVector		CreateGameBoards() override;
	virtual UIChessBasketBase*	CreateChessBasket() override;
	virtual UIOperationalLayer*	InnerCreateOperationalLayer(QWidget* presentation_layer) override;

private:
	UIGoBasketBase*				go_basket_{ nullptr };

	OperationalDelegate*		operational_delegate_{ nullptr };

	bool						game_board_enabled_{ true };
};

GAME_NAMESPACE_END

#endif