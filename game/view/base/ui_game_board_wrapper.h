#ifndef __GAME_VIEW_BASE_UI_GAME_BOARD_WRAPPER_H__
#define __GAME_VIEW_BASE_UI_GAME_BOARD_WRAPPER_H__
#include <functional>
#include <vector>
#include <QWidget>
#include "game/model/base/client_type.h"
#include "game/model/base/usage_type.h"
#include "game/model/command_handler/command_handler_fwd.h"
#include "game/model/command_handler/game_board_wrapper.h"
#include "protocol/command/base/command_fwd.h"

class DrawBoard;

GAME_NAMESPACE_BEGIN

class GameBoardTuple;
class GameBoardTuples;
class MessageProcessor;
class UIGameBoardBase;
class UIAnimationLayer;
class UIOperationalLayer;

typedef std::vector<UIGameBoardBase*> GameBoardVector;

class UIChessBasketBase;

class UIGameBoardWrapper : public QWidget, public GameBoardWrapper
{
	Q_OBJECT

	class MessageCommandObserver;

	typedef std::shared_ptr<MessageProcessor>	MessageProcessorPtr;
	typedef std::function<void(const CommandSharedPtr&)>	CommandSender;

public:
	~UIGameBoardWrapper();

	inline ClientType			GetClientType() const { return client_type_; }
	inline bool					ClientTypeIs(const ClientType client_type) const { return client_type_ == client_type; }
	inline bool					ClientTypeIsTeacher() const { return ClientTypeIs(ClientType::Teacher); }
	inline bool					ClientTypeIsStudent() const { return ClientTypeIs(ClientType::Student); }

	inline UsageType			GetUsageType() const { return usage_type_; }
	inline bool					UsageTypeIs(const UsageType usage_type) const { return usage_type_ == usage_type; }
	inline bool					UsageTypeIsPlayback() const { return UsageTypeIs(UsageType::Playback_O2O) || UsageTypeIs(UsageType::Playback_Class); }

	void						SetMessageProcessor(const MessageProcessorPtr& msg_processor);

	void						SetCommandSender(const CommandSender& sender);

	void						SetRuleBased(bool rule_based);

	void						SetDrawBoardActive(bool active);

	void						SetGameBoardEnabled(bool game_board_enabled);
	inline bool					IsGameBoardEnabled() const { return game_board_enabled_; }

	void						CreateOperationalLayer(QWidget* presentation_layer = nullptr);

	UIGameBoardBase*			CurrentGameBoard() const;
	DrawBoard*					CurrentDrawBoard() const;
	UIAnimationLayer*			CurrentAnimationLayer() const;
	inline UIChessBasketBase*	ChessBasket() const { return chess_basket_; }

	// override GameBoardWrapper
	virtual void				SwithToGameBoard(GameBoardType) override final;
	virtual DrawBoard*			DrawBoardOnGameBoard(GameBoardType) const override;
	virtual void				HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr) override;

protected:
	explicit UIGameBoardWrapper(ClientType client_type, UsageType usage_type, QWidget* parent = nullptr);

	virtual void				Init();

	virtual GameBoardVector		CreateGameBoards() = 0;
	virtual UIChessBasketBase*	CreateChessBasket() = 0;
	virtual QSize				CalculteSize();
	virtual UIOperationalLayer*	InnerCreateOperationalLayer(QWidget* presentation_layer) = 0;

	virtual	void				ZOrderUi();

	virtual void				resizeEvent(QResizeEvent* resize_event) override;

protected:
	ClientType					client_type_{ ClientType::None };
	UsageType					usage_type_{ UsageType::None };

	GameBoardTuples*			game_board_tuples_{ nullptr };
	GameBoardTuple*				game_board_tuple_active_{ nullptr };

	UIChessBasketBase*			chess_basket_{ nullptr };
	UIOperationalLayer*			operational_layer_{ nullptr };

	CommandHandlerPtr			command_handler_;
	MessageProcessorPtr			message_processor_;
	MessageCommandObserver*		command_observer_{ nullptr };

	bool						game_board_enabled_{ true };
};

GAME_NAMESPACE_END

#endif