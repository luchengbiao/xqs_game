#ifndef __GAME_VIEW_CHESS_UI_CHESS_GAME_BOARD_H__
#define __GAME_VIEW_CHESS_UI_CHESS_GAME_BOARD_H__
#include <QPointer>
#include "base/callback/callback.h"
#include "game/model/base/grid/grid_index.h"
#include "game/model/go/go_trait.h"
#include "game/view/base/ui_game_board_base.h"
#include "ui_go_piece.h"

GAME_NAMESPACE_BEGIN

class GoGameBoard;
class UIScaleLineLayer;

typedef std::vector<UIGoPieceBase*> GoPieces;

class UIGoGameBoard : public UIGameBoardBase
{
	Q_OBJECT

public:
	explicit UIGoGameBoard(int grid, QWidget* parent = nullptr);

	// override UIGameBoardBase
	virtual GameBoardType		BoardType() const override { return board_type_; }
	virtual	int					RowsOfBoard() const override { return grid_; }
	virtual	int					ColumnsOfBoard() const override { return grid_; }
	virtual void				SetRuleBased(bool rule_based) override;
	virtual int					GridWidth() const override { return std::round(InnerGridWidth()); }
	virtual QPoint				PositionByGridIndex(const GridIndex& grid_index) const override;
	virtual UIChessPieceBase*	ChessPieceAt(const GridIndex& grid) const override { return InnerChessPieceAt(grid.X(), grid.Y()); }
	virtual void				SetScaleLineVisible(bool visible);
	virtual void				HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr) override;

	GoGameBoard*				LocalGameBoard() const;

	// override UIAnimationDelegate
	virtual QPixmap				MovingPixmapOfChessId(ChessId) override;
	virtual	QPoint				GlobalPositionOfGridCenterInBoard(const GridIndex&) override;
	virtual	QPoint				GlobalPositionOfChessCenterInBasket(ChessId) override;
	virtual void				HideChessInBoardGrid(const GridIndex&) override;
	virtual int					MilliSecondsOfMovingAnimation() override;

	void						SetAllChessToNormalState();
	void						SetChessToSelectedState(const GridIndex&);
	void						UpdateSelectedChessByActionResult(const GameActionResult&);
	bool						HasChessSelected(GridIndex*, ChessId*);

protected:
	virtual void				Init() override;

	virtual void				InitModel();
	virtual void				InitUI();

	virtual void				OnOperationalLayerAssociated() override;
	virtual	void				OnLayoutChanged() override;

	void						CreateChesses();
	void						CreateScaleLine();

	UIGoPieceBase*				InnerChessPieceAt(int x, int y) const { return (x >= 0 && x < grid_ && y >= 0 && y < grid_) ? chesses_[ChessIndexByXY(x, y)] : nullptr; }

	template<typename Callable>
	void	ForeachChess(Callable&& callable)
	{
		for (auto chess : chesses_)
		{
			callable(chess);
		}
	}

	inline QPoint				GridOffsetFromOrigin() const { return GoTrait::GoBoardInfoByGrid(grid_).offset_; };

	void						UpdateChessesFromModel(const GameBoardPtr& model);
	void						UpdateChessesFromModelAndUndoVariation(const GameBoardPtr& model, const GameActionResult& result);

	virtual void				SyncLocalWithMaster() override;

	virtual void				OnMasterDone(const GameActionResult&) override;
	virtual void				OnLocalDone(const GameActionResult&) override;

	bool						NeedToPlayVariationImmediately(const GameActionResult&);

	void						PlayMasterVariationOnAnimationLayer(const GameActionResult&);
	void						PlayLocalVariationOnAnimationLayer(const GameActionResult&);
	void						MarkGridsCaptured(const GameActionResult&);

	GoGameBoard*				MasterGameBoard() const;

	inline int					ChessIndexByXY(int x, int y) const { return (y * grid_ + x); }
	inline float				InnerGridWidth() const { return GoTrait::GoBoardInfoByGrid(grid_).grid_width_; }
	inline int					InnerChessWidth() const { return GoTrait::GoBoardInfoByGrid(grid_).chess_width_; }

private:
	const int					grid_{ 0 };
	const GameBoardType			board_type_{ GameBoardType::None };

	GoPieces					chesses_;

	UIScaleLineLayer*			scale_line_layer_{ nullptr };

	nbase::WeakCallbackFlag		chess_move_callback_flag_;
};

GAME_NAMESPACE_END

#endif