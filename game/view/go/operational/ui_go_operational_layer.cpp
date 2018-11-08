#include "ui_go_operational_layer.h"
#include <assert.h>
#include <limits>
#include <QMouseEvent>
#include "common/uiutils/ui_utils.h"
#include "game/view/arrow_line/ui_arrow_line.h"
#include "game/view/base/ui_chess_face_impl.h"

GAME_NAMESPACE_BEGIN

struct UIGoOperationalLayer::DragInfo
{
	ChessId			chess_id_being_dragged_{ ChessInfo::EmptyId };
	GridIndex		grid_from_;
	GridIndex		grid_current_;
	QRect			current_grid_global_rect_;

	bool			mouse_is_moved_{ false };

	inline void		SetChessIdBeingDragged(ChessId chess_id) { chess_id_being_dragged_ = chess_id; }
	inline void		SetGridFrom(const GridIndex& grid_index) { grid_from_ = grid_index; }
	inline void		SetGridCurrent(const GridIndex& grid_index) { grid_current_ = grid_index; }
	inline void		SetCurrentGridGlobalRect(const QRect& rect) { current_grid_global_rect_ = rect; }

	inline void		SetMouseIsMoved(bool mouse_is_moved) { mouse_is_moved_ = mouse_is_moved; }

	inline ChessId	ChessIdBeingDragged() const { return chess_id_being_dragged_; }
	inline const GridIndex& GridFrom() const { return grid_from_; }
	inline const GridIndex& GridCurrent() const { return grid_current_; }
	inline const QRect CurrentGridGlobalRect() const { return current_grid_global_rect_; }

	inline bool		MouseIsMoved() const { return mouse_is_moved_; }

	inline bool		HasChessIdBeingDragged() const { return ChessIdIsNotEmpty(chess_id_being_dragged_); }
};

UIGoOperationalLayer::UIGoOperationalLayer(QWidget* parent, QWidget* presentation_layer)
: UIOperationalLayer(parent)
, presentation_layer_(presentation_layer)
, drag_info_(new DragInfo)
{}

UIGoOperationalLayer::~UIGoOperationalLayer()
{
	delete drag_info_;
}

void UIGoOperationalLayer::SetOperationalDelegate(MyOperationalDelegate* operational_delegate)
{
	operational_delegate_ = operational_delegate;
}

void UIGoOperationalLayer::Interrupt()
{
	this->SetInterrupted(true);

	this->InnerClear();
}

void UIGoOperationalLayer::Clear()
{
	this->InnerClear();
}

void UIGoOperationalLayer::InnerClear()
{
	if (drag_info_->HasChessIdBeingDragged())
	{
		operational_delegate_->ShowChessInBoardGrid(drag_info_->GridFrom());
		drag_info_->SetChessIdBeingDragged(ChessInfo::EmptyId);
	}

	if (chess_face_)
	{
		chess_face_->hide();
	}

	if (arrow_line_)
	{
		arrow_line_->hide();
	}
}

void UIGoOperationalLayer::mousePressEvent(QMouseEvent* mouse_event)
{
	assert(operational_delegate_);

	if (!operational_delegate_->CanOperateOnBoardNow()) { return; }

	operational_delegate_->OnWillOperateOnBoard();

	this->SetInterrupted(false);

	this->LazyGetGridRects();

	drag_info_->SetGridFrom(GridIndex::InvalidGrid);
	drag_info_->SetChessIdBeingDragged(ChessInfo::EmptyId);
	drag_info_->SetMouseIsMoved(false);

	QPoint global_pos = mouse_event->globalPos();

	ChessId chess_id = ChessInfo::EmptyId;
	QRect grid_global_rect;

	if (GlobalPosInBasket(global_pos, &chess_id, &grid_global_rect))
	{
		drag_info_->SetChessIdBeingDragged(chess_id);
		drag_info_->SetGridFrom(GridIndex::InvalidGrid);
	}
	else
	{
		GridIndex grid_index;
		if (GlobalPosInBoardGrid(global_pos, &grid_index, &grid_global_rect))
		{
			drag_info_->SetGridFrom(grid_index);

			if (operational_delegate_->GridContainsChessInBoard(grid_index, &chess_id))
			{
				drag_info_->SetChessIdBeingDragged(chess_id);
			}
		}
	}

	const auto grid_from = drag_info_->GridFrom();
	drag_info_->SetCurrentGridGlobalRect(grid_global_rect);
	drag_info_->SetGridCurrent(grid_from);

	this->MoveAnchorLines(grid_from);

	operational_delegate_->OnMousePressed(grid_from, drag_info_->ChessIdBeingDragged());

	if (drag_info_->HasChessIdBeingDragged())
	{
		this->LazyCreateChessUi();

		auto grid_from_center = grid_from.IsValid() ? GlobalPosOfBoardGridCenter(grid_from) : GlobalPosOfBasketChessCenter(drag_info_->ChessIdBeingDragged());

		if (arrow_line_)
		{
			arrow_line_->SetStartAndEndPosition(grid_from_center, global_pos);
			arrow_line_->show();
		}

		this->ShowChessFaceInSelectedState(drag_info_->ChessIdBeingDragged());
		this->MoveChessFaceCenterToPos(mouse_event->pos());

		if (grid_from.IsValid())
		{
			operational_delegate_->HideChessInBoardGrid(grid_from);
		}
	}
}

