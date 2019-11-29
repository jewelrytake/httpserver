#pragma once
#include "TCPConnection.h"

namespace Network
{
	struct Client
	{
		bool Connect(IPAddress ip);
		bool IsConnected();
		bool Frame();
		void Send(std::shared_ptr<Packet> packet);
		void Receive(std::shared_ptr<Packet> packet);
	protected:
		virtual bool ProcessPacket(std::shared_ptr<Packet> packet);
		virtual void OnConnect();
		virtual void OnConnectFail();
		virtual void OnDisconnect(std::string reason);
		void CloseConnection(std::string&& reason);
		TCPConnection m_connection;

	private:
		bool m_isConnected = false;
		WSAPOLLFD m_master_fd{};
		WSAPOLLFD m_use_fd{};
	};


}
