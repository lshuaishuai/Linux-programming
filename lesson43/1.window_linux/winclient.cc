#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <cstring>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

uint16_t serverPort = 8080;
string serverIp = "42.193.110.107";

int main()
{
	WSAData wsd;
	// 启动WinSock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WASStartup Error = " << WSAGetLastError() << endl;
		return 0;
	}
	else
	{
		cout << "WSAStartup Success" << endl;
	}

	SOCKET csock = socket(AF_INET, SOCK_DGRAM, 0); // IPPROTO_UDP
	if (csock == SOCKET_ERROR)
	{
		cout << "WSAStartip Error = " << WSAGetLastError() << endl;
		return 1;
	}
	else
	{
		cout << "socket success" << endl;
	}

	struct sockaddr_in server;
	memset(&server, 0, sizeof server);
	server.sin_family = AF_INET;
	server.sin_port = htons(serverPort);
	server.sin_addr.s_addr = inet_addr(serverIp.c_str());

	string line;
#define NUM 1024
	char buffer[NUM];
	while (true)
	{
		// 发送逻辑
		cout << "Please Enter# ";
		getline(cin, line);

		int s = sendto(csock, line.c_str(), line.size(), 0, (struct sockaddr*)&server, sizeof(server));
		if (s < 0)
		{
			cerr << "sendto error" << endl;
			break;
		}

		struct sockaddr_in peer;
		int peerlen = sizeof(peer);
		// 收数据
		buffer[s] = 0;
		int n = recvfrom(csock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&peer, &peerlen);
		if (n > 0)
		{
			buffer[n] = 0;
			cout << "server 返回的消息是# " << buffer << endl;
		}
		else break;
	}

	closesocket(csock);
	WSACleanup();
	return 0;
}