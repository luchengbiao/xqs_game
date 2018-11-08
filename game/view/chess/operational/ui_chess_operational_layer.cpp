#include "ui_chess_operational_layer.h"
#include <assert.h>
#include <QMouseEvent>
#include "game/view/arrow_line/ui_arrow_line.h"
#include "game/view/base/ui_chess_face_impl.h"
#include "ui_chess_path_marker_layer.h"

GAME_NAMESPACE_BEGIN

struct UIChessOperationalLayer::DragInfo
{
	ChessId			chess_id_being_dragged_{ ChessInfo::EmptyId };
	GridIndex		grid_from_;
	GridIndex		grid_current_;
	QRect			current_grid_global_rect_;

	ChessId			chess_id_selected_{ ChessInfo::EmptyId };
	GridIndex		grid_selected_; // grid index which is selected in last mouse-release event

	bool			mouse_is_moved_{ false };

	inline void		SetChessIdBeingDragged(ChessId chess_id) { chess_id_being_dragged_ = chess_id; }
	inline void		SetGridFrom(const GridIndex& grid_index) { grid_from_ = grid_index; }
	inline void		SetGridCurrent(const GridIndex& grid_index) { grid_current_ = grid_index; }
	inline void		SetCurrentGridGlobalRect(const QRect& rect) { current_grid_global_rect_ = rect; }

	inline void		SetChessIdSelected(ChessId chess_id) { chess_id_selected_ = chess_id; }
	inline void		SetGridSelected(const GridIndex& grid_index) { grid_selected_ = grid_index; }

	inline void		SetMouseIsMoved(bool mouse_is_moved) { mouse_is_moved_ = mouse_is_moved; }

	inline ChessId	ChessIdBeingDragged() const { return chess_id_being_dragged_; }
	inline const GridIndex& GridFrom() const { return grid_from_; }
	inline const GridIndex& GridCurrent() const { return grid_current_; }
	inline const QRect CurrentGridGlobalRect() const { return current_grid_global_rect_; }

	inline ChessId ChessIdSelected() const { return chess_id_selected_; }
	inline const GridIndex& GridSelected() const { return grid_selected_; }

	inline bool		MouseIsMoved() const { return mouse_is_moved_; }

	inline bool		HasChessIdBeingDragged() const { return ChessIdIsNotEmpty(chess_id_being_dragged_); }
	inline bool		HasChessIdSelected() const { return ChessIdIsNotEmpty(chess_id_selected_)/* && grid_selected_.IsValid()*/; }

	inline void		InvalidateChessSelected() { chess_id_selected_ = ChessInfo::EmptyId; grid_selected_ = GridIndex::InvalidGrid; }
};

UIChessOperationalLayer::UIChessOperationalLayer(QWidget* parent, QWidget* presentation_layer)
: UIOperationalLayer(parent)
, presentation_layer_(presentation_layer)
, drag_info_(new DragInfo)
{}

UIChessOperationalLayer::~UIChessOperationalLayer()
{
	delete drag_info_;
}

void UIChessOperationalLayer::SetOperationalDelegate(MyOperationalDelegate* operational_delegate)
{
	operational_delegate_ = operational_delegate;
}

void UIChessOperationalLayer::Interrupt()
{
	this->SetInterrupted(true);

	this->InnerClear();
}

void UIChessOperationalLayer::Clear()
{
	this->InnerClear();
}

void UIChessOperationalLayer::InnerClear()
{
	if (drag_info_->HasChessIdSelected())
	{
		operational_delegate_->ShowChessInBoardGrid(drag_info_->GridSelected());
		drag_info_->InvalidateChessSelected();
	}

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

	this->HidePathMarker();
}

void UIChessOperationalLayer::mousePressEvent(QMouseEvent* mouse_event)
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

	ChessId chess_id;
	QRect grid_global_rect;

	if (GlobalPosInBasket(global_pos, &chess_id, &grid_global_rect))
	{
		drag_info_->SetChessIdBeingDragged(chess_id);
		drag_info_->SetGridFrom(GridIndex::InvalidGrid);
	}
	else
	{
		GridIndex grid_index;
		if (GlobalPosInBoardGridWithValidChess(global_pos, &chess_id, &grid_index, &grid_global_rect))
		{
			drag_info_->SetChessIdBeingDragged(chess_id);
			drag_info_->SetGridFrom(grid_index);
		}
	}

	if (!drag_info_->HasChessIdBeingDragged()) { return; }

	const auto grid_from = drag_info_->GridFrom();
	drag_info_->SetCurrentGridGlobalRect(grid_global_rect);
	drag_info_->SetGridCurrent(grid_from);

	if (drag_info_->HasChessIdSelected()) // if has selected a chess in last mouse press-move-release event loop
	{
		if (drag_info_->GridSelected() != grid_from)
		{
			drag_info_->SetChessIdBeingDragged(ChessInfo::EmptyId);

			return;
		}
	}

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

		auto grids = operational_delegate_->GridsReachableFrom(grid_from);
		this->ShowPathMarker(grids);
	}
}

