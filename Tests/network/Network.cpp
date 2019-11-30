#include <Initializer.hpp>
#include <gtest/gtest.h>

TEST(NetworkTest, Initialize)
{
	ASSERT_TRUE(Network::Initialize());
}