#include <NetworkUtility.h>
#include <iostream>



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

ConditionStrategy ProcessPacketSize(Network::TCPConnection& connection, int bytesReceived, ConditionStrategy cs)
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
}
#pragma endregion End code specific to read data

