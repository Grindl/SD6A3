#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <gl/gl.h>
//#include <gl/glu.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>

#include "Primitives/Color3b.hpp"
#include "Systems/Time Utility.hpp"

#include "RemoteUDPClient.hpp"

#pragma comment(lib, "Ws2_32.lib")

//#include "ReliableUDPServer.hpp"





struct sockaddr_in g_serverSockAddr;
SOCKET g_Socket;
std::vector<RemoteUDPClient> g_clients;
bool g_gameOver;
Vector2f g_flagPos;
//std::map<CS6Packet, int> g_unconfirmedPackets;

void startServer(const char* IPAddress, const char* port)
{
	g_gameOver = false;
	initializeTimeUtility();
	g_flagPos = Vector2f (rand()%500, rand()%500);
	WSAData myWSAData;
	int WSAResult;
	g_Socket = INVALID_SOCKET;
	//hostent* HostName =  gethostbyname("smu.gametheorylabs.com");
	//IPAddress = HostName->h_addr_list[0];

	long IPAsLong = inet_addr(IPAddress);
	u_long fionbioFlag = 1;

	g_serverSockAddr.sin_family = AF_INET;
	g_serverSockAddr.sin_port = htons(atoi(port));
	g_serverSockAddr.sin_addr.s_addr = INADDR_ANY;

	WSAResult = WSAStartup(MAKEWORD(2,2), &myWSAData);
	g_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	WSAResult = ioctlsocket(g_Socket, FIONBIO, &fionbioFlag);
	WSAResult = bind(g_Socket, (struct sockaddr*)&g_serverSockAddr, sizeof(g_serverSockAddr));
}

CS6Packet receive()
{
	int WSAResult;
	CS6Packet pk;
	pk.packetType = 0;
	RemoteUDPClient tempClient;
	int recvSize = sizeof(tempClient.m_sockAddr);
	bool newClient = true;
	pk.packetType = 0;

	WSAResult = recvfrom(g_Socket, (char*)&pk, sizeof(pk), 0, (sockaddr*)&(tempClient.m_sockAddr), &recvSize);
	//tempClient.m_sockAddr.sin_addr.s_addr = htonl(tempClient.m_sockAddr.sin_addr.s_addr);
	if (WSAResult != -1)
	{
		for (unsigned int ii = 0; ii < g_clients.size(); ii++)
		{
			if (tempClient == g_clients[ii])
			{
				//update the time
				newClient = false;
				double currentTime = getCurrentTimeSeconds();
				g_clients[ii].m_timeOfLastReport = currentTime;
				g_clients[ii].m_unprocessedPackets.push_back(pk);
			}
		}
		if (newClient)
		{
			std::cout<<"New client connected\n";
			//give a time
			double currentTime = getCurrentTimeSeconds();
			tempClient.m_timeOfLastReport = currentTime;
			tempClient.m_unprocessedPackets.push_back(pk);
			g_clients.push_back(tempClient);
		}
	}
	else
	{
		WSAResult = WSAGetLastError();
		int BREAKNOW = 0;
	}

	return pk;
}

void sendToAllClients(CS6Packet pk)
{
	int WSAResult;
	for (unsigned int ii = 0; ii < g_clients.size(); ii++)
	{
		WSAResult = sendto(g_Socket, (char*)&pk, sizeof(pk), 0, (const sockaddr*)&(g_clients[ii].m_sockAddr), sizeof(g_clients[ii].m_sockAddr));
	}
}

int main()
{
	startServer("127.0.0.1", "8080");
	while(true)
	{
		CS6Packet currentPacket;
		do 
		{
			currentPacket = receive();
		} while (currentPacket.packetType != 0);

		
		std::vector<CS6Packet> positionUpdatePackets;
		for (unsigned int i = 0; i < g_clients.size(); i++)
		{
			//check for timeout
			if (g_clients[i].timedOut())
			{
				g_clients.erase(g_clients.begin()+i);
				i--;
			}
			else
			{
				//else process all their pending packets and put their new position in the queue			
				g_clients[i].processUnprocessedPackets();
				positionUpdatePackets.push_back(g_clients[i].packForSend());
				//also check to see if they have declared victory and propagate
				if (g_clients[i].m_isDeclaringVictory)
				{
					std::cout<<"Victory\n";
					//victory stuff
					g_gameOver = true;
					CS6Packet vicPacket;
					vicPacket.packetType = TYPE_Victory;
					Color3b cleanedColor = Color3b(g_clients[i].m_unit.m_color);
					memcpy(vicPacket.playerColorAndID, &cleanedColor, sizeof(cleanedColor));
					memcpy(vicPacket.data.victorious.playerColorAndID, &cleanedColor, sizeof(cleanedColor));
					positionUpdatePackets.push_back(vicPacket);
				}
			}
		}
		if(g_gameOver)
		{
			g_flagPos = Vector2f (rand()%500, rand()%500);
		}
		for (unsigned int i = 0; i < g_clients.size(); i++)
		{
			if (g_gameOver)
			{
				CS6Packet newGamePacket;
				g_clients[i].m_unit.m_position = Vector2f (rand()%500, rand()%500);

				//send them a reset
				newGamePacket.packetType = TYPE_Reset;
				Color3b cleanedColor = Color3b(g_clients[i].m_unit.m_color);
				memcpy(newGamePacket.playerColorAndID, &cleanedColor, sizeof(cleanedColor));
				memcpy(newGamePacket.data.reset.playerColorAndID, &cleanedColor, sizeof(cleanedColor));
				newGamePacket.data.reset.flagXPosition = g_flagPos.x;
				newGamePacket.data.reset.flagYPosition = g_flagPos.y;
				newGamePacket.data.reset.playerXPosition = g_clients[i].m_unit.m_position.x;
				newGamePacket.data.reset.playerYPosition = g_clients[i].m_unit.m_position.y;
				g_clients[i].m_nonAckedGuaranteedPackets.push_back(newGamePacket);
				g_clients[i].m_pendingPacketsToSend.push_back(newGamePacket);
				g_clients[i].m_isDeclaringVictory = false;
			}
			g_clients[i].m_pendingPacketsToSend.insert(g_clients[i].m_pendingPacketsToSend.end(), positionUpdatePackets.begin(), positionUpdatePackets.end());
			//g_clients[i].m_pendingPacketsToSend.insert(g_clients[i].m_pendingPacketsToSend.end(), g_clients[i].m_nonAckedGuaranteedPackets.begin(), g_clients[i].m_nonAckedGuaranteedPackets.end());
			g_clients[i].sendAllPendingPackets();
		}
		if(g_gameOver)
		{
			g_gameOver = false;
		}
		
		//if (currentPacket.packetType !=0)
		//{
		//	sendToAllClients(currentPacket);
		//}

		Sleep(50);
	}

	return 0;
}