#include <Client.hpp>
#include <iostream>
#include <NetworkUtility.hpp>
#include <string>
#include <thread>


//TODO: remove flag variable and make a function returns bool


Network::Client::Client() = default;

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
				SetSocketStatus(m_connection.m_socket, m_master_fd);
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
		std::string status;
		ReventsError(m_use_fd, status);
		if (!status.empty())
		{
			CloseConnection(std::move(status));
			return false;
		}

		if (m_use_fd.revents & POLLRDNORM) //If normal data can be read without blocking
		{
			int bytesReceived = ReceiveData(m_connection, m_use_fd);
			ReceivedBytesError(bytesReceived, status);
			if (!status.empty())
			{
				CloseConnection(std::move(status));
				return false;
			}
			if (bytesReceived > 0)
			{
				m_connection.pm_incoming.currentPacketExtractionOffset += bytesReceived;
				if (m_connection.pm_incoming.m_currentTask == Network::PacketTask::ProcessPacketSize)
				{
					if (!ProcessPacketSize(m_connection))
					{
						CloseConnection("Packet size too large.");
						return false;
					}
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
					if (!SendSizeData(pm, m_use_fd))
						break;
				}
				else //Sending packet contents
				{
					if (!SendContentData(pm, m_use_fd))
						break;
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
		std::shared_ptr<Packet> sender;
		if (!ProcessPacket(m_connection, frontPacket))
		{
			CloseConnection("Failed to process incoming packet.");
			return false;
		}
		m_connection.pm_incoming.Pop();
	}
	return true;
}

void Network::Client::ChatFrame()
{
	std::string text;
	std::shared_ptr<Packet> message = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	std::getline(std::cin, text);
	*message << text;
	m_connection.pm_outgoing.Append(message);
}

bool Network::Client::ProcessPacket(TCPConnection& connected, std::shared_ptr<Packet> packet)
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

void Network::Client::CloseConnection(std::string&& reason)
{
	OnDisconnect(reason);
	m_master_fd.fd = 0;
	m_isConnected = false;
	m_connection.Close();
}