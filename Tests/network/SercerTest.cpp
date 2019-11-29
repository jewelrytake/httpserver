#include <gtest/gtest.h>
#include <Server.h>

using namespace Network;

TEST(TestServer, StartServer)
{
	Server server;
	ASSERT_TRUE(server.Start(IPAddress("127.0.0.1", 4590)));

}