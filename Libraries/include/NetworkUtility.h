#pragma once
#include "IncludeMe.h"
#include <Server.h>


enum class ConditionStrategy : uint8_t
{
	ST_FALSE, ST_TRUE, ST_CONTINUE, ST_BREAK
};

void ProcessPacketContent(Network::TCPConnection& connection);
ConditionStrategy ProcessPacketSize(Network::TCPConnection& connection, int bytesReceived, int connectionIndex, Network::Server& server, ConditionStrategy cs);
