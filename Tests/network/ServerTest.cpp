#include <gtest/gtest.h>
#include <Server.hpp>
#include <Initializer.hpp>

using namespace Network;

TEST(ServerTest, StartServer)
{
	Initialize();
	Server server;
	ASSERT_TRUE(server.Start(IPAddress("127.0.0.1", 7777)));
	Shutdown();
}