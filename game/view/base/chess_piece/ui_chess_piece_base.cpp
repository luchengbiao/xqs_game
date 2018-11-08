#include "ui_chess_piece_base.h"
#include "game/view/base/ui_chess_face_impl.h"

GAME_NAMESPACE_BEGIN

UIChessPieceBase::UIChessPieceBase(QWidget* parent)
: QWidget(parent)
, face_(new ChessFace(this))
{
	setAutoFillBackground(true);

	QPalette palette;
	palette.setBrush(backgroundRole(), Qt::BrushStyle::NoBrush);
	setPalette(palette);
}

void UIChessPieceBase::SetChessId(const ChessInfo& chess_info)
{
	this->SetChessId(chess_info.GetId());
}

void UIChessPieceBase::SetChessId(ChessId chess_id)
{
	if (chess_info_.GetId() != chess_id)
	{
		chess_info_.SetId(chess_id);

		this->UpdateChess(UpdateReason::ChessIdChanged);
	}
	else
	{
		this->UpdateChess(UpdateReason::NoReason);
	}
}

void UIChessPieceBase::HideFace()
{
	face_->hide();
}

void UIChessPieceBase::ShowFace()
{
	face_->show();
}

const QPixmap* UIChessPieceBase::ChessPixmap() const
{
	return face_ ? face_->pixmap() : nullptr;
}

void UIChessPieceBase::UpdateChess(UpdateReason reason)
{
	if (ChessIdIsEmpty())
	{
		face_->setPixmap(QPixmap());

		face_->hide();

		return;
	}

	face_->show();

	if (UpdateReason::ChessIdChanged == reason || UpdateReason::Forcely == reason)
	{
		QPixmap pixmap(QString::fromStdString(ImagePath()));
		face_->setPixmap(pixmap);
		face_->setFixedSize(pixmap.size());
		face_->move((width() - face_->width()) * 0.5f, (height() - face_->height()) * 0.5f);
	}
}

void UIChessPieceBase::Highlight()
{
	QPalette palette;
	palette.setBrush(backgroundRole(), Qt::GlobalColor::red);
	setPalette(palette);
}

void UIChessPieceBase::Unhighlight()
{
	QPalette palette;
	palette.setBrush(backgroundRole(), Qt::BrushStyle::NoBrush);
	setPalette(palette);
}

GAME_NAMESPACE_END