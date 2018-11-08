#ifndef __GAME_MODEL_CHESS_CHESS_BOARD_INFO_ADAPTER_FOR_RULE_BASE_H__
#define __GAME_MODEL_CHESS_CHESS_BOARD_INFO_ADAPTER_FOR_RULE_BASE_H__
#include <assert.h>
#include <memory>
#include "game/model/chess/chess_enum.h"

GAME_NAMESPACE_BEGIN

struct BoardInfoAdapter
{
	template<typename ChessBoardT>
	BoardInfoAdapter(const ChessBoardT& chess_board)
	{
		chess_board.ChessIds(chesses_);
	}

	int RowForwardStepOfColor(const ChessColor color) const
	{
		return color == ChessColor::White ? -1 : 1;
	}

	int BottomRowIndexOfColor(const ChessColor color) const
	{
		return color == ChessColor::White ? 0 : 7;
	}

	int SecondRowIndexOfColor(const ChessColor color) const
	{
		return color == ChessColor::White ? 6 : 1;
	}

	int EnpassantRowIndexOfColor(const ChessColor color) const
	{
		return color == ChessColor::White ? 4 : 3;
	}

	ChessId ChessIdAt(const GridIndex& grid) const
	{
		if (grid.X() >= 0 && grid.X() < 8 && grid.Y() >= 0 && grid.Y() < 8)
		{
			return chesses_[grid.X()][grid.Y()];
		}
		
		return ChessInfo::EmptyId;
	}

private:
	ChessId  chesses_[8][8];
};

GAME_NAMESPACE_END

#endif