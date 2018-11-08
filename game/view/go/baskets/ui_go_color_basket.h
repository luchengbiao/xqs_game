#ifndef __GAME_VIEW_GO_UI_GO_COLOR_BASKET_H__
#define __GAME_VIEW_GO_UI_GO_COLOR_BASKET_H__
#include <QWidget>
#include "game/model/base/chess/chess_info.h"
#include "game/model/go/go_trait.h"

class WidgetWithPixmap;

GAME_NAMESPACE_BEGIN

class UIGoPieceBaseInBasket;

class UIGoColorBasket : public QWidget
{
	Q_OBJECT

public:
	explicit UIGoColorBasket(GoTrait::Color color, QWidget* parent = nullptr);

	inline static QPoint	PosOfChessArea() { return QPoint(12, 0); }
	inline static QSize		SizeOfChessArea() { return QSize(170, 170); }
	inline static int		YOffsetOfBasket() { return 37; }

	QRect					ChessGlobalArea() const;
	QPoint					GlobalPosOfChessAreaCenter() const;

	void					ShowDisabledFlag();
	void					HideDisabledFlag();

	void					ShowFixedChess(const int grid);
	void					HideFixedChess();

private:
	void					Init();

	void					LazyCreateGoPieces();
	UIGoPieceBaseInBasket*	CreateGoPiece(const int grid);

private:
	const GoTrait::Color	color_;

	WidgetWithPixmap*		basket_{ nullptr };
	WidgetWithPixmap*		disabled_flag_{ nullptr };

	UIGoPieceBaseInBasket*	go_piece_9_{ nullptr };
	UIGoPieceBaseInBasket*	go_piece_13_{ nullptr };
	UIGoPieceBaseInBasket*	go_piece_19_{ nullptr };
};

GAME_NAMESPACE_END

#endif