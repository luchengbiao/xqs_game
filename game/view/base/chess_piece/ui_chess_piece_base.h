#ifndef __GAME_VIEW_BASE_CHESS_PIECE_UI_CHESS_PIECE_BASE_H__
#define __GAME_VIEW_BASE_CHESS_PIECE_UI_CHESS_PIECE_BASE_H__
#include <QWidget>
#include "game/model/base/chess/chess_info.h"
#include "game/view/base/ui_chess_face_fwd.h"

class QPixmap;

GAME_NAMESPACE_BEGIN

class UIChessPieceBase : public QWidget
{
	Q_OBJECT

public:
	enum class UpdateReason
	{
		NoReason,
		ChessIdChanged,
		Forcely,
	};

public:
	explicit UIChessPieceBase(QWidget* parent = nullptr);

	void				SetChessId(const ChessInfo& chess_info);
	void				SetChessId(ChessId chess_id);
	ChessId				GetChessId() const { return chess_info_.GetId(); }
	bool				ChessIdIsEmpty() const { return chess_info_.IsEmpty(); }
	bool				ChessIdIsNotEmpty() const { return chess_info_.IsNotEmpty(); }

	void				HideFace();
	void				ShowFace();

	const QPixmap*		ChessPixmap() const;

	void				UpdateChess(UpdateReason reason);

	virtual std::string ImagePath() const = 0;

	virtual void		Highlight();
	virtual void		Unhighlight();

protected:
	ChessInfo			chess_info_;
	ChessFace*			face_{ nullptr };
};

GAME_NAMESPACE_END

#endif