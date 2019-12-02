#include <iostream>
#include "opencv2\opencv.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include "config.h"

using namespace cv;

#define FRAME_WIDTH		640
#define FRAME_HEIGHT	480

int imgSize;
int bytes = 0;
bool running = true;
char key;

int main()
{
	int iResult = 0;

	WSADATA wsaData;

	SOCKET RecvSocket;
	sockaddr_in RecvAddr;

	unsigned short Port = 7200;

	char RecvBuf[1024];
	int BufLen = 1024;

	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error %d\n", iResult);
		return 1;
	}

	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (RecvSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error %d\n", WSAGetLastError());
		return 1;
	}
	
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = bind(RecvSocket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (iResult != 0) {
		wprintf(L"bind failed with error %d\n", WSAGetLastError());
		return 1;
	}
#define BUF_LEN 65540 
	int recvMsgSize = 0;
	char buffer[BUF_LEN];
	while (running)
	{
		do
		{
			recvMsgSize = recvfrom(RecvSocket, buffer, BUF_LEN, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
		}
		while (recvMsgSize > sizeof(int));

		int total_pack = ((int*)buffer)[0];
		std::cout << "expecting length of packs:" << total_pack << '\n';
		char* longbuf = new char[PACK_SIZE * total_pack];
		for (int i = 0; i < total_pack; i++) {
			recvMsgSize = recvfrom(RecvSocket, buffer, BUF_LEN, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
			if (recvMsgSize != PACK_SIZE)
			{
				std::cerr << "Received unexpected size pack:" << recvMsgSize << '\n';
				continue;
			}
			memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
		}
		std::cout << "Received packet from ANY.\n";

		Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
		Mat frame = imdecode(rawData, IMREAD_COLOR);

		if (frame.size().width == 0)
		{
			std::cerr << "decode failure!\n";
			continue;
		}
		imshow("recv", frame);
		delete[] longbuf;
		waitKey(1);
	}
	return 0;
}
