#include <Initializer.hpp>
#include <Server.hpp>
#include <iostream>
#include "NetworkUtility.hpp"

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
				//TODO: use SetSocketStatus
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
					WSAPOLLFD connectedFD;
					SetSocketStatus(connectedSocket, connectedFD);
					m_master_fd.emplace_back(connectedFD);
					TCPConnection& acceptedConnection = m_connections.back();
					OnConnect(acceptedConnection);
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
				std::string status;
				ReventsError(m_use_fd[i], status);
				if (!status.empty())
				{
					CloseConnection(connectionIndex, std::move(status));
					continue;
				}
#pragma endregion End of ERROR_CHECKING

#pragma region Read data from a client
				if (m_use_fd[i].revents & POLLRDNORM) //If normal data can be read without blocking from a client
				{
					int bytesReceived = ReceiveData(connection, m_use_fd[i]);
					ReceivedBytesError(bytesReceived, status);
					if (!status.empty())
					{
						CloseConnection(connectionIndex, std::move(status));
						continue;
					}
					if (bytesReceived > 0)
					{
						connection.pm_incoming.currentPacketExtractionOffset += bytesReceived;
						if (connection.pm_incoming.m_currentTask == Network::PacketTask::ProcessPacketSize)
						{
							uint8_t flag = 0;
							ProcessPacketSize(connection, flag);
							if (flag == 1)
							{
								CloseConnection(connectionIndex, "Packet size too large."); 
								continue;
							}
						}
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
					int flag;
					PacketManager& pm = connection.pm_outgoing;
					while (pm.HasPendingPackets())
					{
						if (pm.m_currentTask == PacketTask::ProcessPacketSize)
						{
							flag = 0;
							SendSizeData(pm, m_use_fd[i], flag);
							if (flag == 1)
								break;
						}
						else
						{
							flag = 0;
							SendContentData(pm, m_use_fd[i], flag);
							if (flag == 1)
								break;
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

#pragma region Process incoming packets
		for (int i = m_connections.size() - 1; i >= 0; i--)
		{
			while (m_connections[i].pm_incoming.HasPendingPackets())
			{
				std::shared_ptr<Packet> frontPacket = m_connections[i].pm_incoming.GetCurrentPacket();
				if (!ProcessPacket(frontPacket))
				{
					CloseConnection(i, "Failed to process incoming packet.");
					break;
				}
				m_connections[i].pm_incoming.Pop();
			}
		}
#pragma endregion End of process incoming packets
	}	
	void Server::OnConnect(TCPConnection& newConnection)
	{
		std::cout << newConnection.ToString() << " - New connection accepted.\n";
	}

	void Server::OnDisconnect(TCPConnection& lostConnection, std::string&& reason)
	{
		std::cout << "[" << reason << "] Connection lost: " << lostConnection.ToString() << ".\n";
	}

	void Server::CloseConnection(int connectionIndex, std::string&& reason)
	{
		TCPConnection& connection = m_connections[connectionIndex];
		OnDisconnect(connection, std::move(reason));
		m_master_fd.erase(m_master_fd.begin() + (connectionIndex + 1));
		m_use_fd.erase(m_use_fd.begin() + (connectionIndex + 1));
		connection.Close();
		m_connections.erase(m_connections.begin() + connectionIndex);
	}

	bool Server::ProcessPacket(std::shared_ptr<Packet> packet)
	{
		std::cout << "Packet received with size: " << packet->m_buffer.size() << '\n';
		return true;
	}
}