void UIGoOperationalLayer::mouseMoveEvent(QMouseEvent* mouse_event)
{
	if (IsInterrupted()) { return; }

	auto global_pos = mouse_event->globalPos();

	if (drag_info_->HasChessIdBeingDragged())
	{
		if (!drag_info_->MouseIsMoved()) // change from SelectedFace to MovingFace when first moved
		{
			drag_info_->SetMouseIsMoved(true);

			this->ShowChessFaceInMovingState(drag_info_->ChessIdBeingDragged());

			operational_delegate_->OnMouseWillMove(drag_info_->GridFrom(), drag_info_->ChessIdBeingDragged());
		}

		this->MoveChessFaceCenterToPos(mouse_event->pos());

		if (arrow_line_)
		{
			arrow_line_->SetEndPosition(global_pos);
		}
	}

	GridIndex grid_current = drag_info_->GridCurrent();

	if (grid_current.IsValid())
	{
		if (!drag_info_->CurrentGridGlobalRect().contains(global_pos))
		{
			QRect grid_global_rect;
			if (GlobalPosInBoardGrid(global_pos, &grid_current, &grid_global_rect))
			{
				drag_info_->SetGridCurrent(grid_current);
				drag_info_->SetCurrentGridGlobalRect(grid_global_rect);
			}
			else
			{
				drag_info_->SetGridCurrent(GridIndex::InvalidGrid);
				drag_info_->SetCurrentGridGlobalRect(QRect(global_pos, QSize(0, 0)));
			}
		}
	}
	else
	{
		QRect grid_global_rect;
		if (GlobalPosInBoardGrid(global_pos, &grid_current, &grid_global_rect))
		{
			drag_info_->SetGridCurrent(grid_current);
			drag_info_->SetCurrentGridGlobalRect(grid_global_rect);
		}
		else
		{
			drag_info_->SetCurrentGridGlobalRect(QRect(global_pos, QSize(0, 0)));
		}
	}

	this->MoveAnchorLines(drag_info_->GridCurrent());
}

void UIGoOperationalLayer::mouseReleaseEvent(QMouseEvent* mouse_event)
{
	this->HideAnchorLines();

	if (IsInterrupted()) { return; }

	if (drag_info_->HasChessIdBeingDragged())
	{
		this->mouseReleaseEvent_ChessIdBeingDragged(mouse_event);
	}
	else
	{
		this->mouseReleaseEvent_NoChessIdBeingDragged(mouse_event);
	}
}

void UIGoOperationalLayer::mouseReleaseEvent_ChessIdBeingDragged(QMouseEvent* mouse_event)
{
	if (arrow_line_) { arrow_line_->hide(); }

	if (chess_face_) { chess_face_->hide(); }

	const GridIndex grid_current = drag_info_->GridCurrent();
	ChessId chess_id_current = ChessInfo::EmptyId;
	if (grid_current.IsValid())
	{
		operational_delegate_->GridContainsChessInBoard(grid_current, &chess_id_current);
	}
	else
	{
		GlobalPosInBasket(mouse_event->globalPos(), &chess_id_current);
	}

	// drag_info_ will be modified in TryToMoveChess frame, so need to save the GridFrom and ChessIdBeingDragged to call OnMouseReleased later
	const GridIndex grid_from = drag_info_->GridFrom();
	const ChessId chess_id_dragging = drag_info_->ChessIdBeingDragged();

	if (grid_from != grid_current)
	{
		this->TryToMoveChess(chess_id_dragging, grid_from, grid_current);
	}
	else
	{
		operational_delegate_->ShowChessInBoardGrid(grid_from);
	}

	operational_delegate_->OnMouseReleased(grid_from, chess_id_dragging, grid_current, chess_id_current);
}

