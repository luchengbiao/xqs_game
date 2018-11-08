#ifndef __GAME_MODEL_CHESS_CHESS_TRAIT_H__
#define __GAME_MODEL_CHESS_CHESS_TRAIT_H__
#include <string>
#include "chess_enum.h"
#include "game/model/base/chess/chess_info_adapter.h"

GAME_NAMESPACE_BEGIN

class ChessInfoMatrix;
class GridIndex;

struct ChessTrait
{
	using Color = ChessColor;
	using Type = ChessType;

	static Color ColorById(const ChessId chess_id);

	static Type TypeById(const ChessId chess_id);

	static ChessId IdByColorAndType(const Color color, const Type type);

	static inline const ChessId IdOfWhiteType(const Type type) { return IdByColorAndType(Color::White, type); }
	static inline const ChessId IdOfBlackType(const Type type) { return IdByColorAndType(Color::Black, type); }

	static inline const ChessId IdOfEmpty() { return -1; } //-1
	static inline const ChessId IdOfWhitePawn() { static auto chess_id = IdOfWhiteType(Type::Pawn); return chess_id; } // 0
	static inline const ChessId IdOfWhiteRook() { static auto chess_id = IdOfWhiteType(Type::Rook); return chess_id; } // 1
	static inline const ChessId IdOfWhiteKnight() { static auto chess_id = IdOfWhiteType(Type::Knight); return chess_id; } // 2
	static inline const ChessId IdOfWhiteBishop() { static auto chess_id = IdOfWhiteType(Type::Bishop); return chess_id; } // 3
	static inline const ChessId IdOfWhiteQueen() { static auto chess_id = IdOfWhiteType(Type::Queen); return chess_id; } // 4
	static inline const ChessId IdOfWhiteKing() { static auto chess_id = IdOfWhiteType(Type::King); return chess_id; } // 5
	static inline const ChessId IdOfBlackPawn() { static auto chess_id = IdOfBlackType(Type::Pawn); return chess_id; } // 10
	static inline const ChessId IdOfBlackRook() { static auto chess_id = IdOfBlackType(Type::Rook); return chess_id; } // 11
	static inline const ChessId IdOfBlackKnight() { static auto chess_id = IdOfBlackType(Type::Knight); return chess_id; } // 12
	static inline const ChessId IdOfBlackBishop() { static auto chess_id = IdOfBlackType(Type::Bishop); return chess_id; } // 13
	static inline const ChessId IdOfBlackQueen() { static auto chess_id = IdOfBlackType(Type::Queen); return chess_id; } // 14
	static inline const ChessId IdOfBlackKing() { static auto chess_id = IdOfBlackType(Type::King); return chess_id; } // 15

	static inline bool IsSameColor(const ChessInfoAdapter& chess_lh, const ChessInfoAdapter& chess_rh) { return IsSameColor(chess_lh.GetId(), chess_rh.GetId()); }
	static inline bool IsSameColor(const ChessInfo* chess_lh, const ChessInfo* chess_rh) { return IsSameColor(chess_lh->GetId(), chess_rh->GetId()); }
	static inline bool IsSameColor(const ChessId chess_id_lh, const ChessId chess_id_rh) { return ColorById(chess_id_lh) == ColorById(chess_id_rh); }

	static std::string MovingImagePathById(const ChessId chess_id);
	static std::string NormalImagePathById(const ChessId chess_id);
	static std::string SelectedImagePathById(const ChessId chess_id);

	static std::string NotationOfMove(const ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to);
	static std::string NotationOfBoard(const ChessInfoMatrix& matrix);
};

GAME_NAMESPACE_END

#endif