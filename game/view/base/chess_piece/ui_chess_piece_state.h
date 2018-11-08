#ifndef __GAME_VIEW_BASE_CHESS_PIECE_UI_CHESS_PIECE_STATE_H__
#define __GAME_VIEW_BASE_CHESS_PIECE_UI_CHESS_PIECE_STATE_H__
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

class ChessPieceState
{
public:
	enum class State
	{
		Normal,
		Moving,
		Selected,
	};

public:
	ChessPieceState() = default;
	virtual ~ChessPieceState() = default;

	inline State	GetState() const { return state_; }
	void			SetState(const State state)
	{
		if (state_ != state)
		{
			auto old = state_;
			state_ = state;

			this->OnStateChanged(old, state_);
		}
	}

	inline void		SetToNormalState() { this->SetState(State::Normal); }
	inline void		SetToMovingState() { this->SetState(State::Moving); }
	inline void		SetToSelectedState() { this->SetState(State::Selected); }

	inline bool		IsNormalState() const { return state_ == State::Normal; }
	inline bool		IsMovingState() const { return state_ == State::Moving; }
	inline bool		IsSelectedState() const { return state_ == State::Selected; }

protected:
	virtual	void	OnStateChanged(const State old_state, const State new_state) {}

private:
	State			state_{ State::Normal };
};

GAME_NAMESPACE_END

#endif