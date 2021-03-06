#ifndef __GAME_VIEW_GO_OPERATIONAL_UI_GO_OPERATIONAL_LAYER_H__
#define __GAME_VIEW_GO_OPERATIONAL_UI_GO_OPERATIONAL_LAYER_H__
#include <QPointer>
#include "game/view/base/ui_chess_face_fwd.h"
#include "game/view/base/ui_operational_layer.h"
#include "ui_go_operational_delegate.h"

GAME_NAMESPACE_BEGIN

class UIArrowLine;

class UIGoOperationalLayer : public UIOperationalLayer
{
	Q_OBJECT

	typedef std::map<GridIndex, QRect>	GridRectInBoardMap;
	typedef std::map<ChessId, QRect>	ChessRectInBasketMap;
	typedef GoOperationalDelegate		MyOperationalDelegate;

	struct DragInfo;

public:
	explicit UIGoOperationalLayer(QWidget* parent, QWidget* presentation_layer = nullptr);
	~UIGoOperationalLayer();

	void					SetOperationalDelegate(MyOperationalDelegate* operational_delegate);

	virtual void			Interrupt() override;
	virtual void			Clear() override;
	virtual QPoint			CurrentGlobalPosOfChessFaceCenter() const override;

protected:
	virtual void			mousePressEvent(QMouseEvent*) override;
	virtual void			mouseMoveEvent(QMouseEvent*) override;
	virtual void			mouseReleaseEvent(QMouseEvent*) override;

	void					mouseReleaseEvent_ChessIdBeingDragged(QMouseEvent*);
	void					mouseReleaseEvent_NoChessIdBeingDragged(QMouseEvent*);

	QPoint					LocalPosOfBoardGridCenter(const GridIndex& grid_index) { return mapFromGlobal(GlobalPosOfBoardGridCenter(grid_index)); }
	QPoint					LocalPosOfBasketChessCenter(ChessId chess_id) { return mapFromGlobal(GlobalPosOfBasketChessCenter(chess_id)); }

	QPoint					GlobalPosOfBoardGridCenter(const GridIndex& grid_index);
	QPoint					GlobalPosOfBasketChessCenter(ChessId chess_id);

	bool					GlobalPosInBoardGrid(const QPoint& global_pos, GridIndex* grid_index = nullptr, QRect* grid_global_rect = nullptr);
	bool					GlobalPosInBasket(const QPoint& global_pos, ChessId* chess_id = nullptr, QRect* grid_global_rect = nullptr);

	void					LazyGetGridRects();
	void					LazyCreateChessUi();

	void					ShowChessFaceInMovingState(ChessId chess_id);
	void					ShowChessFaceInSelectedState(ChessId chess_id);
	void					MoveChessFaceCenterToPos(const QPoint& local_pos);

	void					MoveAnchorLines(const GridIndex& grid_in_board);
	void					HideAnchorLines();
	void					LazyCreateAnchorLines();

	void					ZOrderUi();

	bool					TryToMoveChess(ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to);

	void					InnerClear();

	inline	void			SetInterrupted(bool interrupted) { interrupted_ = interrupted; }
	inline	bool			IsInterrupted() const { return interrupted_; }

private:
	MyOperationalDelegate*	operational_delegate_{ nullptr };

	GridRectInBoardMap		map_grid_global_rect_in_board_;

	ChessRectInBasketMap	map_chess_global_rect_in_basket_;

	// presentation_layer_ may be from the outside, use QPointer to guard
	// the parent of chess_face_ and arrow_line_ is presentation_layer_
	QPointer<QWidget>		presentation_layer_{ nullptr };
	QPointer<ChessFace>		chess_face_{ nullptr };
	QPointer<UIArrowLine>	arrow_line_{ nullptr };

	WidgetWithPixmap*		x_anchor_line_{ nullptr };
	WidgetWithPixmap*		y_anchor_line_{ nullptr };

	DragInfo*				drag_info_{ nullptr };

	bool					interrupted_{ false };
};

GAME_NAMESPACE_END

#endif