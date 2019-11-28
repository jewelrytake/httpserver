#include <NetworkUtility.h>
#include <iostream>

void DecomposeInt_32(unsigned char* buf, int32_t val)
{
	uint32_t uval = val;
	buf[0] = uval;
	buf[1] = uval >> 8;
	buf[2] = uval >> 16;
	buf[3] = uval >> 24;
}

int32_t ComposeInt_32(unsigned char* buf)
{
	// This prevents buf[i] from being promoted to a signed int.
	uint32_t u0 = buf[0], u1 = buf[1], u2 = buf[2], u3 = buf[3];
	uint32_t uval = u0 | (u1 << 8) | (u2 << 16) | (u3 << 24);
	return uval;
}

#pragma region Read data
void ProcessPacketContent(Network::TCPConnection& connection)
{
	std::shared_ptr<Network::Packet> packet = std::make_shared<Network::Packet>();
	packet->m_buffer.resize(connection.pm_incoming.currentPacketSize);
	memcpy(&packet->m_buffer[0], connection.m_buffer, connection.pm_incoming.currentPacketSize);
	connection.pm_incoming.Append(packet);
	connection.pm_incoming.currentPacketSize = 0;
	connection.pm_incoming.currentPacketExtractionOffset = 0;
	connection.pm_incoming.m_currentTask = Network::PacketTask::ProcessPacketSize;
}

ConditionStrategy ProcessPacketSize(Network::TCPConnection& connection, ConditionStrategy cs)
{
	
	if (connection.pm_incoming.m_currentTask == Network::PacketTask::ProcessPacketSize)
	{
		if (connection.pm_incoming.currentPacketExtractionOffset == sizeof(uint16_t))
		{
			connection.pm_incoming.currentPacketSize = ntohs(connection.pm_incoming.currentPacketSize);
			if (connection.pm_incoming.currentPacketSize > Network::g_maxPacketSize)
			{
				if (cs == ConditionStrategy::ST_CONTINUE)
					return ConditionStrategy::ST_CONTINUE;
				else if (cs == ConditionStrategy::ST_FALSE)
					return ConditionStrategy::ST_FALSE;
			}
			connection.pm_incoming.currentPacketExtractionOffset = 0;
			connection.pm_incoming.m_currentTask = Network::PacketTask::ProcessPacketContents;
		}
	}
	return ConditionStrategy::ST_NONE;
}
#pragma endregion End code specific to read data

