#include "ui_game_board_wrapper.h"
#include <assert.h>
#include <QPointer>
#include <QResizeEvent>
#include "base_control/painter/draw_board.h"
#include "common/uiutils/ui_utils.h"
#include "game/model/command_handler/command_handler.h"
#include "game/model/message_processor/message_processor.h"
#include "game/view/animation/ui_animation_layer.h"
#include "game/view/base/ui_chess_basket_base.h"
#include "game/view/base/ui_game_board_base.h"
#include "game/view/base/ui_operational_layer.h"
#include "protocol/command/base/command.h"

using namespace proto;

GAME_NAMESPACE_BEGIN

class GameBoardTuple
{
	typedef  QPointer<UIGameBoardBase>	GameBoardPtr;
	typedef  QPointer<DrawBoard>		DrawBoardPtr;
	typedef  QPointer<UIAnimationLayer>	AnimationLayerPtr;

public:
	inline GameBoardType		BoardType() const { return game_board_ ? game_board_->BoardType() : GameBoardType::None; }
	inline UIGameBoardBase*		GetGameBoard() const { return game_board_; }
	inline DrawBoard*			GetDrawBoard() const { return draw_board_; }
	inline UIAnimationLayer*	GetAnimationLayer() const { return animatian_layer_; }

	inline void					SetGameBoard(UIGameBoardBase* game_board) { game_board_ = game_board; }
	inline void					SetDrawBoard(DrawBoard* draw_board) { draw_board_ = draw_board; }
	inline void					SetAnimationLayer(UIAnimationLayer* animation_layer) { animatian_layer_ = animation_layer; }

	void Show()
	{
		if (game_board_) { game_board_->show(); }

		if (draw_board_) 
		{ 
			draw_board_->SetVisibleEx(true); 
			draw_board_->show(); 
		}

		if (animatian_layer_) { animatian_layer_->show(); }
	}

	void Hide()
	{
		if (game_board_) { game_board_->hide(); }

		if (draw_board_)
		{
			draw_board_->SetVisibleEx(false);
			draw_board_->hide();
		}

		if (animatian_layer_) { animatian_layer_->hide(); }
	}

	void Raise()
	{
		if (game_board_) { game_board_->raise(); }

		if (animatian_layer_) { animatian_layer_->raise(); }

		if (draw_board_)
		{
			draw_board_->raise();
		}
	}

	void Lower()
	{
		if (draw_board_)
		{
			draw_board_->lower();
		}

		if (animatian_layer_) { animatian_layer_->lower(); }

		if (game_board_) { game_board_->lower(); }
	}

private:
	GameBoardPtr				game_board_{ nullptr };
	DrawBoardPtr				draw_board_{ nullptr };
	AnimationLayerPtr			animatian_layer_{ nullptr };
};

class GameBoardTuples : private std::vector<GameBoardTuple*>
{
public:
	GameBoardTuples() = default;

	~GameBoardTuples()
	{
		for (auto truple : *this)
		{
			delete truple;
		}
	}

	GameBoardTuples(GameBoardTuples&) = delete;
	GameBoardTuples& operator=(GameBoardTuples) = delete;

	inline GameBoardTuple& Front() const
	{
		return *front();
	}

	inline GameBoardTuple& Back() const
	{
		return *back();
	}

	inline void Push(UIGameBoardBase* game_board, DrawBoard* draw_board, UIAnimationLayer* animation_layer)
	{
		auto truple = new GameBoardTuple;

		truple->SetGameBoard(game_board);
		truple->SetDrawBoard(draw_board);
		truple->SetAnimationLayer(animation_layer);

		this->emplace_back(truple);
	}

	GameBoardTuple* TrupleByType(const GameBoardType board_type) const
	{
		for (auto truple : *this)
		{
			if (truple->BoardType() == board_type)
			{
				return truple;
			}
		}

		return nullptr;
	}

	UIGameBoardBase* GameBoardByType(const GameBoardType board_type) const 
	{ 
		for (auto truple : *this)
		{
			if (truple->BoardType() == board_type)
			{
				return truple->GetGameBoard();
			}
		}

		return nullptr;
	}

	DrawBoard* DrawBoardByType(const GameBoardType board_type) const
	{
		for (auto truple : *this)
		{
			if (truple->BoardType() == board_type)
			{
				return truple->GetDrawBoard();
			}
		}

		return nullptr;
	}

	UIAnimationLayer* AnimationLayerdByType(const GameBoardType board_type) const
	{
		for (auto truple : *this)
		{
			if (truple->BoardType() == board_type)
			{
				return truple->GetAnimationLayer();
			}
		}

		return nullptr;
	}

	inline void Foreach(const std::function<void(GameBoardTuple&)>& callback)
	{
		for (auto truple : *this)
		{
			callback(*truple);
		}
	}
};

class UIGameBoardWrapper::MessageCommandObserver : public MessageProcessor::CommandObserver
{
public:
	MessageCommandObserver(UIGameBoardWrapper* board_wrapper)
		: board_wrapper_(board_wrapper)
	{}

