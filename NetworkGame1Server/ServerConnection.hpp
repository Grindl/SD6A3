//UNUSED


#pragma once
#ifndef incude_SERVERCONNECTION
#define incude_SERVERCONNECTION

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <gl/gl.h>
//#include <gl/glu.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


#pragma comment(lib, "Ws2_32.lib")

struct GamePacket
{
	char ID;
	char r, g, b;
	int orderNum;
	double sendTime;
	float x;
	float y;

	const bool operator<(const GamePacket& rhs) const
	{
		return orderNum < rhs.orderNum;
	}
};

struct Client
{
	struct sockaddr_in m_sockAddr;
	float m_timeOfLastReport;
};

class ServerConnection
{
public:
	ServerConnection();

	SOCKET m_Socket;
	struct sockaddr_in g_serverSockAddr;

	GamePacket receivePacket();
	void sendPacket(GamePacket pk, Client sendTarget);
};

#endif