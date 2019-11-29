#include <gtest/gtest.h>
#include <Server.h>
#include <Initializer.h>

using namespace Network;

TEST(ServerTest, StartServer)
{
	Initialize();
	Server server;
	ASSERT_TRUE(server.Start(IPAddress("127.0.0.1", 7777)));
	Shutdown();
}