void UIGoOperationalLayer::mouseReleaseEvent_NoChessIdBeingDragged(QMouseEvent* mouse_event)
{
	GridIndex grid_current;
	ChessId chess_id_current = ChessInfo::EmptyId;
	if (GlobalPosInBoardGrid(mouse_event->globalPos(), &grid_current))
	{
		operational_delegate_->GridContainsChessInBoard(grid_current, &chess_id_current);
	}
	else
	{
		GlobalPosInBasket(mouse_event->globalPos(), &chess_id_current);
		grid_current = GridIndex::InvalidGrid;
	}

	// drag_info_ will be modified in TryToMoveChess frame, so need to save the GridFrom and ChessIdBeingDragged to call OnMouseReleased later
	const GridIndex grid_from = drag_info_->GridFrom();
	const ChessId chess_id_dragging = drag_info_->ChessIdBeingDragged(); // should be ChessInfo::EmptyId

	ChessId chess_id = ChessInfo::EmptyId;
	if (operational_delegate_->IsChessFixedInBasket(&chess_id))
	{
		if (grid_current.IsValid())
		{
			// CurrentGlobalPosOfChessFaceCenter will be called in TryToMoveChess frame 
			this->MoveChessFaceCenterToPos(LocalPosOfBasketChessCenter(chess_id));

			this->TryToMoveChess(chess_id, GridIndex::InvalidGrid, grid_current);
		}
	}
	else
	{
		GridIndex grid;
		if (operational_delegate_->IsChessSelectedInBoard(&grid, &chess_id))
		{
			// CurrentGlobalPosOfChessFaceCenter will be called in TryToMoveChess frame 
			this->MoveChessFaceCenterToPos(LocalPosOfBoardGridCenter(grid));

			if (grid != grid_current && grid_current.IsValid())
			{
				this->TryToMoveChess(chess_id, grid, grid_current);
			}
		}
	}

	operational_delegate_->OnMouseReleased(grid_from, chess_id_dragging, grid_current, chess_id_current);
}

QPoint UIGoOperationalLayer::GlobalPosOfBoardGridCenter(const GridIndex& grid_index)
{
	auto iter = map_grid_global_rect_in_board_.find(grid_index);

	assert(iter != map_grid_global_rect_in_board_.cend());

	return iter->second.center();
}

QPoint UIGoOperationalLayer::GlobalPosOfBasketChessCenter(ChessId chess_id)
{
	auto iter = map_chess_global_rect_in_basket_.find(chess_id);

	assert(iter != map_chess_global_rect_in_basket_.cend());

	return iter->second.center();
}

bool UIGoOperationalLayer::GlobalPosInBoardGrid(const QPoint& global_pos, GridIndex* grid_index, QRect* grid_global_rect)
{
	for (const auto& pair : map_grid_global_rect_in_board_)
	{
		if (pair.second.contains(global_pos))
		{
			if (grid_index) { *grid_index = pair.first; }
			
			if (grid_global_rect) { *grid_global_rect = pair.second; }
		
			return true;
		}
	}

	return false;
}

bool UIGoOperationalLayer::GlobalPosInBasket(const QPoint& global_pos, ChessId* chess_id, QRect* grid_global_rect)
{
	for (const auto& pair : map_chess_global_rect_in_basket_)
	{
		if (pair.second.contains(global_pos))
		{
			if (chess_id) { *chess_id = pair.first; }
			
			if (grid_global_rect) { *grid_global_rect = pair.second; }
			
			return true;
		}
	}

	return false;
}

void UIGoOperationalLayer::LazyGetGridRects()
{
	map_grid_global_rect_in_board_ = operational_delegate_->GridsGlobalRectInBoard();

	map_chess_global_rect_in_basket_ = operational_delegate_->ChessesGlobalRectInBasket();
}

void UIGoOperationalLayer::LazyCreateChessUi()
{
	if (!chess_face_)
	{
		if (!presentation_layer_)
		{
			presentation_layer_ = this;
		}

		chess_face_ = new ChessFace(presentation_layer_);

		arrow_line_ = new UIArrowLine(presentation_layer_);
		arrow_line_->SetLineWidth(2);
		arrow_line_->SetLineStyle(Qt::PenStyle::DotLine);

		arrow_line_->setFixedSize(this->size());

		this->ZOrderUi();
	}
}

