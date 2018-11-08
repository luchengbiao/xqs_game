#ifndef __GAME_MODEL_COMMAND_HANDLER_DRAW_BOARD_HODLER_H__
#define __GAME_MODEL_COMMAND_HANDLER_DRAW_BOARD_HODLER_H__
#include "game/model/base/board/game_board_type.h"
#include "protocol/command/base/command_fwd.h"

class DrawBoard;

GAME_NAMESPACE_BEGIN

class GameBoardWrapper
{
public:
	virtual ~GameBoardWrapper() = default;

	virtual void		SwithToGameBoard(GameBoardType) = 0;

	virtual DrawBoard*	DrawBoardOnGameBoard(GameBoardType) const = 0;

	virtual	void		HandleMasterCommandSpecially(const CommandSharedPtr&) = 0;
};

GAME_NAMESPACE_END

#endif