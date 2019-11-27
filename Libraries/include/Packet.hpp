#pragma once
#include "PacketType.h"
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <winsock.h>
#include <string>

namespace Network
{
	struct Packet
	{
		Packet(PacketType packetType = PacketType::PT_Invalid);
		PacketType GetPacketType();

		Packet& operator << (uint32_t data); //insertion operator
		Packet& operator >> (uint32_t& data); //extraction operator
		Packet& operator << (const std::vector< uint8_t >& data); //insertion operator
		Packet& operator >> (std::vector< uint8_t >& data); //extraction operator
		uint32_t m_extractionOffset = 0; //where to read
		std::vector<uint8_t> m_buffer;
	private:
		void AssignPacketType(PacketType packetType);
		void Clear();
		void Append(const void* data, uint32_t size);
	};
}