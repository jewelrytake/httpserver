#include <Initializer.hpp>
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

bool Network::Initialize()
{
	WSADATA wsadata;
	int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (result != 0)
	{
		return false;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
	{
		return false;
	}
	return true;
}

void Network::Shutdown()
{
	WSACleanup();
}
