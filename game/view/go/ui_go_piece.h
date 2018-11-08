#ifndef __GAME_VIEW_GO_GO_PIECE_H__
#define __GAME_VIEW_GO_GO_PIECE_H__
#include "game/model/go/go_trait.h"
#include "game/view/base/chess_piece/ui_chess_piece_grid.h"
#include "game/view/base/chess_piece/ui_chess_piece_mark.h"
#include "game/view/base/chess_piece/ui_chess_piece_state.h"
#include "game/view/base/chess_piece/ui_chess_piece_with_mixins.h"
#include "game/view/base/chess_piece/ui_chess_piece_with_trait.h"

GAME_NAMESPACE_BEGIN

class UIGoPieceBase : public UIChessPieceWithMixins<UIChessPieceBase, ChessPieceGrid, ChessPieceState, ChessPieceMark<UIGoPieceBase>>
{
public:
	UIGoPieceBase(QWidget* parent = nullptr) : UIChessPieceWithMixins(parent) {}

	static UIGoPieceBase* Create(const int grid, QWidget* parent);

	virtual void Highlight() { /*do nothing*/ }

	virtual void OnStateChanged(const State old_state, const State new_state) override
	{
		this->UpdateChess(UIChessPieceBase::UpdateReason::Forcely);
	}
};

template<int X>
class UIGoPiece : public UIChessPieceWithTrait<GoTraitX<X>, UIGoPieceBase>
{
	typedef GoTraitX<X> MyTrait;

public:
	explicit UIGoPiece(QWidget* parent = nullptr) : UIChessPieceWithTrait(parent) {}

	virtual std::string ImagePath() const override
	{
		switch (GetState())
		{
		case ChessPieceState::State::Moving:
			return MyTrait::MovingImagePathById(GetChessId());

		case ChessPieceState::State::Selected:
			return MyTrait::SelectedImagePathById(GetChessId());

		default:
			return MyTrait::NormalImagePathById(GetChessId());
		}
	}
};

#define UI_GO_PIECE(X) \
class UIGoPiece##X : public UIGoPiece<X> \
{ \
	Q_OBJECT \
public: \
	explicit UIGoPiece##X(QWidget* parent = nullptr) : UIGoPiece<X>(parent) {} \
};

UI_GO_PIECE(9);
UI_GO_PIECE(13);
UI_GO_PIECE(19);

#undef UI_GO_PIECE

inline UIGoPieceBase* UIGoPieceBase::Create(const int grid, QWidget* parent)
{
	UIGoPieceBase* go_piece = nullptr;

	if (grid == 9) { go_piece = new UIGoPiece9(parent); }
	else if (grid == 13) { go_piece = new UIGoPiece13(parent); }
	else if (grid == 19) { go_piece = new UIGoPiece19(parent); }

	return go_piece;
}

GAME_NAMESPACE_END

#endif