#ifndef __GAME_VIEW_BASE_CHESS_PIECE_UI_CHESS_PIECE_MARK_H__
#define __GAME_VIEW_BASE_CHESS_PIECE_UI_CHESS_PIECE_MARK_H__
#include <type_traits>
#include <QWidget>
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

enum class MarkShape
{
	None,
	Circle,
	Square,
	Triangle,
};

template<typename WidgetT>
class ChessPieceMark
{
public:
	ChessPieceMark()
	{
		// ensure that QWidget is base of WidgetT
		static_assert(std::is_base_of<QWidget, WidgetT>::value, "QWidget should be base of WidgetT");

		// ensure the safety of the base-derived conversion: static_cast<WidgetT*>(this)
		static_assert(std::is_base_of<ChessPieceMark<WidgetT>, WidgetT>::value, "ChessPieceMark<WidgetT> should also be base of WidgetT");
	}

	// to enable dynamic_cast<WidgetT*>(this): 
	// base-derived dynamic_cast needs base class has virtual function (at least one virtual function in base class)
	virtual ~ChessPieceMark() = default;
	
	void Mark(const MarkShape shape)
	{
		//auto widget = static_cast<WidgetT*>(this); // sub base class to derived
		auto widget = dynamic_cast<WidgetT*>(this); // absolutely safe, trade-off on a little performance
		
		//TODO: shape on widget
		if (widget)
		{
		}
	}

protected:
	MarkShape shape_{ MarkShape::None };
};

GAME_NAMESPACE_END

#endif