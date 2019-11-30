#pragma once
#include "PacketType.hpp"
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <WinSock2.h>
#include <string>

namespace Network
{
	struct Packet
	{
		Packet(PacketType packetType = PacketType::PT_Invalid);
		PacketType GetPacketType();

		Packet& operator << (uint32_t data); //insertion operator
		Packet& operator >> (uint32_t& data); //extraction operator
		Packet& operator << (const std::vector< char > & data); //insertion operator
		Packet& operator >> (std::vector< char > & data); //extraction operator
		Packet& operator << (const std::vector< uint32_t >& data);
		Packet& operator >> (std::vector < uint32_t >& data);
		Packet& operator << (const std::string& data);//insertion operator
		Packet& operator >> (std::string& data);//extraction operator
		uint32_t m_extractionOffset = 0; //where to read
		std::vector<char> m_buffer;
		//bool operator==(const Packet& rhs) const;
		friend bool operator==(const Packet& lhs, const Packet& rhs);
	private:
		void AssignPacketType(PacketType packetType);
		void Clear();
		void Append(const void* data, uint32_t size);
		void AppendInteger(const uint32_t* data, uint32_t size);
	};
}