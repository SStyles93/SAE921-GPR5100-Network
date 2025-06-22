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
}

