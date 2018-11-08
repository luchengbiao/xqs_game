#ifndef __GAME_MODEL_BASE_ACTION_GAME_ACTION_H__
#define __GAME_MODEL_BASE_ACTION_GAME_ACTION_H__
#include "base/memory/objectcounter.h"
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

class GameBoardBase;
class GameActionResult;

class GameAction
#ifdef GAME_TEST_MEMORY_LEAK
	: public nbase::ObjectCounter<GameAction>
#endif
{
public:
	virtual ~GameAction() = default;

	virtual	GameActionResult	Do(GameBoardBase*) = 0;
	virtual	void				Undo(GameBoardBase*) = 0;
	virtual	void				Redo(GameBoardBase*) = 0;

	virtual bool				PropertyOfChangingBoard() const = 0;
	virtual bool				PropertyOfBeingInnerActionInOneGame() const = 0;
};

GAME_NAMESPACE_END

#endif