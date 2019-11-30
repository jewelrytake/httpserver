#pragma once
#include <stdint.h>

namespace Network
{
	enum class PacketType : uint16_t
	{
		PT_Invalid,
		PT_IntegerArray,
		PT_ChatMessage,
		PT_Test
	};
}