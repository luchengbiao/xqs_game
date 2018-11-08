#ifndef __GAME_MODEL_GO_ACTIONS_GO_ACTION_ERROR_H__
#define __GAME_MODEL_GO_ACTIONS_GO_ACTION_ERROR_H__
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

namespace go_actoin_error
{
	extern const char*  ParamErrorOnFromGrid;
	extern const char*  ParamErrorOnToGrid;
	extern const char*  ExistingChessOnToGrid;
	extern const char*  NotExistingChessOnFromGrid;
	extern const char*  CannotTakeChessOfOwnColorOnToGrid;
	extern const char*  ChessIdMismatchingOnFromGrid;
	extern const char*  CannotMovePieceOutofBoardActivelyWithRule;
	extern const char*  CannotMovePieceInsideofBoardActivelyWithRule;
	extern const char*  CannotClearBoardWithRule;
	extern const char*  CannotMoveInPiecesOfSameColorContinuouslyWithRule;
	extern const char*  ToGridIsANoGoPoint;
	extern const char*  ToGridIsKoPoint;
}

GAME_NAMESPACE_END

#endif