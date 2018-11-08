#ifndef __GAME_MODEL_COMMAND_HANDLER_COMMAND_HANDLER_H__
#define __GAME_MODEL_COMMAND_HANDLER_COMMAND_HANDLER_H__
#include <functional>
#include <list>
#include <QPointer>
#include "base_control/painter/paint_data.h"
#include "game/model/base/board/game_board_type.h"
#include "game/model/base/board/game_board_base_fwd.h"
#include "game/model/base/client_type.h"
#include "game/model/base/usage_type.h"
#include "game/model/command_handler/command_conflict_level.h"
#include "protocol/command/base/command_fwd.h"

class DrawBoard;
struct DrawOpInfo;

GAME_NAMESPACE_BEGIN

class GameBoardWrapper;
//typedef QPointer<GameBoardWrapper> GameBoardWrapperPtr;
typedef GameBoardWrapper* GameBoardWrapperPtr;

class CommandCache;
class GameBoardPairs;
class GameActionResult;
class GameDrawBoardInfo;

class CommandHandler
{
	using CommandSender = std::function<void(const CommandSharedPtr&)>;

public:
	CommandHandler();
	~CommandHandler();

	void				SetCommandSender(const CommandSender& sender) { sender_ = sender; }

	void				SendCommand(const CommandSharedPtr& cmd_ptr);
	void				SendDrawCommand(const DrawOpInfo& draw_info);

	void				SetGameBoardWrapper(GameBoardWrapper* game_board_wrapper);
	void				SetClientAndUsageType(ClientType client_type, UsageType	usage_type);

	void				HandleMasterCommand(const CommandSharedPtr& cmd_ptr);
	CmdConflictLevel	CheckCommandConflictBetweenMasterAndLocal(const CommandSharedPtr& master_cmd_ptr);

	bool				TestSyncLocalWithMasterWhenMasterMoved(GameBoardType board_type, const GameActionResult& result_on_master);

	void				DelegateMasterGameBoard(const GameBoardPtr& game_board);
	void				DelegateLocalGameBoard(const GameBoardPtr& game_board);

	void				SwitchToGameBoard(GameBoardType board_type);

private:
	void				TryToSwitchGameBoardBeforeNew(const CommandSharedPtr& cmd_ptr);

	void				HandleLocalCommand(const CommandSharedPtr& cmd_ptr);

	void				HandleMasterCommandInfluencingDrawBoard(const CommandSharedPtr& cmd_ptr);
	void				HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr);

	DrawBoard*			DrawBoardOnGameBoard(GameBoardType board_type);

	inline ClientType	GetClientType() const { return client_type_; }
	inline bool			ClientTypeIs(const ClientType client_type) const { return client_type_ == client_type; }
	inline bool			ClientTypeIsTeacher() const { return ClientTypeIs(ClientType::Teacher); }
	inline bool			ClientTypeIsStudent() const { return ClientTypeIs(ClientType::Student); }

	inline UsageType	GetUsageType() const { return usage_type_; }
	inline bool			UsageTypeIs(const UsageType usage_type) const { return usage_type_ == usage_type; }
	inline bool			UsageTypeIsPlayback() const { return UsageTypeIs(UsageType::Playback_O2O) || UsageTypeIs(UsageType::Playback_Class); }

	void				TeacherDraw(const std::list<DrawOpInfo>&);
	void				StudentDraw(const std::list<DrawOpInfo>&);
	
private:
	CommandSender		sender_;

	GameBoardPairs*		board_pairs_{ nullptr };
	CommandCache*		master_chess_cmd_cache_{ nullptr };
	CommandCache*		local_chess_cmd_cache_{ nullptr };

	GameBoardType		current_board_type_{ GameBoardType::None };

	ClientType			client_type_{ ClientType::None };
	UsageType			usage_type_{ UsageType::None };

	GameBoardWrapperPtr	game_board_wrapper_{ nullptr };
	GameDrawBoardInfo*	draw_board_by_tea_{ nullptr };
	GameDrawBoardInfo*	draw_board_by_stu_{ nullptr };
};

GAME_NAMESPACE_END

#endif