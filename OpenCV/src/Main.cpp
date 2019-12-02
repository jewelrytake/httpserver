#include <iostream>
#include "opencv2\opencv.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include "config.h"
#include <vector>

using namespace cv;

Mat cameraFeed;
SOCKET SendSocket = INVALID_SOCKET;
sockaddr_in RecvAddr;
int height, width, IM_HEIGHT, IM_WIDTH, imgSize;

WSADATA wsaData;
void Server()
{
	int iResult;
	SendSocket = INVALID_SOCKET;
	unsigned short Port = 7200;
	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
	}

	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET)
	{
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
	}
	
	RecvAddr.sin_port = htons(Port);
	RecvAddr.sin_family = AF_INET;
	const char* ip = "127.0.0.1";
	inet_pton(AF_INET, ip, &RecvAddr.sin_addr.S_un.S_addr);

}

int main()
{
	Server();
	int jpegqual = ENCODE_QUALITY; // Compression Parameter
	Mat frame, send;
	std::vector < uchar > encoded;
	VideoCapture cap("C:\\Users\\perso\\Downloads\\vid.mp4");

	namedWindow("send", WINDOW_AUTOSIZE);
	if (!cap.isOpened()) {
		std::cerr << "OpenCV Failed to open camera";
		exit(1);
	}
	while (true)
	{
		cap >> frame;

		if (frame.size().width == 0)
			continue;//simple integrity check; skip erroneous data...
		resize(frame, send, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);

		std::vector < int > compression_params;
		compression_params.push_back(IMWRITE_JPEG_QUALITY);
		compression_params.push_back(jpegqual);

		imencode(".jpg", send, encoded, compression_params);
		imshow("send", send);
		int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;
		int ibuf[1];
		ibuf[0] = total_pack;
		int iResult = 0;
		iResult = sendto(SendSocket, (char*)ibuf, sizeof(int), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
		for (int i = 0; i < total_pack; i++)
		{
			iResult = sendto(SendSocket, (char*)&encoded[i * PACK_SIZE], PACK_SIZE, 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
			if (iResult == -1)
			{
				int res = WSAGetLastError();
				std::cout << "gg wp\n";
			}
			waitKey(FRAME_INTERVAL);
		}
	}

	return 0;
}