#include <gtest/gtest.h>
#include <TCPConnection.h>

using namespace Network;

TEST(TCPConnectionTest, ConstructTCPConnection)
{
	IPAddress ip("127.0.0.1", 4590);
	TCPSocket socket;
	TCPConnection connection(socket, ip);
	ASSERT_EQ(connection.m_socket, socket);
	ASSERT_EQ(connection.GetIpAddress(), ip);
	ASSERT_FALSE(connection.pm_incoming.HasPendingPackets());
	ASSERT_FALSE(connection.pm_outgoing.HasPendingPackets());
}

TEST(TCPConnectionTest, StringRepresentation)
{
	IPAddress ip("127.0.0.1", 4590);
	TCPSocket socket;
	TCPConnection connection(socket, ip);
	ASSERT_EQ(connection.ToString(), "[127.0.0.1:4590]");
}