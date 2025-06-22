#pragma once

namespace sts
{
	enum class ServerState
	{
		WAITING_FOR_PLAYERS,
		PLAYERS_MOVE,
		RESOLVING,
		END
	};
	inline std::string StateToString(sts::ServerState state)
	{
		switch (state)
		{
		case sts::ServerState::WAITING_FOR_PLAYERS:
			return "WAITING_FOR_PLAYERS";
		case sts::ServerState::PLAYERS_MOVE:
			return "PLAYERS_MOVE";
		case sts::ServerState::RESOLVING:
			return "RESOLVING";
		case sts::ServerState::END:
			return "END";
		}
	}
}

