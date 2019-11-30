#include <gtest/gtest.h>
#include <TCPSocket.hpp>
#include <Initializer.hpp>

using namespace Network;

TEST(TCPSocketTest, DefaultInitializationSocket)
{
	Network::Initialize();
	TCPSocket sock;
	ASSERT_EQ(sock.GetIPVersion(), IPVersion::IPv4);
	ASSERT_EQ(sock.GetHandle(), INVALID_SOCKET);
	ASSERT_EQ(sock.GetProtocol(), IPPROTO_TCP);
	ASSERT_EQ(sock.GetType(), SOCK_STREAM);
	Network::Shutdown();
}

TEST(TCPSocketTest, CreateSocket)
{
	Network::Initialize();
	TCPSocket sock;
	ASSERT_TRUE(sock.Create());
	ASSERT_EQ(sock.GetIPVersion(), IPVersion::IPv4);
	ASSERT_NE(sock.GetHandle(), INVALID_SOCKET);
	ASSERT_EQ(sock.GetProtocol(), IPPROTO_TCP);
	ASSERT_EQ(sock.GetType(), SOCK_STREAM);
}

TEST(TCPSocketTest, CheckBlockingState)
{
	Network::Initialize();
	TCPSocket sock;
	sock.Create();
	ASSERT_FALSE(sock.IsBlocking());
	ASSERT_TRUE(sock.SetBlocking(true));
	ASSERT_TRUE(sock.IsBlocking());
	ASSERT_TRUE(sock.SetBlocking(false));
	ASSERT_FALSE(sock.IsBlocking());
}

TEST(TCPSocketTest, CloseSocket)
{
	Network::Initialize();
	TCPSocket sock;
	ASSERT_TRUE(sock.Create());
	ASSERT_EQ(sock.GetIPVersion(), IPVersion::IPv4);
	ASSERT_NE(sock.GetHandle(), INVALID_SOCKET);
	ASSERT_EQ(sock.GetProtocol(), IPPROTO_TCP);
	ASSERT_EQ(sock.GetType(), SOCK_STREAM);

	sock.Close();
	ASSERT_EQ(sock.GetIPVersion(), IPVersion::IPv4);
	ASSERT_EQ(sock.GetHandle(), INVALID_SOCKET);
	ASSERT_EQ(sock.GetProtocol(), IPPROTO_TCP);
	ASSERT_EQ(sock.GetType(), SOCK_STREAM);
	Network::Shutdown();
}

TEST(TCPSocketTest, BindSocket)
{
	Network::Initialize();
	TCPSocket sock;
	IPAddress ip("127.0.0.1", 3590);
	sock.Create();
	ASSERT_TRUE(sock.Bind(ip));
	sock.Close();
	Network::Shutdown();
}


TEST(TCPSocketTest, ListenSocket)
{
	Network::Initialize();
	TCPSocket sock;
	IPAddress ip("127.0.0.1", 3590);
	sock.Create();
	ASSERT_TRUE(sock.Listen(ip));
	sock.Close();
	Network::Shutdown();
}

TEST(TCPSocketTest, BlockingConnectSocket)
{
	Network::Initialize();
	TCPSocket sock;
	TCPSocket client;
	IPAddress ip("127.0.0.1", 3590);
	sock.Create();
	client.Create();
	sock.Listen(ip);
	client.SetBlocking(true);
	ASSERT_TRUE(client.Connect(ip));
	sock.Close();
	client.Close();
	Network::Shutdown();
}

TEST(TCPSocketTest, NonBlockingConnectSocket)
{
	Network::Initialize();
	TCPSocket sock;
	TCPSocket client;
	IPAddress ip("127.0.0.1", 3590);
	sock.Create();
	client.Create();
	sock.Listen(ip);
	//TODO: 
	ASSERT_FALSE(client.Connect(ip));
	sock.Close();
	client.Close();
	Network::Shutdown();
}

TEST(TCPSocketTest, AcceptSocket)
{
	Network::Initialize();
	TCPSocket sock;
	TCPSocket accepted;
	TCPSocket client;
	IPAddress ip("127.0.0.1", 3590);
	IPAddress acceptedIp;
	sock.Create();
	client.Create();
	sock.Listen(ip);
	client.Connect(ip);
	Sleep(50);
	ASSERT_TRUE(sock.Accept(accepted, &acceptedIp));
	ASSERT_NE(accepted.GetHandle(), INVALID_SOCKET);
	ASSERT_EQ(acceptedIp.GetAddress(), ip.GetAddress());
	ASSERT_EQ(acceptedIp.GetDomain(), ip.GetDomain());
	ASSERT_EQ(acceptedIp.GetStream(), ip.GetStream());
	ASSERT_EQ(acceptedIp.GetVersion(), ip.GetVersion());
	
	sock.Close();
	accepted.Close();
	accepted.Close();
	Network::Shutdown();
}