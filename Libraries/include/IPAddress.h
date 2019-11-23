#pragma once
#include <string>
#include <IPVersion.h>
#include <cstdint>
#include <vector>
#include <WS2tcpip.h>

namespace Network
{
	struct IPAdress
	{
		IPAdress();
		IPAdress(const char* address, const std::uint16_t port);
		std::string GetDomain();
		std::string GetAddress();
		uint16_t GetPort();
		IPVersion GetVersion();
		std::vector<uint8_t> GetStream();
	private:
		std::string m_domain;
		std::string m_address;
		uint16_t m_port;
		IPVersion m_version;
		std::vector<uint8_t> m_stream;
		
	};
}
