#ifndef STOCKFISH_ADAPTER_MOVE_INFO_H_INCLUDED
#define STOCKFISH_ADAPTER_MOVE_INFO_H_INCLUDED
#include "stockfish/macros.h"
#include "stockfish/src/types.h"

STK_NAMESPACE_BEGIN

struct MoveDetail
{
	Move	move_{ Move::MOVE_NONE };

	Square	square_from_{ Square::SQ_NONE };
	Square	square_to_{ Square::SQ_NONE };

	bool	is_short_castling_{ false };			// ����λ(������λ) O-O/
	bool	is_long_castling_{ false };				// ����λ(������λ) O-O-O/

	bool	is_captured_{ false };					// ���� x/
	Square	square_captured_{ Square::SQ_NONE };	// �����ӵĸ��ӣ��Թ�·��ʱ����/

	bool	is_promotion_{ false };					// ���� =/
	Piece	piece_to_promote_{ Piece::NO_PIECE };	// ������ɵ�����/

	bool	is_check_{ false };						// ���� +/
	bool	is_checkmate_{ false };					// ���� #/

	inline bool	IsValid() const { return move_ != Move::MOVE_NONE; }
	explicit operator bool() const { return IsValid(); }
};

STK_NAMESPACE_END

#endif