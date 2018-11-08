#include "chess_info_matrix.h"
#include <assert.h>

GAME_NAMESPACE_BEGIN

ChessInfoMatrix::ChessInfoMatrix(int rows, int cols)
: rows_(rows)
, cols_(cols)
{
	assert(rows_ > 0 && cols_ > 0);

	int count = rows_ * cols_;
	for (int i = 0; i < count; ++i)
	{
		chesses_.emplace_back(new ChessInfo);
	}
}

ChessInfoMatrix::~ChessInfoMatrix()
{
	for (auto ptr : chesses_)
	{
		delete ptr;
	}
}

ChessInfo* ChessInfoMatrix::ChessAt(const GridIndex& grid) const
{ 
	return ChessAt(grid.Y(), grid.X()); 
}

ChessInfo* ChessInfoMatrix::ChessAt(int x, int y) const
{ 
	if (y >= 0 && y < rows_ && x >= 0 && x < cols_)
	{
		return chesses_[IndexByXY(x, y)];
	}
	 
	return nullptr;
}

ChessId ChessInfoMatrix::ChessIdAt(const GridIndex& grid) const	
{ 
	return this->ChessIdAt(grid.Y(), grid.X());
}

ChessId ChessInfoMatrix::ChessIdAt(int x, int y) const
{ 
	auto chess_info = this->ChessAt(x, y);

	return chess_info ? chess_info->GetId() : ChessInfo::EmptyId;
}

void ChessInfoMatrix::CopyChessesFrom(const ChessInfoMatrix& matrix)
{
	assert(rows_ == matrix.rows_ && cols_ == matrix.cols_);

	for (int i = 0; i < chesses_.size(); ++i)
	{
		chesses_[i]->SetId(matrix.chesses_[i]->GetId());
	}
}

GAME_NAMESPACE_END