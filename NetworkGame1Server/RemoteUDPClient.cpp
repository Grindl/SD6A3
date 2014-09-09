#include <algorithm>
#include <math.h>
#include "Primitives/Color3b.hpp"
#include "Systems/Time Utility.hpp"
#include "RemoteUDPClient.hpp"

extern SOCKET g_Socket;
extern Vector2f g_flagPos;

bool packetComparitor(CS6Packet lhs, CS6Packet rhs)
{
	return lhs.packetNumber > rhs.packetNumber; //intentionally reversed to guarantee the lowest numbered packets are at the back for popping
}

RemoteUDPClient::RemoteUDPClient()
{
	m_isDeclaringVictory = false;
	m_lastRecievedPacketNum = 0;
	m_lastSentPacketNum = 0;
	m_timeOfLastReport = -1.f;

}

CS6Packet RemoteUDPClient::packForSend()
{
	CS6Packet preparedPacket;
	preparedPacket.packetType = TYPE_Update;
	Color3b cleanedColor = Color3b(m_unit.m_color);
	memcpy(preparedPacket.playerColorAndID, &cleanedColor, sizeof(cleanedColor));
	preparedPacket.data.updated.xPosition = m_unit.m_position.x;
	preparedPacket.data.updated.yPosition = m_unit.m_position.y;
	preparedPacket.data.updated.xVelocity = m_unit.m_velocity.x;
	preparedPacket.data.updated.yVelocity = m_unit.m_velocity.y;
	preparedPacket.data.updated.yawDegrees = m_unit.m_orientation;
	return preparedPacket;
}

void RemoteUDPClient::sendAllPendingPackets()
{
	//clean up the pending packets and give them the appropriate identifier and time
	double sendTime = getCurrentTimeSeconds();
	for (unsigned int i = 0; i < m_pendingPacketsToSend.size(); i++)
	{
		m_pendingPacketsToSend[i].packetNumber = m_lastSentPacketNum;
		m_lastSentPacketNum++; //naming is somewhat confusing since it's technically the *next* one to send, but we want to send 0 first
		m_pendingPacketsToSend[i].timestamp = sendTime;
	}
	//send the actual packets
	//TODO confirm global extern socket works
	int WSAResult;
	for (unsigned int ii = 0; ii < m_pendingPacketsToSend.size(); ii++)
	{
		WSAResult = sendto(g_Socket, (char*)&m_pendingPacketsToSend[ii], sizeof(m_pendingPacketsToSend[ii]), 0, (const sockaddr*)&(m_sockAddr), sizeof(m_sockAddr));
	}
	

	//post-send cleanup
	m_pendingPacketsToSend.clear(); //possibly dangerous since sendto isn't blocking
}

bool RemoteUDPClient::timedOut()
{
	bool hasTimedOut = false;
	double currentTime = getCurrentTimeSeconds();
	hasTimedOut = currentTime > m_timeOfLastReport + 10.f; //HACK hardcoded time tolerance
	return hasTimedOut;
}

const bool RemoteUDPClient::operator==(const RemoteUDPClient& rhs) const
{
	bool isEquivalent = true;
	if (m_sockAddr.sin_addr.S_un.S_addr != rhs.m_sockAddr.sin_addr.S_un.S_addr)
	{
		isEquivalent = false;
	}
	if (m_sockAddr.sin_port != rhs.m_sockAddr.sin_port)
	{
		isEquivalent = false;
	}
	//TODO: in the future, a unique user ID may be considered as a way to differentiate clients on the same IP and port,
	//though they may step over each other on the receiving end
	return isEquivalent;
}

void RemoteUDPClient::sortUnprocessed()
{
	std::sort(m_unprocessedPackets.begin(), m_unprocessedPackets.end(), packetComparitor);
}

void RemoteUDPClient::processUnprocessedPackets()
{
	bool hasPacketsLeftToProcess = true;
	sortUnprocessed();
	while(!m_unprocessedPackets.empty() && hasPacketsLeftToProcess)
	{
		CS6Packet currentPacket = m_unprocessedPackets.back();
		
		if (currentPacket.packetNumber == m_lastRecievedPacketNum)
		{
			m_unprocessedPackets.pop_back();
			m_lastRecievedPacketNum = currentPacket.packetNumber;

			switch(currentPacket.packetType)
			{
				case 0:
					{
						//do nothing, invalid packet
						break;
					}
				case TYPE_Acknowledge:
					{
						int ackedPacketNum = currentPacket.data.acknowledged.packetNumber;
						if (currentPacket.data.acknowledged.packetType == TYPE_Acknowledge || currentPacket.data.acknowledged.packetType == 0)
						{
							//prepare them on this end
							m_unit.m_color = Color4f(((float)rand())/RAND_MAX, ((float)rand())/RAND_MAX, ((float)rand())/RAND_MAX, 1.f);
							m_unit.m_position = Vector2f (rand()%500, rand()%500);

							//send them a reset
							CS6Packet resetPacket;
							resetPacket.packetType = TYPE_Reset;
							Color3b cleanedColor = Color3b(m_unit.m_color);
							memcpy(resetPacket.playerColorAndID, &cleanedColor, sizeof(cleanedColor));
							memcpy(resetPacket.data.reset.playerColorAndID, &cleanedColor, sizeof(cleanedColor));
							resetPacket.data.reset.flagXPosition = g_flagPos.x;
							resetPacket.data.reset.flagYPosition = g_flagPos.y;
							resetPacket.data.reset.playerXPosition = m_unit.m_position.x;
							resetPacket.data.reset.playerYPosition = m_unit.m_position.y;
							m_nonAckedGuaranteedPackets.push_back(resetPacket);
							m_pendingPacketsToSend.push_back(resetPacket);
							
							
						}
						else
						{
							for (unsigned int i = 0; i < m_nonAckedGuaranteedPackets.size(); i++)
							{
								//remove acked packets from the list
								if (currentPacket.data.acknowledged.packetNumber == m_nonAckedGuaranteedPackets[i].packetNumber)
								{
									m_nonAckedGuaranteedPackets.erase(m_nonAckedGuaranteedPackets.begin()+i);
									i--; //probably not necessary, but no point in taking chances
								}
							}
						}
						break;
					}
				case TYPE_Update:
					{
						//update the relevant player in our data
						m_unit.m_position.x = currentPacket.data.updated.xPosition;
						m_unit.m_position.y = currentPacket.data.updated.yPosition;
						m_unit.m_velocity.x = currentPacket.data.updated.xVelocity;
						m_unit.m_velocity.y = currentPacket.data.updated.yVelocity;
						m_unit.m_orientation = currentPacket.data.updated.yawDegrees;
						break;
					}
				case TYPE_Victory:
					{
						//send up the chain that the game is over
						m_isDeclaringVictory = true;
						//ack back
						CS6Packet ackBackPacket;
						ackBackPacket.packetType = TYPE_Acknowledge;
						ackBackPacket.data.acknowledged.packetNumber = currentPacket.packetNumber;
						ackBackPacket.data.acknowledged.packetType = currentPacket.packetType;
						m_pendingPacketsToSend.push_back(ackBackPacket);
						break;
					}
			}
		}
		else if (currentPacket.packetNumber < m_lastRecievedPacketNum)
		{
			m_unprocessedPackets.pop_back();
		}
		else
		{
			hasPacketsLeftToProcess = false;
		}
	}
}