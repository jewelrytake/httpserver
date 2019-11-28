#pragma once
#include "IncludeMe.h"
#include <Server.h>

enum class ConditionStrategy : uint8_t
{
	ST_FALSE, ST_TRUE, ST_CONTINUE, ST_BREAK, ST_NONE
};

void DecomposeInt_32(unsigned char* buf, int32_t val);
int32_t ComposeInt_32(unsigned char* buf);
void ProcessPacketContent(Network::TCPConnection& connection);
ConditionStrategy ProcessPacketSize(Network::TCPConnection& connection, ConditionStrategy cs);