	virtual void OnCommand(const CommandSharedPtr& cmd_ptr, bool sender_is_self) override
	{
		if (board_wrapper_->command_handler_)
		{
			board_wrapper_->command_handler_->HandleMasterCommand(cmd_ptr);
		}
	}

	UIGameBoardWrapper* board_wrapper_{ nullptr };
};

UIGameBoardWrapper::UIGameBoardWrapper(ClientType client_type, UsageType usage_type, QWidget* parent)
: QWidget(parent)
, client_type_(client_type)
, usage_type_(usage_type)
, game_board_tuples_(new GameBoardTuples)
{
	setWindowFlags(Qt::FramelessWindowHint);
	setAutoFillBackground(true);

	this->setAttribute(Qt::WA_TranslucentBackground, true);
}

UIGameBoardWrapper::~UIGameBoardWrapper()
{
	delete game_board_tuples_;

	if (message_processor_)
	{
		message_processor_->RemoveCommandObserver(command_observer_);
	}

	delete command_observer_;

	command_handler_->SetGameBoardWrapper(nullptr);
}

void UIGameBoardWrapper::Init()
{
	//1: create command handler
	command_handler_ = std::make_shared<CommandHandler>();
	command_handler_->SetGameBoardWrapper(this);
	command_handler_->SetClientAndUsageType(client_type_, usage_type_);

	//2: create chess basket
	chess_basket_ = this->CreateChessBasket();
	assert(chess_basket_);
	if (!chess_basket_->parent())
	{
		chess_basket_->setParent(this);
	}

	//3: create game boards
	auto vec_game_board = this->CreateGameBoards();
	assert(!vec_game_board.empty());
	for (auto game_board : vec_game_board)
	{
		if (!game_board->parent())
		{
			game_board->setParent(this);
		}

		game_board->SetClientType(GetClientType());
		game_board->SetUsageType(GetUsageType());

		// create draw board for each game board
		DrawBoard* draw_board = nullptr;
		{
			PaintBaseInfo paint;
			paint.width = game_board->width();
			paint.height = game_board->height();
			paint.offset_x_ = 0;
			paint.offset_y_ = 0;
			paint.b_stu_ = (GetClientType() == ClientType::Student);
			paint.b_accept_event_ = false;

			draw_board = new DrawBoard(paint, this);
			draw_board->setFixedSize(game_board->size());
			draw_board->SetShowBkImage(false);
			draw_board->SetTeaPenColor(QColor(255, 0, 0));
			draw_board->SetVisibleEx(false);

			if (UsageType::O2O == GetUsageType() || UsageType::Class == GetUsageType())
			{
				draw_board->SetDrawCb([=](const DrawOpInfo& draw_info){
					if (command_handler_)
					{
						command_handler_->SendDrawCommand(draw_info);
					}
				});
			}
		}

		// create animation layer for each game board
		auto animation_layer = new UIAnimationLayer(this);
		animation_layer->SetAnimationDelegate(game_board);

		game_board->AssociateAnimationLayer(animation_layer);
		game_board->AssociateChessBasket(chess_basket_);

		game_board_tuples_->Push(game_board, draw_board, animation_layer);

		game_board_tuples_->Back().Hide();
	}
	this->SwithToGameBoard(game_board_tuples_->Front().BoardType());

	//4: calculate size
	const QSize size = this->CalculteSize();
	this->setFixedSize(size);
}

QSize UIGameBoardWrapper::CalculteSize()
{
	QSize sz_game_board(0, 0);
	const QSize sz_chess_basket = chess_basket_->size();
	const int space = 0;

	game_board_tuples_->Foreach([&sz_game_board](const GameBoardTuple& truple){
		auto game_board = truple.GetGameBoard();
		if (game_board->width() > sz_game_board.width())
		{
			sz_game_board.setWidth(game_board->width());
		}
		
		if (game_board->height() > sz_game_board.height())
		{
			sz_game_board.setHeight(game_board->height());
		}
	});

	const QSize size(sz_game_board.width() + sz_chess_basket.width() + space, max(sz_game_board.height(), sz_chess_basket.height()));

	return QSize(sz_game_board.width() + sz_chess_basket.width() + space, max(sz_game_board.height(), sz_chess_basket.height()));
}

void  UIGameBoardWrapper::SetMessageProcessor(const MessageProcessorPtr& msg_processor)
{
	if (message_processor_ && command_observer_)
	{
		message_processor_->RemoveCommandObserver(command_observer_);
	}

	message_processor_ = msg_processor;

	if (!command_observer_)
	{
		command_observer_ = new MessageCommandObserver(this);
	}

	if (message_processor_)
	{
		message_processor_->AddCommandObserver(command_observer_);
	}
}

void  UIGameBoardWrapper::SetCommandSender(const CommandSender& sender)
{
	command_handler_->SetCommandSender(sender);

	game_board_tuples_->Foreach([this](const GameBoardTuple& truple){
		truple.GetGameBoard()->SetCommandHandler(command_handler_);
	});
}

void  UIGameBoardWrapper::SetRuleBased(bool rule_based)
{
	game_board_tuples_->Foreach([=](const GameBoardTuple& truple){
		truple.GetGameBoard()->SetRuleBased(rule_based);
	});
}

