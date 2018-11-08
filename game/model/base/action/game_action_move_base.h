#ifndef __GAME_MODEL_BASE_ACTION_GAME_ACTION_MOVE_BASE_H__
#define __GAME_MODEL_BASE_ACTION_GAME_ACTION_MOVE_BASE_H__
#include <assert.h>
#include <type_traits>
#include "game_action_with_result.h"
#include "game/model/base/chess/chess_info.h"
#include "game/model/base/grid/grid_index.h"
#include "protocol/command/base/command.h"

GAME_NAMESPACE_BEGIN

template < typename Base, typename = std::enable_if_t<std::is_base_of<GameActionWithResult, Base>::value>>
class GameActionMoveBase : public Base
{
public:
	GameActionMoveBase() = default;

	ChessId ChessId() const
	{
		assert(command_ptr_);

		return command_ptr_->IntOfVariantAt(0);
	}

	GridIndex GridFrom() const
	{
		assert(command_ptr_);

		return GridIndex(command_ptr_->IntOfVariantAt(1), command_ptr_->IntOfVariantAt(2));
	}

	GridIndex GridTo() const
	{
		assert(command_ptr_);

		return GridIndex(command_ptr_->IntOfVariantAt(3), command_ptr_->IntOfVariantAt(4));
	}
};

GAME_NAMESPACE_END

#endif