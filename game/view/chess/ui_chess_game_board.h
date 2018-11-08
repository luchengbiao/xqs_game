#ifndef __GAME_VIEW_CHESS_UI_CHESS_GAME_BOARD_H__
#define __GAME_VIEW_CHESS_UI_CHESS_GAME_BOARD_H__
#include <QPointer>
#include "base/callback/callback.h"
#include "game/model/base/grid/grid_index.h"
#include "game/view/base/ui_game_board_base.h"
#include "ui_chess_piece.h"

GAME_NAMESPACE_BEGIN

class ChessGameBoard;
class UIScaleLineLayer;

class UIChessGameBoard : public UIGameBoardBase
{
	Q_OBJECT

public:
	static const int Grid{ 8 };

public:
	explicit UIChessGameBoard(QWidget* parent = nullptr);

	// override UIGameBoardBase
	virtual GameBoardType		BoardType() const override { return GameBoardType::Chess; }
	virtual	int					RowsOfBoard() const override { return Grid; }
	virtual	int					ColumnsOfBoard() const override { return Grid; }
	virtual void				SetRuleBased(bool rule_based) override;
	virtual int					GridWidth() const override { return 58; }
	virtual QPoint				PositionByGridIndex(const GridIndex& grid_index) const override;
	virtual UIChessPieceBase*	ChessPieceAt(const GridIndex& grid) const override { return InnerChessPieceAt(grid.X(), grid.Y()); }
	virtual void				SetScaleLineVisible(bool visible);
	virtual void				HandleMasterCommandSpecially(const CommandSharedPtr& cmd_ptr) override;

	ChessGameBoard*				LocalGameBoard() const;

	// override UIAnimationDelegate
	virtual QPixmap				MovingPixmapOfChessId(ChessId) override;
	virtual	QPoint				GlobalPositionOfGridCenterInBoard(const GridIndex&) override;
	virtual	QPoint				GlobalPositionOfChessCenterInBasket(ChessId) override;
	virtual void				HideChessInBoardGrid(const GridIndex&) override;
	virtual int					MilliSecondsOfMovingAnimation() override;

protected:
	virtual void				Init() override;

	virtual void				InitModel();
	virtual void				InitUI();

	virtual void				OnOperationalLayerAssociated() override;
	virtual	void				OnLayoutChanged() override;

	void						CreateChesses();
	void						CreateScaleLine();

	UIChessPiece*				InnerChessPieceAt(int x, int y) const { return (x >= 0 && x < Grid && y >= 0 && y < Grid) ? chesses_[x][y] : nullptr; }

	template<typename Callable>
	void	ForeachChess(Callable&& callable)
	{
		for (int x = 0; x < Grid; ++x)
		{
			for (int y = 0; y < Grid; ++y)
			{
				callable(chesses_[x][y]);
			}
		}
	}

	inline QPoint				GridOffsetFromOrigin() const { return QPoint(26, 24); };

	void						UpdateChessesFromModel(const GameBoardPtr& model);
	void						UpdateChessesFromModelAndUndoVariation(const GameBoardPtr& model, const GameActionResult& result);

	virtual void				SyncLocalWithMaster() override;

	virtual void				OnMasterDone(const GameActionResult&) override;
	virtual void				OnLocalDone(const GameActionResult&) override;

	bool						NeedToPlayVariationImmediately(const GameActionResult&);

	void						PlayMasterVariationOnAnimationLayer(const GameActionResult&);
	void						PlayLocalVariationOnAnimationLayer(const GameActionResult&);

	ChessGameBoard*				MasterGameBoard() const;

private:
	UIChessPiece*				chesses_[Grid][Grid]; // 8 * 8

	UIScaleLineLayer*			scale_line_layer_{ nullptr };

	nbase::WeakCallbackFlag		chess_move_callback_flag_;
};

GAME_NAMESPACE_END

#endif