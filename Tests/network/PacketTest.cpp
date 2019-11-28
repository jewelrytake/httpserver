#include <Packet.hpp>
#include <gtest/gtest.h>

using namespace Network;

TEST(PacketTest, DefaultPacketType)
{
	Packet packet;
	ASSERT_EQ(packet.m_buffer.size(), 2);
	ASSERT_EQ(packet.GetPacketType(), PacketType::PT_Invalid);
}

TEST(PacketTest, AssignAndGetPacketType)
{
	Packet packet(PacketType::PT_Test);
	ASSERT_EQ(packet.m_buffer.size(), 2);
	ASSERT_EQ(packet.m_extractionOffset, 2);
	ASSERT_EQ(packet.GetPacketType(), PacketType::PT_Test);
	//second byte is the packet type
	ASSERT_EQ(packet.m_buffer[1], 3U);
}

TEST(PacketTest, CheckIntegerExtractionOffset)
{
	Packet packet(PacketType::PT_Test);
	ASSERT_EQ(packet.m_extractionOffset, 2);
	uint32_t in1 = 0;
	uint32_t in2 = 0;
	uint32_t out1 = 0;
	uint32_t out2 = 0;

	packet << in1;
	ASSERT_EQ(packet.m_extractionOffset, 2);
	packet << in2;
	ASSERT_EQ(packet.m_extractionOffset, 2);
	packet >> out1;
	ASSERT_EQ(packet.m_extractionOffset, 6);
	packet >> out2;
	ASSERT_EQ(packet.m_extractionOffset, 10);
}

TEST(PacketTest, CheckByteVectorExtractionOffset)
{
	Packet packet(PacketType::PT_Test);
	ASSERT_EQ(packet.m_extractionOffset, 2);
	std::vector< uint8_t > in1 = { 10, 20, 30, 40, 50, 231, 131, 3 };
	std::vector< uint8_t > in2 = { 1, 100, 5, 32, 2, 1, 10, 32 };
	std::vector< uint8_t > out1;
	std::vector< uint8_t > out2;

	packet << in1;
	ASSERT_EQ(packet.m_extractionOffset, 2);
	packet << in2;
	ASSERT_EQ(packet.m_extractionOffset, 2);
	packet >> out1;
	ASSERT_EQ(packet.m_extractionOffset, 14);
	packet >> out2;
	ASSERT_EQ(packet.m_extractionOffset, 26);
}

TEST(PacketTest, CheckIntegerVectorExtractionOffset)
{
	Packet packet(PacketType::PT_Test);
	ASSERT_EQ(packet.m_extractionOffset, 2);
	std::vector< uint32_t > in1 = { 2312419, 3213213, 45343, 1111, 2000, 300, 275, 300 };
	std::vector< uint32_t > in2 = { 1, 100, 5, 32, 2, 1, 10, 32 };
	std::vector< uint32_t > out1;
	std::vector< uint32_t > out2;

	packet << in1;
	ASSERT_EQ(packet.m_extractionOffset, 2);
	packet << in2;
	ASSERT_EQ(packet.m_extractionOffset, 2);
	packet >> out1;
	ASSERT_EQ(packet.m_extractionOffset, 38);
	ASSERT_EQ(in1, out1);
	packet >> out2;
	ASSERT_EQ(packet.m_extractionOffset, 74);
	ASSERT_EQ(in1.size(), out1.size());
	ASSERT_EQ(in2.size(), out2.size());
	
	ASSERT_EQ(in2, out2);
	
}
TEST(PacketTest, PacketInsertExtractData)
{
	Packet packet(PacketType::PT_Test);
	std::vector< uint8_t > testBuffer1 = { 10, 20, 30, 40, 50, 231, 131, 3 };
	std::vector< uint8_t > testBuffer2 = { 1, 100, 5, 32, 2, 1, 10, 32 };
	std::vector< uint8_t > outBuffer;
	std::vector< uint8_t > outBuffer2;
	packet << testBuffer1;
	packet << testBuffer2;

	packet >> outBuffer;
	packet >> outBuffer2;
	ASSERT_EQ(outBuffer.size(), 8);
	ASSERT_EQ(outBuffer, testBuffer1);

	ASSERT_EQ(outBuffer2.size(), 8);
	ASSERT_EQ(outBuffer2, testBuffer2);
}