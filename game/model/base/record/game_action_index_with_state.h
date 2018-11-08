#ifndef __GAME_MODEL_BASE_RECORD_GAME_ACTION_INDEX_WITH_STATE_H__
#define __GAME_MODEL_BASE_RECORD_GAME_ACTION_INDEX_WITH_STATE_H__
#include <algorithm>
#include <deque>
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

enum class GameActionState
{
	Done,
	Undone,
	Redone,
};

// Index to a GameAction container
class GameActionIndexWithState
{
public:
	GameActionIndexWithState(int index) : index_(index) {}

	inline int				Index() const { return index_; }

	inline GameActionState	State() const { return state_; }
	inline void				SetState(const GameActionState state) { state_ = state; }

private:
	int						index_;
	GameActionState			state_{ GameActionState::Done };
};

class GameActionIndices : public std::deque<GameActionIndexWithState>
{
public:
	GameActionIndices() = default;

	void RemoveItemsByIndex(const int index)
	{
		erase(std::remove_if(begin(), end(), [=](const GameActionIndexWithState& item){ return item.Index() == index; }), end());
	}

	void RemoveItemsByState(const GameActionState state)
	{
		erase(std::remove_if(begin(), end(), [=](const GameActionIndexWithState& item){ return item.State() == state; }), end());
	}
};

GAME_NAMESPACE_END

#endif