#ifndef __GAME_MODEL_BASE_NOTATION_NOTATION_H__
#define __GAME_MODEL_BASE_NOTATION_NOTATION_H__
#include <string>
#include <vector>
#include "game/macros.h"

GAME_NAMESPACE_BEGIN

class Notation
{
public:
	Notation(const std::string& move_notation, const std::string& board_notation)
		: move_notation_(move_notation), board_notation_(board_notation)
	{}

	std::string		MoveNotation() const { return move_notation_; }
	std::string		BoardNotation() const { return board_notation_; }

private:
	std::string		move_notation_;
	std::string		board_notation_;
};

class Notations : public std::vector<Notation>
{
public:
	Notations(const std::string& game_name = "") : game_name_(game_name) {}

	void			SetGameName(const std::string& game_name) { game_name_ = game_name; }
	std::string		GameName() const { return game_name_; }

private:
	std::string		game_name_;
};

GAME_NAMESPACE_END

#endif