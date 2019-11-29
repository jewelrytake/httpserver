#pragma once
#include <string>
#include <IPVersion.h>
#include <cstdint>
#include <vector>
#include <WS2tcpip.h>

namespace Network
{
	struct IPAddress
	{
		IPAddress();
		~IPAddress();

		//API
		IPAddress(const char* address, const uint16_t port = 80);
		IPAddress(sockaddr* addr); //when we call accept, need to identify addresses
		std::string GetDomain() const;
		std::string GetAddress() const;
		uint16_t GetPort() const;
		IPVersion GetVersion() const;
		std::vector<uint8_t> GetStream() const;
		sockaddr_in GetIPv4Address() const;
		sockaddr_in6 GetIPv6Address() const;
		friend bool set_address(const char* ip, Network::IPAddress& outObject);
		friend bool operator==(const IPAddress& lhs, const IPAddress& rhs);
	private:
		std::string m_domain;
		std::string m_address;
		uint16_t m_port;
		IPVersion m_version;
		std::vector<uint8_t> m_stream;
	};
}
