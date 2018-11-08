#include "ui_chess_piece_in_basket.h"
#include "common/uiutils/ui_utils.h"
#include "game/view/widget_with_pixmap/widget_with_pixmap.h"

GAME_NAMESPACE_BEGIN

void UIChessPieceInBasket::ShowDisabledFlag()
{
	if (!disabled_flag_)
	{
		disabled_flag_ = new WidgetWithPixmap(this);

		QPixmap pxp_disabled(":/common/images/common/disabled_flag.png");
		if (!pxp_disabled.isNull())
		{
			disabled_flag_->setPixmap(pxp_disabled);
			disabled_flag_->setFixedSize(pxp_disabled.size());
		}

		UiUtils::MoveWidgetRightBottomTo(disabled_flag_, QPoint(width(), height()) + QPoint(5, 5));
	}

	disabled_flag_->show();
}

void UIChessPieceInBasket::HideDisabledFlag()
{
	if (disabled_flag_)
	{
		disabled_flag_->hide();
	}
}

GAME_NAMESPACE_END