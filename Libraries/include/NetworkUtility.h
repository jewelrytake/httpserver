#pragma once
#include "IncludeMe.h"
#include <Server.h>

#define CONTINUE 999
#define OK 100

enum class ConditionStrategy : uint8_t
{
	ST_FALSE, ST_TRUE, ST_CONTINUE, ST_BREAK, ST_NONE
};
void SetSocketStatus(Network::TCPSocket& connected, WSAPOLLFD& connectedFD);
void DecomposeInt_32(unsigned char* buf, int32_t val);
int32_t ComposeInt_32(unsigned char* buf);
void ProcessPacketContent(Network::TCPConnection& connection);
void ProcessPacketSize(Network::TCPConnection& connection, uint8_t& conditionFlag);
void ReventsError(WSAPOLLFD& fd, std::string& status);
void ReceivedBytesError(int receivedBytes, std::string& status);
int ReceiveData(Network::TCPConnection& connection, WSAPOLLFD& use_fd);
void SendSizeData(Network::PacketManager& pm, WSAPOLLFD& use_fd, int& flag);
void SendContentData(Network::PacketManager& pm, WSAPOLLFD& use_fd, int& flag);




