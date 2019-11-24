#include <Initializer.h>
#include <gtest/gtest.h>

TEST(NetworkTest, Initialize)
{
	ASSERT_TRUE(Network::Initialize());
}