#ifndef __GAME_VIEW_GO_OPERATIONAL_UI_GO_OPERATIONAL_DELEGATE_H__
#define __GAME_VIEW_GO_OPERATIONAL_UI_GO_OPERATIONAL_DELEGATE_H__
#include <map>
#include <QPixmap>
#include <QRect>
#include "game/model/base/chess/chess_info.h"
#include "game/model/base/grid/grid_index.h"

GAME_NAMESPACE_BEGIN

using GridRectInBoardMap = std::map<GridIndex, QRect>;
using ChessRectInBasketMap = std::map<ChessId, QRect>;

class GoOperationalDelegate
{
public:
	virtual ~GoOperationalDelegate() = default;

	virtual bool					CanOperateOnBoardNow() = 0;

	virtual void					OnWillOperateOnBoard() = 0;

	virtual GridRectInBoardMap		GridsGlobalRectInBoard() = 0;

	virtual ChessRectInBasketMap	ChessesGlobalRectInBasket() = 0;

	virtual QPixmap					MovingPixmapOfChessId(ChessId) = 0;

	virtual QPixmap					SelectedPixmapOfChessId(ChessId) = 0;

	virtual bool					GridContainsChessInBoard(const GridIndex& grid_index, ChessId*) = 0;

	virtual void					HideChessInBoardGrid(const GridIndex&) = 0;

	virtual void					ShowChessInBoardGrid(const GridIndex&) = 0;

	virtual void					OnMousePressed(const GridIndex& grid_selected, ChessId chess_id_selected) = 0;

	virtual void					OnMouseWillMove(const GridIndex& grid_selected, ChessId chess_id_selected) = 0;

	virtual void					OnMouseReleased(const GridIndex& grid_from, ChessId chess_id_from, const GridIndex& grid_to, ChessId chess_id_to) = 0;

	virtual bool					TryToMoveChessLocally(ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to) = 0;

	virtual bool					IsChessFixedInBasket(ChessId* chess_id_fixed) = 0;

	virtual bool					IsChessSelectedInBoard(GridIndex* grid_selected, ChessId* chess_id_selected) = 0;
};

GAME_NAMESPACE_END

#endif