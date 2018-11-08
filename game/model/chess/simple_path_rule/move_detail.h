#ifndef __GAME_MODEL_CHESS_SIMPLE_PATH_RULE_MOVE_DETAIL_H__
#define __GAME_MODEL_CHESS_SIMPLE_PATH_RULE_MOVE_DETAIL_H__
#include "game/model/base/grid/grid_index.h"
#include "game/model/base/chess/chess_info.h"
#include "move_type.h"

GAME_NAMESPACE_BEGIN

struct ChessMoveDetail
{
	ChessId			chess_id_from_{ ChessInfo::EmptyId };

	GridIndex		grid_to_;

	ChessMoveType	move_type_{ ChessMoveType::None };

	GridIndex		grid_captured_; // ����ǳ�����������¼������λ����Ϣ��һ���������grid_to_���Թ�·��ʱ��������/

	ChessMoveDetail(ChessId chess_id_from, const GridIndex& grid_to, ChessMoveType move_type, const GridIndex& grid_captured = GridIndex::InvalidGrid)
		: chess_id_from_(chess_id_from), grid_to_(grid_to), move_type_(move_type), grid_captured_(grid_captured)
	{}
};

GAME_NAMESPACE_END

#endif