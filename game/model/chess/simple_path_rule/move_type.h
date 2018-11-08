#ifndef __GAME_MODEL_CHESS_SIMPLE_PATH_RULE_MOVE_TYPE_H__
#define __GAME_MODEL_CHESS_SIMPLE_PATH_RULE_MOVE_TYPE_H__
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

enum class ChessMoveType
{
	None				= -1,
	Normal				= 0,	// ��ͨ����/
	Capture				= 1,	// ���� x/
	LongCastling		= 2,	// ����λ, ������λ O-O-O/
	ShortCastling		= 3,	// ����λ, ������λ O-O/
	Enpassant			= 4,	// �Թ�·��/
	Promotion			= 5,	// ���� =/
	CaptureAndPromotion	= 6,	// ����&���� x=/
};

GAME_NAMESPACE_END

#endif