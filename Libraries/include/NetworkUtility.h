#pragma once
#include "IncludeMe.h"
#include <Server.h>


enum class ConditionStrategy : uint8_t
{
	ST_FALSE, ST_TRUE, ST_CONTINUE, ST_BREAK
};

void Network::CloseConnection(int connectionIndex, std::string&& reason, Network::Server& server);
void Network::OnConnect(TCPConnection& newConnection, Network::Server& server);
void Network::OnDisconnect(TCPConnection& lostConnection, std::string&& reason, Network::Server& server);
void Network::CloseConnection(int connectionIndex, std::string&& reason, Network::Server& server);
void ProcessPacketContent(Network::TCPConnection& connection);
ConditionStrategy ProcessPacketSize(Network::TCPConnection& connection, int bytesReceived, int connectionIndex, Network::Server& server, ConditionStrategy cs);
bool Network::ProcessPacket(std::shared_ptr<Packet> packet);