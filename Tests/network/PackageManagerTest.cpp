#include <gtest/gtest.h>
#include <PacketManager.hpp>

using namespace Network;

TEST(PackageManagerTest, NoPendingPackets)
{
	PacketManager pm;
	ASSERT_FALSE(pm.HasPendingPackets());
	ASSERT_EQ(pm.currentPacketExtractionOffset, 0);
	ASSERT_EQ(pm.currentPacketSize, 0);
	ASSERT_EQ(pm.m_currentTask, PacketTask::ProcessPacketSize);
}

TEST(PackageManagerTest, AppendPacketToQueue)
{
	PacketManager pm;
	Packet packet1(PacketType::PT_Test);
	Packet packet2(PacketType::PT_Test);
	Packet packet3(PacketType::PT_Test);
	packet1 << 5;
	pm.Append(std::make_shared<Packet>(packet1));
	const std::vector < uint32_t > in1 = { 10, 20, 30, 40, 50, 60 };
	std::vector < uint32_t > out1;
	packet2 << in1;
	packet3 << in1;
	pm.Append(std::make_shared<Packet>(packet2));
	ASSERT_TRUE(pm.HasPendingPackets());
	ASSERT_EQ(*pm.GetCurrentPacket(), packet1);	
}

TEST(PackageManagerTest, ClearPacketQueue)
{
	PacketManager pm;
	Packet packet1(PacketType::PT_Test);
	Packet packet2(PacketType::PT_Test);
	Packet packet3(PacketType::PT_Test);
	const std::vector < uint32_t > in1 = { 10, 20, 30, 40, 50, 60 };
	packet1 << 5;
	packet2 << in1;
	packet3 << in1;
	pm.Append(std::make_shared<Packet>(packet1));
	pm.Append(std::make_shared<Packet>(packet2));
	pm.Append(std::make_shared<Packet>(packet3));
	ASSERT_TRUE(pm.HasPendingPackets());
	pm.Clear();
	ASSERT_FALSE(pm.HasPendingPackets());
}