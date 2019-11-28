#include <Client.h>
#include <iostream>
#include "NetworkUtility.h"

bool Network::Client::Connect(Network::IPAddress ip)
{
	m_isConnected = false;
	TCPSocket socket = TCPSocket(ip.GetVersion());
	if (socket.Create())
	{
		if (!socket.SetBlocking(true))
			return false;
		std::cout << "Socket successfully created.\n";
		if (socket.Connect(ip))
		{
			if (socket.SetBlocking(false))
			{
				m_connection = TCPConnection(socket, ip);
				m_master_fd.fd = m_connection.m_socket.GetHandle();
				m_master_fd.events = POLLRDNORM;
				m_master_fd.revents = 0;
				m_isConnected = true;
				OnConnect();
				return true;
			}
		}
		else
		{
			socket.Close();
		}
	}
	else
	{
		std::cerr << "Socket failed to create.\n";
	}
	OnConnectFail();
	return false;
}

bool Network::Client::IsConnected()
{
	return m_isConnected;
}

bool Network::Client::Frame()
{
	if (m_connection.pm_outgoing.HasPendingPackets())
	{
		m_master_fd.events = POLLRDNORM | POLLWRNORM;
	}
	m_use_fd = m_master_fd;
	if (WSAPoll(&m_use_fd, 1, 1) > 0)
	{
		if (m_use_fd.revents & POLLERR) //If error occurred on this socket
		{
			CloseConnection("POLLERR");
			return false;
		}
		if (m_use_fd.revents & POLLHUP) //If poll hangup occurred on this socket
		{
			CloseConnection("POLLHUP");
			return false;
		}
		if (m_use_fd.revents & POLLNVAL) //If invalid socket
		{
			CloseConnection("POLLNVAL");
			return false;
		}
		if (m_use_fd.revents & POLLRDNORM) //If normal data can be read without blocking
		{
			int bytesReceived = 0;
			if (m_connection.pm_incoming.m_currentTask == PacketTask::ProcessPacketSize)
			{
				bytesReceived = recv(
					m_use_fd.fd,
					(char*)&m_connection.pm_incoming.currentPacketSize + m_connection.pm_incoming.currentPacketExtractionOffset,
					sizeof(uint16_t) - m_connection.pm_incoming.currentPacketExtractionOffset,
					0);
			}
			else //Process Packet Contents
			{
				bytesReceived = recv(
					m_use_fd.fd,
					(char*)&m_connection.m_buffer + m_connection.pm_incoming.currentPacketExtractionOffset,
					m_connection.pm_incoming.currentPacketSize - m_connection.pm_incoming.currentPacketExtractionOffset,
					0);
			}
			if (bytesReceived == 0) //If connection was lost
			{
				CloseConnection("Recv==0");
				return false;
			}
			if (bytesReceived == SOCKET_ERROR) //If error occurred on socket
			{
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
				{
					CloseConnection("Recv<0");
					return false;
				}
			}
			if (bytesReceived > 0)
			{
				m_connection.pm_incoming.currentPacketExtractionOffset += bytesReceived;
				if (ProcessPacketSize(m_connection, ConditionStrategy::ST_CONTINUE) == ConditionStrategy::ST_CONTINUE)
				{
					CloseConnection("Packet size too large.");
					return false;
				}
					
				else //Processing packet contents
				{
					if (m_connection.pm_incoming.currentPacketExtractionOffset == m_connection.pm_incoming.currentPacketSize)
					{
						ProcessPacketContent(m_connection);
					}
				}
			}
		}

		if (m_use_fd.revents & POLLWRNORM) //If normal data can be written without blocking
		{
			PacketManager& pm = m_connection.pm_outgoing;
			while (pm.HasPendingPackets())
			{
				if (pm.m_currentTask == PacketTask::ProcessPacketSize) //Sending packet size
				{
					pm.currentPacketSize = (uint16_t)pm.GetCurrentPacket()->m_buffer.size();
					uint16_t bigEndianPacketSize = htons(pm.currentPacketSize);
					int bytesSent = send(
						m_use_fd.fd,
						(char*)(&bigEndianPacketSize) + pm.currentPacketExtractionOffset,
						sizeof(uint16_t) - pm.currentPacketExtractionOffset,
						0);
					if (bytesSent > 0)
					{
						pm.currentPacketExtractionOffset += bytesSent;
					}

					if (pm.currentPacketExtractionOffset == sizeof(uint16_t)) //If full packet size was sent
					{
						pm.currentPacketExtractionOffset = 0;
						pm.m_currentTask = PacketTask::ProcessPacketContents;
					}
					else //If full packet size was not sent, break out of the loop for sending outgoing packets for this connection
						//- we'll have to try again next time we are able to write normal data without blocking
					{
						break;
					}
				}
				else //Sending packet contents
				{
					uint8_t* bufferPtr = &pm.GetCurrentPacket()->m_buffer[0];
					int bytesSent = send(m_use_fd.fd,
						(char*)(bufferPtr)+pm.currentPacketExtractionOffset,
						pm.currentPacketSize - pm.currentPacketExtractionOffset,
						0);
					if (bytesSent > 0)
					{
						pm.currentPacketExtractionOffset += bytesSent;
					}

					if (pm.currentPacketExtractionOffset == pm.currentPacketSize) //If full packet contents have been sent
					{
						pm.currentPacketExtractionOffset = 0;
						pm.m_currentTask = PacketTask::ProcessPacketSize;
						pm.Pop(); //Remove packet from queue after finished processing
					}
					else
					{
						break;
					}
				}
			}
			if (!m_connection.pm_outgoing.HasPendingPackets())
			{
				m_master_fd.events = POLLRDNORM;
			}
		}
	}
	while (m_connection.pm_incoming.HasPendingPackets())
	{
		std::shared_ptr<Packet> frontPacket = m_connection.pm_incoming.GetCurrentPacket();
		if (!ProcessPacket(frontPacket))
		{
			CloseConnection("Failed to process incoming packet.");
			return false;
		}
		m_connection.pm_incoming.Pop();
	}
	return true;
}

void Network::Client::Send(std::shared_ptr<Packet> packet)
{
	m_connection.pm_outgoing.Append(packet);
}

void Network::Client::Receive(std::shared_ptr<Packet> packet)
{
	
}

bool Network::Client::ProcessPacket(std::shared_ptr<Packet> packet)
{
	std::cout << "Packet received with size: " << packet->m_buffer.size() << '\n';
	return true;
}

void Network::Client::OnConnect()
{
	std::cout << "Successfully connected!\n";
}

void Network::Client::OnConnectFail()
{
	std::cout << "Failed to connect.\n";
}

void Network::Client::OnDisconnect(std::string reason)
{
	std::cout << "Lost connection. Reason: " << reason << ".\n";
}

void Network::Client::CloseConnection(std::string reason)
{
	OnDisconnect(reason);
	m_master_fd.fd = 0;
	m_isConnected = false;
	m_connection.Close();
}