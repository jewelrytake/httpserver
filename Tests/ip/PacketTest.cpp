#include <Packet.hpp>
#include <gtest/gtest.h>

TEST(PacketTest, AssignAndGetPacketType)
{
	Network::Packet packet(Network::PacketType::PT_Test);
	ASSERT_EQ(packet.m_buffer.size(), 2);
	ASSERT_EQ(packet.m_extractionOffset, 2);
	ASSERT_EQ(packet.GetPacketType(), Network::PacketType::PT_Test);
	//second byte is packet type
	ASSERT_EQ(packet.m_buffer[1], 3);
}
TEST(PacketTest, PacketInsertExtractData)
{
	Network::Packet packet(Network::PacketType::PT_Test);
	const std::vector< uint8_t > testBuffer = { 10, 20, 30, 40, 50, 231, 131, 3 };
	uint32_t length = 0;
	std::vector< uint8_t > outBuffer;
	packet << testBuffer;
	ASSERT_EQ(14, packet.m_buffer.size());
	ASSERT_EQ(2, packet.m_extractionOffset);
	packet >> outBuffer;

	ASSERT_EQ(outBuffer.size(), 8);
	ASSERT_EQ(outBuffer, testBuffer);
}