#pragma once

namespace sts
{
	enum class PlayerAction
	{
		NONE,
		ROCK,
		PAPER,
		SCISSORS,
		HAND,
		STUMP,
	};


	inline std::string ActionToString(sts::PlayerAction action)
	{
		switch (action) 
		{
		case PlayerAction::NONE:
			return "NONE";
		case PlayerAction::ROCK:
			return "ROCK";
		case PlayerAction::PAPER:
			return "PAPER";
		case PlayerAction::SCISSORS:
			return "SCISSORS";
		case PlayerAction::HAND:
			return "HAND";
		case PlayerAction::STUMP:
			return "STUMP";
		default:
			return "No action with that name";
		}
	}

}
