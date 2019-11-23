#include <IPAddress.h>

Network::IPAdress::IPAdress()
	: m_port(0), m_version(IPVersion::Unknown) 
{}

Network::IPAdress::IPAdress(const char* address, const std::uint16_t port)
	: m_address(address), m_port(port), m_version(IPVersion::IPv4)
{}

