#include <IPAddress.h>
#include <gtest/gtest.h>
#include <cstdint>

TEST(IPTest, CorrectIpAddress)
{
	
}
TEST(IPTest, IPv4)
{
	Network::IPAddress ip("127.0.0.1", (uint16_t)8080);
	ASSERT_EQ(ip.GetAddress(), "127.0.0.1");
	ASSERT_EQ(ip.GetDomain(), "127.0.0.1");
	ASSERT_EQ(ip.GetPort(), 8080);
	ASSERT_EQ(ip.GetVersion(), Network::IPVersion::IPv4);
	std::vector< uint8_t > bytesStream = { 127, 0, 0, 1 };
	ASSERT_EQ(ip.GetStream(), bytesStream);
}

TEST(IPTest, IPv6)
{
	Network::IPAddress ip("::1", (uint16_t)8080);
	ASSERT_EQ(ip.GetAddress(), "::1");
	ASSERT_EQ(ip.GetDomain(), "::1");
	ASSERT_EQ(ip.GetPort(), 8080);
	ASSERT_EQ(ip.GetVersion(), Network::IPVersion::IPv6);
	std::vector< uint8_t > bytesStream = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
	ASSERT_EQ(ip.GetStream(), bytesStream);
}