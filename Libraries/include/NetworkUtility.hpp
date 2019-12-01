#pragma once
#include "Include.hpp"

void DecomposeInt_32(char* buf, int32_t val);
int32_t ComposeInt_32(char* buf);
void SetSocketStatus(Network::TCPSocket& connected, WSAPOLLFD& connectedFD);
void ReventsError(WSAPOLLFD& fd, std::string& status);
int ReceiveData(Network::TCPConnection& connection, WSAPOLLFD& use_fd);
void ReceivedBytesError(int receivedBytes, std::string& status);
bool ProcessPacketSize(Network::TCPConnection& connection);
void ProcessPacketContent(Network::TCPConnection& connection);
bool SendSizeData(Network::PacketManager& pm, WSAPOLLFD& use_fd);
bool SendContentData(Network::PacketManager& pm, WSAPOLLFD& use_fd);




