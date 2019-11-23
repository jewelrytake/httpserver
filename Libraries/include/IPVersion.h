#pragma once
#include <cstdint>

namespace Network
{
	enum class IPVersion : std::uint16_t
	{
		Unknown, IPv4, IPv6,
	};
}