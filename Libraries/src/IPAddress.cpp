#include <IPAddress.h>
#include <iostream>
#include <assert.h>
#include <algorithm>

namespace
{
	//TODO: utils

}

Network::IPAddress::~IPAddress() = default;

Network::IPAddress::IPAddress()
	: m_port(0), m_version(IPVersion::Unknown) 
{}


Network::IPAddress::IPAddress(const char* address, const uint16_t port)
	: m_port(port), m_version(IPVersion::Unknown)
{
	if (!set_address(address, *this))
	{
		std::cerr << "Cannot process IP address.\n";
	}
}

Network::IPAddress::IPAddress(sockaddr* addr)
{
	assert(addr->sa_family == AF_INET || addr->sa_family == AF_INET6);
	if (addr->sa_family == AF_INET) //IPv4
	{
		sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);
		m_version = IPVersion::IPv4;
		m_port = ntohs(addrv4->sin_port);
		m_stream.resize(sizeof(ULONG));
		memcpy(&m_stream[0], &addrv4->sin_addr, sizeof(ULONG));
		m_address.resize(16);
		inet_ntop(AF_INET, &addrv4->sin_addr, &m_address[0], 16);
		m_address.erase(std::find(m_address.begin(), m_address.end(), '\0'), m_address.end());
		m_address.shrink_to_fit();
		m_domain = m_address;
	}
	else //IPv6
	{
		sockaddr_in6* addrv6 = reinterpret_cast<sockaddr_in6*>(addr);
		m_version = IPVersion::IPv6;
		m_port = ntohs(addrv6->sin6_port);
		m_stream.resize(16);
		memcpy(&m_stream[0], &addrv6->sin6_addr, 16);
		m_address.resize(46);
		inet_ntop(AF_INET6, &addrv6->sin6_addr, &m_address[0], 46);
		m_address.erase(std::find(m_address.begin(), m_address.end(), '\0'), m_address.end());
		m_address.shrink_to_fit();
		m_domain = m_address;
	}
}

std::string Network::IPAddress::GetDomain() const
{
	return m_domain;
}

std::string Network::IPAddress::GetAddress() const
{
	return m_address;
}

uint16_t Network::IPAddress::GetPort() const
{
	return m_port;
}

Network::IPVersion Network::IPAddress::GetVersion() const 
{
	return m_version;
}

std::vector<uint8_t> Network::IPAddress::GetStream() const
{
	return m_stream;
}
sockaddr_in Network::IPAddress::GetIPv4Address() const
{
	assert(m_version == IPVersion::IPv4);
	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	memcpy(&addr.sin_addr, &m_stream[0], sizeof(ULONG));
	addr.sin_port = htons(m_port);
	return addr;
}
sockaddr_in6 Network::IPAddress::GetIPv6Address() const
{
	assert(m_version == IPVersion::IPv6);
	sockaddr_in6 addr = {};
	addr.sin6_family = AF_INET6;
	memcpy(&addr.sin6_addr, &m_stream[0], 16);
	addr.sin6_port = htons(m_port);
	return addr;
}

bool Network::set_address(const char* ip, Network::IPAddress& outObject)
{
	in_addr addr{};
	int result = inet_pton(AF_INET, ip, &addr);
	if (result == 1)
	{
		if (addr.S_un.S_addr != INADDR_NONE)
		{
			outObject.m_address = ip;
			outObject.m_domain = ip;
			outObject.m_stream.resize(sizeof(ULONG));
			memcpy(&outObject.m_stream[0], &addr.S_un.S_addr, sizeof(ULONG));
			outObject.m_version = IPVersion::IPv4;
			return true;
		}
	}

	in6_addr addr6{};
	result = inet_pton(AF_INET6, ip, &addr6);
	if (result == 1)
	{
		outObject.m_address = ip;
		outObject.m_domain = ip;
		outObject.m_stream.resize(16);
		memcpy(&outObject.m_stream[0], &addr6.u, 16);
		outObject.m_version = IPVersion::IPv6;
		return true;
	}
	return false;
}
