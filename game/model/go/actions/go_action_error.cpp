#include "game/macros.h"

GAME_NAMESPACE_BEGIN

namespace go_actoin_error
{
	const char*  ParamErrorOnFromGrid = "param error on from-grid";
	const char*  ParamErrorOnToGrid = "param error on to-grid";
	const char*  ExistingChessOnToGrid = "existing chess on to-grid";
	const char*  NotExistingChessOnFromGrid = "not existing chess on from-grid";
	const char*  CannotTakeChessOfOwnColorOnToGrid = "cannot take chess of own color on to-grid";
	const char*  ChessIdMismatchingOnFromGrid = "chess id mismatching on from-grid";
	const char*  CannotMovePieceOutofBoardActivelyWithRule = "cannot move piece out of board actively with rule";
	const char*  CannotMovePieceInsideofBoardActivelyWithRule = "cannot move piece inside of board actively with rule";
	const char*  CannotClearBoardWithRule = "cannot clear board with rule";
	const char*  CannotMoveInPiecesOfSameColorContinuouslyWithRule = "cannot move in pieces of same color continuously with rule";
	const char*  ToGridIsANoGoPoint = "to-grid is a no-go point";
	const char*  ToGridIsKoPoint = "to-grid is a ko point";
}

GAME_NAMESPACE_END