void  UIGameBoardWrapper::SetDrawBoardActive(bool active)
{
	game_board_tuples_->Foreach([=](GameBoardTuple& truple){
		truple.GetDrawBoard()->SetAcceptEvent(active && IsGameBoardEnabled());
	});

	this->ZOrderUi();
}

void  UIGameBoardWrapper::SetGameBoardEnabled(bool game_board_enabled)
{
	if (game_board_enabled_ == game_board_enabled) { return; }

	game_board_enabled_ = game_board_enabled;

	chess_basket_->SetBasketEnabled(IsGameBoardEnabled());

	if (operational_layer_)
	{
		operational_layer_->setEnabled(IsGameBoardEnabled());
	}

	this->ZOrderUi();
}

void UIGameBoardWrapper::CreateOperationalLayer(QWidget* presentation_layer)
{
	if (!operational_layer_)
	{
		operational_layer_ = this->InnerCreateOperationalLayer(presentation_layer);
		if (!operational_layer_->parent())
		{
			operational_layer_->setParent(this);
		}

		operational_layer_->setFixedSize(this->size());

		game_board_tuples_->Foreach([=](GameBoardTuple& truple){
			truple.GetGameBoard()->AssociateOperationalLayer(operational_layer_);
		});

		this->ZOrderUi();
	}
}

UIGameBoardBase* UIGameBoardWrapper::CurrentGameBoard() const
{
	assert(game_board_tuple_active_);

	return game_board_tuple_active_->GetGameBoard();
}

DrawBoard* UIGameBoardWrapper::CurrentDrawBoard() const
{
	assert(game_board_tuple_active_);

	return game_board_tuple_active_->GetDrawBoard();
}

UIAnimationLayer* UIGameBoardWrapper::CurrentAnimationLayer() const
{
	assert(game_board_tuple_active_);

	return game_board_tuple_active_->GetAnimationLayer();
}

void UIGameBoardWrapper::SwithToGameBoard(GameBoardType target_board_type)
{
	if (!game_board_tuple_active_ || game_board_tuple_active_->BoardType() != target_board_type)
	{
		if (game_board_tuple_active_) { game_board_tuple_active_->Hide(); }

		game_board_tuple_active_ = game_board_tuples_->TrupleByType(target_board_type);
		assert(game_board_tuple_active_);

		game_board_tuple_active_->Show();

		this->ZOrderUi();
	}

	command_handler_->SwitchToGameBoard(target_board_type);
}

DrawBoard* UIGameBoardWrapper::DrawBoardOnGameBoard(GameBoardType board_type) const
{
	return game_board_tuples_->DrawBoardByType(board_type);
}

void UIGameBoardWrapper::HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr)
{
	const auto cmd_tpye = cmd_ptr->Type();
	if (cmd_tpye == CommandType::TeacherForbiddenStudentOper)
	{
		if (ClientTypeIsStudent() || UsageTypeIsPlayback())
		{
			this->SetGameBoardEnabled(!cmd_ptr->IntOfVariantAt(0));
		}
	}
	else if (cmd_tpye == CommandType::TeacherNewGameBoard)
	{
		CurrentDrawBoard()->ClearDrawedUnits();

		this->SetDrawBoardActive(false);
	}

	game_board_tuples_->Foreach([=](GameBoardTuple& truple){
		truple.GetGameBoard()->HandleMasterCommandSpecially(cmd_ptr);
	});

	chess_basket_->HandleMasterCommandSpecially(cmd_ptr);
}

void UIGameBoardWrapper::ZOrderUi()
{
	assert(game_board_tuple_active_);

	auto board_type_active = game_board_tuple_active_->BoardType();

	game_board_tuples_->Foreach([=](GameBoardTuple& truple){
		(truple.BoardType() == board_type_active) ? truple.Raise() : truple.Lower();
	});

	ChessBasket()->raise();

	CurrentAnimationLayer()->raise();

	auto current_draw_board = CurrentDrawBoard();

	bool draw_board_over_operational_layer = current_draw_board->IsEventAccepted() && IsGameBoardEnabled();

	if (draw_board_over_operational_layer)
	{
		if (operational_layer_)
		{
			operational_layer_->raise();
		}

		current_draw_board->raise();
	}
	else
	{
		current_draw_board->raise();

		if (operational_layer_)
		{
			operational_layer_->raise();
		}
	}
}

void  UIGameBoardWrapper::resizeEvent(QResizeEvent* resize_event)
{
	const QSize size = resize_event->size();

	UiUtils::MoveWidgetLeftTopTo(chess_basket_, QPoint(0, 0));
	game_board_tuples_->Foreach([&](GameBoardTuple& truple){
		UiUtils::MoveWidgetRightTopTo(truple.GetGameBoard(), QPoint(size.width(), 0));

		truple.GetDrawBoard()->move(truple.GetGameBoard()->pos());
		truple.GetAnimationLayer()->setGeometry(0, 0, size.width(), size.height());
	});

	if (operational_layer_)
	{
		operational_layer_->setFixedSize(size);
	}
}

GAME_NAMESPACE_END