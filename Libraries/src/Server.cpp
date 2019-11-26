#include <Initializer.h>
#include <Server.h>
#include <iostream>
#include "NetworkUtility.h"

namespace Network
{
	bool Server::Start(IPAddress ip)
	{
		m_master_fd.clear();
		m_connections.clear();
		m_listeningSocket = TCPSocket(ip.GetVersion());
		if (m_listeningSocket.Create())
		{
			std::cout << "Socket successfully created.\n";
			if (m_listeningSocket.Listen(ip))
			{
				WSAPOLLFD listeningSocketFD = {};//WSAPOLLFD structure stores socket information used by the WSAPoll function.
				listeningSocketFD.fd = m_listeningSocket.GetHandle(); //The identifier of the socket for which to find status.
				listeningSocketFD.events = POLLRDNORM; //A set of flags indicating the type of status being requested.
				listeningSocketFD.revents = 0; //the results of the status query.
				m_master_fd.emplace_back(listeningSocketFD);//very first element will be listeningSocket
				std::cout << "Socket successfully listening.\n";
				return true;
			}
			else
			{
				std::cerr << "Failed to listen.\n";
			}
			m_listeningSocket.Close();
		}
		else
		{
			std::cerr << "Socket failed to create.\n";
		}
		return false;
	}
	void Server::Frame()
	{
		//if the connection has pending packets, we set flags 
		//for the current socket indicating the type of status being requested(read/write)
		for (size_t i = 0; i < m_connections.size(); i++)
		{
			if (m_connections[i].pm_outgoing.HasPendingPackets())
			{
				m_master_fd[i + 1].events = POLLRDNORM | POLLWRNORM;
			}
		}
		//m_master_fd - array of one or more POLLFD structures specifying the set of sockets for which status is requested
		m_use_fd = m_master_fd;//use_fd - temp vector, we won't have to through and reset all of the events every time
		//WSAPoll function determines status of one or more sockets
		if (WSAPoll(m_use_fd.data(), m_use_fd.size(), 1) > 0)//if at least one fd that were querying
		{
#pragma region listener
			auto& listeningSocketFD = m_use_fd[0];
			if (listeningSocketFD.revents & POLLRDNORM) //if the listening socket requests only read
			{
				TCPSocket connectedSocket;
				IPAddress connectedIp;
				if (m_listeningSocket.Accept(connectedSocket, &connectedIp))
				{
					m_connections.emplace_back(TCPConnection(connectedSocket, connectedIp));
					TCPConnection& acceptedConnection = m_connections.back();
					WSAPOLLFD connectedFD = {};//WSAPOLLFD structure stores socket information used by the WSAPoll function.
					connectedFD.fd = connectedSocket.GetHandle();
					connectedFD.events = POLLRDNORM;
					connectedFD.revents = 0;
					m_master_fd.emplace_back(connectedFD);
					OnConnect(acceptedConnection, *this);
					//m_use_fd.emplace_back(connectedFD);
				}
				else
				{
					std::cerr << "Failed to accept new connection." << '\n';
				}
			}
#pragma endregion End of code specific to the listening socket

#pragma region ERROR_CHECKING
			for (size_t i = m_use_fd.size() - 1; i >= 1; i--)
			{
				int connectionIndex = i - 1;
				//Retrieve socket connected to server 
				TCPConnection& connection = m_connections[connectionIndex];
				if (m_use_fd[i].revents & POLLERR) //If error occurred on this socket
				{
					CloseConnection(connectionIndex, "POLLERR", *this);
					continue;
				}
				if (m_use_fd[i].revents & POLLHUP) //If poll hangup occurred on this socket
				{
					CloseConnection(connectionIndex, "POLLHUP", *this);
					continue;
				}
				if (m_use_fd[i].revents & POLLNVAL) //If invalid socket
				{
					CloseConnection(connectionIndex, "Invalid socket", *this);
					continue;
				}
#pragma endregion End of ERROR_CHECKING

#pragma region Read data from a client
				if (m_use_fd[i].revents & POLLRDNORM) //If normal data can be read without blocking from a client
				{
					int bytesReceived = 0;

					if (connection.pm_incoming.m_currentTask == PacketTask::ProcessPacketSize)
					{
						
						bytesReceived = recv(
							m_use_fd[i].fd,
							(char*)&connection.pm_incoming.currentPacketSize + connection.pm_incoming.currentPacketExtractionOffset,
							sizeof(uint16_t) - connection.pm_incoming.currentPacketExtractionOffset,
							0);
					}
					else //Recieve Packet Contents
					{
						bytesReceived = recv(
							m_use_fd[i].fd,
							(char*)&connection.m_buffer + connection.pm_incoming.currentPacketExtractionOffset,
							connection.pm_incoming.currentPacketSize - connection.pm_incoming.currentPacketExtractionOffset,
							0);

					}
					if (bytesReceived == 0) //If connection was lost
					{
						CloseConnection(connectionIndex, "Recv == 0", *this);
						continue;
					}
					if (bytesReceived == SOCKET_ERROR) //If error occurred on socket
					{
						int error = WSAGetLastError();
						if (error != WSAEWOULDBLOCK)
						{
							CloseConnection(connectionIndex, "Recv < 0", *this);
							continue;
						}
					}
					if (bytesReceived > 0)
					{
						connection.pm_incoming.currentPacketExtractionOffset += bytesReceived;
						if (ProcessPacketSize(connection, bytesReceived, connectionIndex, *this, ConditionStrategy::ST_CONTINUE) == ConditionStrategy::ST_CONTINUE)
							continue;
						else //Processing packet contents
						{
							if (connection.pm_incoming.currentPacketExtractionOffset == connection.pm_incoming.currentPacketSize)
							{
								ProcessPacketContent(connection);
							}
						}
					}
				}
#pragma endregion End code specific to read data from a client

#pragma region Send data to client
				if (m_use_fd[i].revents & POLLWRNORM)
				{
					PacketManager& pm = connection.pm_outgoing;
					while (pm.HasPendingPackets())
					{
						if (pm.m_currentTask == PacketTask::ProcessPacketSize)
						{
							pm.currentPacketSize = (uint16_t)pm.Retrieve()->m_buffer.size();
							uint16_t netPacketSize = htons(pm.currentPacketSize);
							int bytesSent = send(
								m_use_fd[i].fd,
								(char*)&netPacketSize + pm.currentPacketExtractionOffset,
								sizeof(uint16_t) - pm.currentPacketExtractionOffset,
								0);
							if (bytesSent > 0)
							{
								pm.currentPacketExtractionOffset += bytesSent;
							}
							if (pm.currentPacketExtractionOffset == sizeof(uint16_t))
							{
								pm.currentPacketExtractionOffset = 0;
								pm.m_currentTask = PacketTask::ProcessPacketContents;
							}
							else
							{
								//If full packet size was not sent, break out of the loop for sending outgoing packets for this connection
								//- we'll have to try again next time we are able to write normal data without blocking
								break;
							}
						}
						else
						{
							uint8_t* bufferPtr = &pm.Retrieve()->m_buffer[0];
							int bytesSent = send(
								m_use_fd[i].fd,
								(char*)bufferPtr + pm.currentPacketExtractionOffset,
								pm.currentPacketSize - pm.currentPacketExtractionOffset,
								0);
							if (bytesSent > 0)
							{
								pm.currentPacketExtractionOffset += bytesSent;
							}
							if (pm.currentPacketExtractionOffset == pm.currentPacketSize)
							{
								pm.currentPacketExtractionOffset = 0;
								pm.m_currentTask = PacketTask::ProcessPacketSize;
								pm.Pop();
							}
							else
							{
								break;
							}
						}
					}
					if (!pm.HasPendingPackets())
					{
						m_master_fd[i].events = POLLRDNORM;
					}
				}
			}
		}
#pragma endregion End code specific to send data to client

		for (int i = m_connections.size() - 1; i >= 0; i--)
		{
			while (m_connections[i].pm_incoming.HasPendingPackets())
			{
				std::shared_ptr<Packet> frontPacket = m_connections[i].pm_incoming.Retrieve();
				if (!ProcessPacket(frontPacket))
				{
					CloseConnection(i, "Failed to process incoming packet.", *this);
					break;
				}
				m_connections[i].pm_incoming.Pop();
			}
		}
	}

	


	

	
}
