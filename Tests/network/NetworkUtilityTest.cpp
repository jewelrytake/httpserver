#include <gtest/gtest.h>
#include <Server.hpp>
#include <Initializer.hpp>
#include <NetworkUtility.hpp>

using namespace Network;

TEST(NetworkUtilityTest, Int_32Conversion)
{
	char buf[4] = { 0 };
	uint32_t val = 31242;
	DecomposeInt_32(buf, val);
	uint32_t ret_val = ComposeInt_32(buf);
	ASSERT_EQ(val, ret_val);

	val = 1;
	DecomposeInt_32(buf, val);
	ret_val = ComposeInt_32(buf);
	ASSERT_EQ(val, ret_val);

	val = -100;
	DecomposeInt_32(buf, val);
	ret_val = ComposeInt_32(buf);
	ASSERT_EQ(val, ret_val);
}

TEST(NetworkUtilityTest, SetSocketStatus)
{
	Initialize();
	WSAPOLLFD fd;
	TCPSocket sock;
	ASSERT_TRUE(sock.Create());
	SetSocketStatus(sock, fd);
	ASSERT_EQ(fd.events, POLLRDNORM);
	ASSERT_EQ(fd.revents, 0);
	ASSERT_EQ(fd.fd, sock.GetHandle());
	Shutdown();
}

TEST(NetworkUtilityTest, ReventsError)
{
	Initialize();
	WSAPOLLFD fd;
	TCPSocket sock;
	sock.Create();
	SetSocketStatus(sock, fd);
	std::string status;
	ReventsError(fd, status);
	ASSERT_EQ(status, "");
	fd.revents = POLLERR;
	ReventsError(fd, status);
	ASSERT_EQ(status, "POLLERR");
	fd.revents = POLLHUP;
	ReventsError(fd, status);
	ASSERT_EQ(status, "POLLHUP");
	fd.revents = POLLNVAL;
	ReventsError(fd, status);
	ASSERT_EQ(status, "POLLNVAL");
	Shutdown();
}