void UIGoOperationalLayer::ShowChessFaceInMovingState(ChessId chess_id)
{
	if (chess_face_)
	{
		auto pixmap = operational_delegate_->MovingPixmapOfChessId(chess_id);

		chess_face_->setPixmap(pixmap);
		chess_face_->setFixedSize(pixmap.size());

		chess_face_->show();
	}
}

void UIGoOperationalLayer::ShowChessFaceInSelectedState(ChessId chess_id)
{
	if (chess_face_)
	{
		auto pixmap = operational_delegate_->SelectedPixmapOfChessId(chess_id);

		chess_face_->setPixmap(pixmap);
		chess_face_->setFixedSize(pixmap.size());

		chess_face_->show();
	}
}

void UIGoOperationalLayer::MoveChessFaceCenterToPos(const QPoint& local_pos)
{
	if (chess_face_)
	{
		chess_face_->move(local_pos.x() - chess_face_->width() * 0.5f, local_pos.y() - chess_face_->height() * 0.5f);
	}
}

void UIGoOperationalLayer::MoveAnchorLines(const GridIndex& grid_in_board)
{
	if (!grid_in_board.IsValid())
	{
		this->HideAnchorLines();
		return;
	}

	if (!x_anchor_line_)
	{
		this->LazyCreateAnchorLines();
	}

	const auto itor = map_grid_global_rect_in_board_.find(grid_in_board);
	if (itor == map_grid_global_rect_in_board_.cend())
	{
		this->HideAnchorLines();
		return;
	}

	QPoint pos = this->mapFromGlobal(itor->second.center());

	if (x_anchor_line_)
	{
		x_anchor_line_->show();

		UiUtils::MoveWidgetMiddleTopTo(x_anchor_line_, QPoint(pos.x(), x_anchor_line_->y()));
	}

	if (y_anchor_line_)
	{
		y_anchor_line_->show();

		UiUtils::MoveWidgetLeftMiddleTo(y_anchor_line_, QPoint(y_anchor_line_->x(), pos.y()));
	}
}

void UIGoOperationalLayer::HideAnchorLines()
{
	if (x_anchor_line_)
	{
		x_anchor_line_->hide();
	}

	if (y_anchor_line_)
	{
		y_anchor_line_->hide();
	}
}

void UIGoOperationalLayer::LazyCreateAnchorLines()
{
	// ugly hard code here temporarily
	// TODO: params should be passed in from outside
	int width = 464;
	int height = 464;
	int thickness = 2;

	QPixmap x_pixmap(thickness, height);
	x_pixmap.fill(Qt::GlobalColor::green);

	QPixmap y_pixmap(width, thickness);
	y_pixmap.fill(Qt::GlobalColor::green);

	x_anchor_line_ = new WidgetWithPixmap(x_pixmap, this);
	y_anchor_line_ = new WidgetWithPixmap(y_pixmap, this);

	x_anchor_line_->setFixedSize(x_pixmap.size());
	y_anchor_line_->setFixedSize(y_pixmap.size());

	UiUtils::MoveWidgetMiddleTopTo(x_anchor_line_, QPoint(226, 24));
	UiUtils::MoveWidgetLeftMiddleTo(y_anchor_line_, QPoint(226, 25));

	x_anchor_line_->hide();
	y_anchor_line_->hide();

	this->ZOrderUi();
}

void UIGoOperationalLayer::ZOrderUi()
{
	if (x_anchor_line_)
	{
		x_anchor_line_->lower();
	}

	if (y_anchor_line_)
	{
		y_anchor_line_->lower();
	}

	if (chess_face_)
	{
		chess_face_->raise();
	}

	if (arrow_line_)
	{
		arrow_line_->raise();
	}
}

bool UIGoOperationalLayer::TryToMoveChess(ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to)
{
	return operational_delegate_->TryToMoveChessLocally(chess_id, grid_from, grid_to);
}

QPoint UIGoOperationalLayer::CurrentGlobalPosOfChessFaceCenter() const
{
	if (chess_face_)
	{
		return chess_face_->mapToGlobal(QPoint(chess_face_->width() * 0.5f, chess_face_->height() * 0.5f));
	}

	return this->mapToGlobal(QPoint(this->width() * 0.5f, this->height() * 0.5f));
}

GAME_NAMESPACE_END