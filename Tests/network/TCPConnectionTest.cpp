#include <gtest/gtest.h>
#include <TCPConnection.h>
#include <Initializer.h>

using namespace Network;

TEST(TCPConnectionTest, ConstructTCPConnection)
{
	Network::Initialize();
	IPAddress ip("127.0.0.1", 4590);
	TCPSocket sock;
	ASSERT_TRUE(sock.Create());
	TCPConnection connection(sock, ip);

	ASSERT_EQ(connection.m_socket, sock);
	ASSERT_EQ(connection.GetIpAddress(), ip);
	ASSERT_FALSE(connection.pm_incoming.HasPendingPackets());
	ASSERT_FALSE(connection.pm_outgoing.HasPendingPackets());
	Network::Shutdown();
}

TEST(TCPConnectionTest, StringRepresentation)
{
	Network::Initialize();
	IPAddress ip("127.0.0.1", 4590);
	TCPSocket sock;
	TCPConnection connection(sock, ip);
	ASSERT_EQ(connection.ToString(), "[127.0.0.1:4590]");
	Network::Shutdown();
}

TEST(TCPConnectionTest, CloseConnection)
{
	Network::Initialize();
	IPAddress ip("127.0.0.1", 4590);
	TCPSocket sock;
	ASSERT_TRUE(sock.Create());
	TCPConnection connection(sock, ip);
	ASSERT_NE(connection.m_socket.GetHandle(), INVALID_SOCKET);
	connection.Close();
	ASSERT_EQ(connection.m_socket.GetHandle(), INVALID_SOCKET);
	Network::Shutdown();
}