void UIChessOperationalLayer::mouseMoveEvent(QMouseEvent* mouse_event)
{
	if (IsInterrupted() || !drag_info_->HasChessIdBeingDragged()) { return; }

	if (!drag_info_->MouseIsMoved()) // change from SelectedFace to MovingFace when first moved
	{
		drag_info_->SetMouseIsMoved(true);

		this->ShowChessFaceInMovingState(drag_info_->ChessIdBeingDragged());

		operational_delegate_->OnMouseWillMove(drag_info_->GridFrom(), drag_info_->ChessIdBeingDragged());
	}

	this->MoveChessFaceCenterToPos(mouse_event->pos());

	auto global_pos = mouse_event->globalPos();

	if (arrow_line_)
	{
		arrow_line_->SetEndPosition(global_pos);
	}
	
	GridIndex grid_current = drag_info_->GridCurrent();

	if (grid_current.IsValid())
	{
		if (!drag_info_->CurrentGridGlobalRect().contains(global_pos))
		{
			operational_delegate_->UnhighlightGridInBoard(grid_current);

			QRect grid_global_rect;
			if (GlobalPosInBoardGrid(global_pos, &grid_current, &grid_global_rect))
			{
				operational_delegate_->HighlightGridInBoard(grid_current);

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
			operational_delegate_->HighlightGridInBoard(grid_current);

			drag_info_->SetGridCurrent(grid_current);
			drag_info_->SetCurrentGridGlobalRect(grid_global_rect);
		}
		else
		{
			drag_info_->SetCurrentGridGlobalRect(QRect(global_pos, QSize(0, 0)));
		}
	}
}

void UIChessOperationalLayer::mouseReleaseEvent(QMouseEvent* mouse_event)
{
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

void UIChessOperationalLayer::mouseReleaseEvent_ChessIdBeingDragged(QMouseEvent* mouse_event)
{
	if (drag_info_->GridCurrent().IsValid())
	{
		operational_delegate_->UnhighlightGridInBoard(drag_info_->GridCurrent());
	}

	if (arrow_line_)
	{
		arrow_line_->hide();
	}

	if (drag_info_->GridFrom() != drag_info_->GridCurrent())
	{
		this->TryToMoveChess(drag_info_->ChessIdBeingDragged(), drag_info_->GridFrom(), drag_info_->GridCurrent());
	}
	else
	{
		if (drag_info_->HasChessIdSelected()) // if a grid has already been selected, then un-select it
		{
			operational_delegate_->ShowChessInBoardGrid(drag_info_->GridFrom());
			this->HidePathMarker();
			if (chess_face_) { chess_face_->hide(); }

			drag_info_->InvalidateChessSelected();
		}
		else
		{
			drag_info_->SetChessIdSelected(drag_info_->ChessIdBeingDragged());
			drag_info_->SetGridSelected(drag_info_->GridCurrent());

			if (drag_info_->MouseIsMoved()) // change from MovingFace back to SelectedFace
			{
				this->ShowChessFaceInSelectedState(drag_info_->ChessIdSelected());
			}
			
			if (drag_info_->GridFrom().IsValid())
			{
				this->MoveChessFaceCenterToPos(LocalPosOfBoardGridCenter(drag_info_->GridSelected()));
			}
			else
			{
				this->MoveChessFaceCenterToPos(LocalPosOfBasketChessCenter(drag_info_->ChessIdSelected()));
			}
		}
	}
}

void UIChessOperationalLayer::mouseReleaseEvent_NoChessIdBeingDragged(QMouseEvent* mouse_event)
{
	if (drag_info_->HasChessIdSelected())
	{
		GridIndex grid_current;
		QRect grid_global_rect;

		if (!GlobalPosInBoardGrid(mouse_event->globalPos(), &grid_current, &grid_global_rect))
		{
			grid_current = GridIndex::InvalidGrid;
		}

		auto chess_id_selected = drag_info_->ChessIdSelected();
		auto grid_selected = drag_info_->GridSelected();

		this->InnerClear();

		if (grid_selected != grid_current && grid_current.IsValid())
		{
			this->TryToMoveChess(chess_id_selected, grid_selected, grid_current);
		}
	}
}

QPoint UIChessOperationalLayer::GlobalPosOfBoardGridCenter(const GridIndex& grid_index)
{
	auto iter = map_grid_global_rect_in_board_.find(grid_index);
	
	assert(iter != map_grid_global_rect_in_board_.cend());

	return iter->second.center();
}

QPoint UIChessOperationalLayer::GlobalPosOfBasketChessCenter(ChessId chess_id)
{
	auto iter = map_chess_global_rect_in_basket_.find(chess_id);

	assert(iter != map_chess_global_rect_in_basket_.cend());

	return iter->second.center();
}

bool UIChessOperationalLayer::GlobalPosInBoardGrid(const QPoint& global_pos, GridIndex* grid_index, QRect* grid_global_rect)
{
	for (const auto& pair : map_grid_global_rect_in_board_)
	{
		if (pair.second.contains(global_pos))
		{
			*grid_index = pair.first;
			*grid_global_rect = pair.second;

			return true;
		}
	}
	
	return false;
}

bool UIChessOperationalLayer::GlobalPosInBoardGridWithValidChess(const QPoint& global_pos, ChessId* chess_id, GridIndex* grid_index, QRect* grid_global_rect)
{
	if (GlobalPosInBoardGrid(global_pos, grid_index, grid_global_rect))
	{
		if (operational_delegate_->GridContainsChessInBoard(*grid_index, chess_id))
		{
			return true;
		}
	}

	return false;
}

bool UIChessOperationalLayer::GlobalPosInBasket(const QPoint& global_pos, ChessId* chess_id, QRect* grid_global_rect)
{
	for (const auto& pair : map_chess_global_rect_in_basket_)
	{
		if (pair.second.contains(global_pos))
		{
			*chess_id = pair.first;
			*grid_global_rect = pair.second;

			return true;
		}
	}

	return false;
}

void UIChessOperationalLayer::LazyGetGridRects()
{
	//if (map_grid_global_rect_in_board_.empty())
	{
		map_grid_global_rect_in_board_ = operational_delegate_->GridsGlobalRectInBoard();
	}

	//if (map_chess_global_rect_in_basket_.empty())
	{
		map_chess_global_rect_in_basket_ = operational_delegate_->ChessesGlobalRectInBasket();
	}
}

void UIChessOperationalLayer::LazyCreateChessUi()
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

void UIChessOperationalLayer::ShowChessFaceInMovingState(ChessId chess_id)
{
	if (chess_face_)
	{
		auto pixmap = operational_delegate_->MovingPixmapOfChessId(chess_id);

		chess_face_->setPixmap(pixmap);
		chess_face_->setFixedSize(pixmap.size());

		chess_face_->show();
	}
}

void UIChessOperationalLayer::ShowChessFaceInSelectedState(ChessId chess_id)
{
	if (chess_face_)
	{
		auto pixmap = operational_delegate_->SelectedPixmapOfChessId(chess_id);

		chess_face_->setPixmap(pixmap);
		chess_face_->setFixedSize(pixmap.size());

		chess_face_->show();
	}
}

void UIChessOperationalLayer::MoveChessFaceCenterToPos(const QPoint& local_pos)
{
	if (chess_face_)
	{
		chess_face_->move(local_pos.x() - chess_face_->width() * 0.5f, local_pos.y() - chess_face_->height() * 0.5f);
	}
}

void UIChessOperationalLayer::ShowPathMarker(const std::map<GridIndex, bool>& vec_grid_in_board)
{
	if (!path_marker_layer_)
	{
		path_marker_layer_ = new UIChessPathMarkerLayer(this);

		path_marker_layer_->setFixedSize(this->size());

		this->ZOrderUi();
	}

	ChessId chess_id;

	if (vec_grid_in_board.empty())
	{
		//TODO: tell why no legal grids with rule checking

		path_marker_layer_->SetMarkerPoints(Point2ColorMap());
	}
	else
	{
		Point2ColorMap map_point_color;
		for (const auto& pair : vec_grid_in_board)
		{
			if (pair.second || operational_delegate_->GridContainsChessInBoard(pair.first, &chess_id))
			{
				map_point_color.emplace(GlobalPosOfBoardGridCenter(pair.first), Qt::GlobalColor::yellow);
			}
			else
			{
				map_point_color.emplace(GlobalPosOfBoardGridCenter(pair.first), Qt::GlobalColor::green);
			}
		}

		path_marker_layer_->SetMarkerPoints(map_point_color);
	}

	path_marker_layer_->show();
}

void UIChessOperationalLayer::ShowPathMarker()
{
	if (path_marker_layer_)
	{
		path_marker_layer_->show();
	}
}

void UIChessOperationalLayer::HidePathMarker()
{
	if (path_marker_layer_)
	{
		path_marker_layer_->hide();
	}
}

void UIChessOperationalLayer::ZOrderUi()
{
	if (path_marker_layer_)
	{
		path_marker_layer_->lower();
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

bool UIChessOperationalLayer::TryToMoveChess(ChessId chess_id, const GridIndex& grid_from, const GridIndex& grid_to)
{
	return operational_delegate_->TryToMoveChessLocally(chess_id, grid_from, grid_to);
}

QPoint UIChessOperationalLayer::CurrentGlobalPosOfChessFaceCenter() const
{
	if (chess_face_)
	{
		return chess_face_->mapToGlobal(QPoint(chess_face_->width() * 0.5f, chess_face_->height() * 0.5f));
	}

	return this->mapToGlobal(QPoint(this->width() * 0.5f, this->height() * 0.5f));
}

GAME_NAMESPACE_END