#ifndef __GAME_VIEW_BASE_UI_GAME_BOARD_BASE_H__
#define __GAME_VIEW_BASE_UI_GAME_BOARD_BASE_H__
#include <functional>
#include <memory>
#include <deque>
#include <QPointer>
#include <QWidget>
#include "game/model/base/action/game_action_result.h"
#include "game/model/base/board/game_board_base_fwd.h"
#include "game/model/base/board/game_board_type.h"
#include "game/model/base/client_type.h"
#include "game/model/base/usage_type.h"
#include "game/model/command_handler/command_handler_fwd.h"
#include "game/view/animation/ui_animation_delegate.h"
#include "protocol/command/base/command_fwd.h"
#include "ui_game_board_layout_type.h"

GAME_NAMESPACE_BEGIN

class ChessInfoMatrix;
class GameActionResult;
class UIChessPieceBase;
class UIChessBasketBase;
class UIAnimationLayer;
class UIOperationalLayer;

class UIGameBoardBase : public QWidget, public UIAnimationDelegate
{
	Q_OBJECT

	template<typename UIGameBoardT, typename... Args>
	friend UIGameBoardT* CreateGameBoard(Args&&... args);

	typedef QPointer<UIAnimationLayer>	UIAnimationLayerPtr;
	typedef QPointer<UIChessBasketBase> UIChessBasketPtr;
	typedef QPointer<UIOperationalLayer> UIOperationalLayerPtr;
	
public:
	explicit UIGameBoardBase(QWidget* parent = nullptr);
	~UIGameBoardBase();

	UIGameBoardBase(const UIGameBoardBase&) = delete;
	UIGameBoardBase& operator=(const UIGameBoardBase&) = delete;

	virtual GameBoardType		BoardType() const = 0;
	virtual	int					RowsOfBoard() const = 0;
	virtual	int					ColumnsOfBoard() const  = 0;
	virtual void				SetRuleBased(bool rule_based) = 0;
	virtual int					GridWidth() const = 0;
	virtual QPoint				PositionByGridIndex(const GridIndex& grid_index) const = 0;
	virtual UIChessPieceBase*	ChessPieceAt(const GridIndex& grid) const = 0;
	virtual bool				CanOperateOnBoardNow() const;
	virtual void				SetScaleLineVisible(bool visible) = 0;
	virtual void				HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr);

	inline void					SetClientType(const ClientType client_type) { client_type_ = client_type; }
	inline void					SetUsageType(const UsageType usage_type) { usage_type_ = usage_type; }

	void						SetCommandHandler(const CommandHandlerPtr& command_handler);

	void						AssociateAnimationLayer(UIAnimationLayer* animation_layer);
	void						AssociateChessBasket(UIChessBasketBase* chess_basket);
	void						AssociateOperationalLayer(UIOperationalLayer* operational_layer);

	bool						TryToHandleCommandOnLocalModel(const CommandSharedPtr& cmd_ptr); // command should be generated locally and will be sent to server after "DoCommand"

	void						TryToRefreshLocal();
	bool						TryToMoveChessLocally(ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to);

	ChessId						LocalChessIdAt(const GridIndex& grid_from) const;

	void						HideChessAt(const GridIndex& grid_index);
	void						ShowChessAt(const GridIndex& grid_index);
		
	void						SetLayout(const GameBoardLayout layout);
	inline GameBoardLayout		GetLayout() const { return layout_; }

protected:
	virtual	void				Init();

	virtual void				OnOperationalLayerAssociated() = 0;
	virtual void				OnLayoutChanged() = 0;

	void						SetMasterModel(const GameBoardPtr& model);
	void						SetLocalModel(const GameBoardPtr& model);

	virtual void				SyncLocalWithMaster();

	virtual void				OnMasterDone(const GameActionResult&);
	virtual void				OnMasterUndone(const GameActionResult&);

	virtual void				OnLocalDone(const GameActionResult&);
	virtual void				OnLocalRetracted(const GameActionResult&);

	inline void					SetLocalDirty(bool dirty) { local_is_dirty_ = dirty; }
	inline bool					IsLocalDirty() const { return local_is_dirty_; }

	void						SendCommand(const CommandSharedPtr& cmd_ptr);

	inline ClientType			GetClientType() const { return client_type_; }
	inline bool					ClientTypeIs(const ClientType client_type) const { return client_type_ == client_type; }
	inline bool					ClientTypeIsTeacher() const { return ClientTypeIs(ClientType::Teacher); }
	inline bool					ClientTypeIsStudent() const { return ClientTypeIs(ClientType::Student); }

	inline UsageType			GetUsageType() const { return usage_type_; }
	inline bool					UsageTypeIs(const UsageType usage_type) const { return usage_type_ == usage_type; }
	inline bool					UsageTypeIsPlayback() const { return UsageTypeIs(UsageType::Playback_O2O) || UsageTypeIs(UsageType::Playback_Class); }

protected:
	GameBoardPtr				master_model_;
	GameBoardPtr				local_model_;
	bool						local_is_dirty_{ false }; // flag to lazy sync local with master when operating on board

	CommandHandlerPtr			command_handler_;

	// AnimationLayer & ChessBasket & OperationalLayer is created and passed from the outside,
	// so use QPointer to guard
	UIAnimationLayerPtr			animation_layer_;
	UIChessBasketPtr			chess_basket_;
	UIOperationalLayerPtr		operational_layer_;

private:
	friend class MasterModelObserver;
	MasterModelObserver*		master_model_observer_{ nullptr };

	friend class LocalModelObserver;
	LocalModelObserver*			local_model_observer_{ nullptr };

	ClientType					client_type_{ ClientType::None };
	UsageType					usage_type_{ UsageType::None };

	GameBoardLayout				layout_{ GameBoardLayout::Normal };
};

template<typename UIGameBoardT, typename... Args>
UIGameBoardT* CreateGameBoard(Args&&... args)
{
	UIGameBoardBase* game_board = new (std::nothrow) UIGameBoardT(std::forward<Args>(args)...);

	if (game_board) { game_board->Init(); }

	return static_cast<UIGameBoardT*>(game_board);
}

GAME_NAMESPACE_END

#endif