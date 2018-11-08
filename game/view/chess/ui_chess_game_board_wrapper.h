#ifndef __GAME_VIEW_CHESS_UI_CHESS_GAME_BOARD_WRAPPER_H__
#define __GAME_VIEW_CHESS_UI_CHESS_GAME_BOARD_WRAPPER_H__
#include <functional>
#include <memory>
#include "game/view/base/ui_game_board_wrapper.h"

GAME_NAMESPACE_BEGIN

class GridIndex;
class UIChessGameBoard;
class UIAnimationLayer;

class UIChessGameBoardWrapper : public UIGameBoardWrapper
{
	Q_OBJECT

	class OperationalDelegate;
	
public:
	static UIChessGameBoardWrapper* Create(ClientType client_type, UsageType usage_type, QWidget* parent);

	~UIChessGameBoardWrapper();

	// override GameBoardWrapper
	virtual void				HandleMasterCommandSpecially(const CommandSharedPtr&) override;

protected:
	explicit UIChessGameBoardWrapper(ClientType client_type, UsageType usage_type, QWidget* parent = nullptr);

	virtual void				Init() override;

	virtual GameBoardVector		CreateGameBoards() override;
	virtual UIChessBasketBase*	CreateChessBasket() override;
	virtual UIOperationalLayer*	InnerCreateOperationalLayer(QWidget* presentation_layer) override;

private:
	UIChessGameBoard*			game_board_{ nullptr };
	UIAnimationLayer*			animation_layer_{ nullptr };

	OperationalDelegate*		operational_delegate_{ nullptr };

	bool						game_board_enabled_{ true };
};

GAME_NAMESPACE_END

#endif