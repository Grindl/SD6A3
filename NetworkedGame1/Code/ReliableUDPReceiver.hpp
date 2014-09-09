#pragma once
#ifndef include_RELIABLEUDPRECEIVER
#define include_RELIABLEUDPRECEIVER

#include <queue>
#include "Connection.hpp"
#include "CS6Packet.hpp"
#include "User.hpp"
//#include "ReliablePacket.hpp"

class ReliableUDPReceiver
{
public:
	ReliableUDPReceiver();

	std::vector<CS6Packet> m_unprocessedPackets;
	std::vector<CS6Packet> m_nonAckedGuaranteedPackets;
	Connection* m_connectionToUse;
	int m_nextPacketNumToProcess;
	int m_nextPacketNumToSend;
	User m_userData;

	void checkForNewPackets();
	void sendAckForPacket(CS6Packet pkToAck);
	void processPackets();
	void sortUnprocessed();
};

#endif