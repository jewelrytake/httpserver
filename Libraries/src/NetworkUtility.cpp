#include <NetworkUtility.hpp>
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
	return u0 | (u1 << 8) | (u2 << 16) | (u3 << 24);
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

void ProcessPacketSize(Network::TCPConnection& connection, uint8_t& conditionFlag)
{
	if (connection.pm_incoming.currentPacketExtractionOffset == sizeof(uint16_t))
	{
		connection.pm_incoming.currentPacketSize = ntohs(connection.pm_incoming.currentPacketSize);
		if (connection.pm_incoming.currentPacketSize > Network::g_maxPacketSize)
		{
			conditionFlag = 1;
		}
		connection.pm_incoming.currentPacketExtractionOffset = 0;
		connection.pm_incoming.m_currentTask = Network::PacketTask::ProcessPacketContents;
	}
}
#pragma endregion End code specific to read data

void ReventsError(WSAPOLLFD& fd, std::string& status)
{
	status.clear();
	if (fd.revents & POLLERR) //If error occurred on this socket
	{
		status = "POLLERR";
	}
	if (fd.revents & POLLHUP) //If poll hangup occurred on this socket
	{
		status = "POLLHUP";
	}
	if (fd.revents & POLLNVAL) //If invalid socket
	{
		status = "POLLNVAL";
	}
}

void ReceivedBytesError(int receivedBytes, std::string& status)
{
	status.clear();
	if (receivedBytes == 0) //If connection was lost
	{
		status = "Recv == 0";
	}
	if (receivedBytes == SOCKET_ERROR) //If error occurred on socket
	{
		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK)
		{
			status = "Recv < 0";
		}
	}
}

void SetSocketStatus(Network::TCPSocket& connected, WSAPOLLFD& connectedFD)
{
	connectedFD.fd = connected.GetHandle();
	connectedFD.events = POLLRDNORM;
	connectedFD.revents = 0;
}

int ReceiveData(Network::TCPConnection& connection, WSAPOLLFD& use_fd)
{
	int bytesReceived = 0;
	if (connection.pm_incoming.m_currentTask == Network::PacketTask::ProcessPacketSize)
	{
		bytesReceived = recv(
			use_fd.fd,
			(char*)(int)(&connection.pm_incoming.currentPacketSize + connection.pm_incoming.currentPacketExtractionOffset),
			sizeof(uint16_t) - (int)connection.pm_incoming.currentPacketExtractionOffset,
			0);
	}
	else //Process Packet Contents
	{
		bytesReceived = recv(
			use_fd.fd,
			(char*)(int)(&connection.m_buffer + connection.pm_incoming.currentPacketExtractionOffset),
			int (connection.pm_incoming.currentPacketSize - connection.pm_incoming.currentPacketExtractionOffset),
			0);
	}
	return bytesReceived;
}

void SendSizeData(Network::PacketManager& pm, WSAPOLLFD& use_fd, uint8_t& flag)
{
	pm.currentPacketSize = (uint16_t)pm.GetCurrentPacket()->m_buffer.size();
	uint16_t netPacketSize = htons(pm.currentPacketSize);
	int bytesSent = send(
		use_fd.fd,
		(char*)&netPacketSize + pm.currentPacketExtractionOffset,
		sizeof(uint16_t) - pm.currentPacketExtractionOffset,
		0);
	if (bytesSent > 0)
	{
		pm.currentPacketExtractionOffset += bytesSent;
	}
	//if full packet was sent
	if (pm.currentPacketExtractionOffset == sizeof(uint16_t))
	{
		pm.currentPacketExtractionOffset = 0;
		pm.m_currentTask = Network::PacketTask::ProcessPacketContents;
	}
	else
	{
		//If full packet size was not sent, break out of the loop for sending outgoing packets for this connection
		//- we'll have to try again next time we are able to write normal data without blocking
		flag = 1;
	}
}

void SendContentData(Network::PacketManager& pm, WSAPOLLFD& use_fd, uint8_t& flag)
{
	char* bufferPtr = &pm.GetCurrentPacket()->m_buffer[0];
	int bytesSent = send(use_fd.fd,
		(char*)(bufferPtr) + pm.currentPacketExtractionOffset,
		pm.currentPacketSize - pm.currentPacketExtractionOffset,
		0);
	if (bytesSent > 0)
	{
		pm.currentPacketExtractionOffset += bytesSent;
	}
	if (pm.currentPacketExtractionOffset == pm.currentPacketSize) //If full packet contents have been sent
	{
		pm.currentPacketExtractionOffset = 0;
		pm.m_currentTask = Network::PacketTask::ProcessPacketSize;
		pm.Pop(); //Remove packet from queue after finished processing
	}
	else
	{
		flag = 1;
	}
}

