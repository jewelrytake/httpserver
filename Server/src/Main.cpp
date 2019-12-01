#include <iostream>
#include <Include.hpp>
#include <TCPServer.hpp>

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		return TRUE;
	case CTRL_C_EVENT:
		return TRUE;
	default:
		return FALSE;
	}
}


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	if (Network::Initialize())
	{
		SetConsoleCtrlHandler(CtrlHandler, TRUE);
		Network::TCPServer server;
		if (server.Start(Network::IPAddress("192.168.0.104", 8080)))
		{
			while (true)
			{
				server.Frame();
			}
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}