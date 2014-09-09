#pragma once
#ifndef include_USER
#define include_USER

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
//#include <stdio.h>
//#include <iostream>
//#include <sstream>
//#include <stdlib.h>
//#include <time.h>
#include "Entity.hpp"
#include "CS6Packet.hpp"

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

enum USER_TYPE{USER_LOCAL, USER_REMOTE};

class User
{
public:
	User();

	Entity m_unit;
	USER_TYPE m_userType;
	bool m_isInGame;

	CS6Packet sendInput();
	//CS6Packet packForSend();
	void processUpdatePacket(CS6Packet newData);
	void update(float deltaSeconds);
	void render();
};

#endif