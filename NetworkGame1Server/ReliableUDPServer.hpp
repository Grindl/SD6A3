//UNUSED

#pragma once
#ifndef include_RELIABLEUDPSERVER
#define include_RELIABLEUDPSERVER

//#include "ServerConnection.hpp"

#include <map>




class ReliableUDPServer
{
public:
	ReliableUDPServer();

	std::map<GamePacket, int> m_unconfirmedPackets;

	void sendPacket(GamePacket pk);
	void receiveAck(GamePacket pk);

};

#endif