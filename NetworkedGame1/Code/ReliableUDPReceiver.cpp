#include "ReliableUDPReceiver.hpp"

ReliableUDPReceiver::ReliableUDPReceiver()
{
	m_connectionToUse = new Connection("0", "0");//TODO feed this in
	m_nextPacketNumToProcess = 0;
}

void ReliableUDPReceiver::checkForNewPackets()
{
	CS6Packet currentPacket = m_connectionToUse->receivePackets();
	m_unprocessedPackets.push_back(currentPacket);
	sendAckForPacket(currentPacket);
}

void ReliableUDPReceiver::sendAckForPacket(CS6Packet pkToAck)
{
	pkToAck.packetType = TYPE_Acknowledge;
	m_connectionToUse->sendPacket(pkToAck);
}

void ReliableUDPReceiver::processPackets()
{
	int currentPacketNum = 0;
	//TODO sort
	do
	{
		currentPacketNum = m_unprocessedPackets.back().packetNumber;
		if (currentPacketNum == m_nextPacketNumToProcess)
		{
			//TODO process this packet
			m_unprocessedPackets.pop_back();
			currentPacketNum++;
		}
		else if(currentPacketNum < m_nextPacketNumToProcess)
		{
			m_unprocessedPackets.pop_back();//discard it
		}
	} while (currentPacketNum == m_nextPacketNumToProcess);
}