#include "User.hpp"
#include "Connection.hpp"
#include "Primitives/Color3b.hpp"

User::User()
{
	m_isInGame = false;
}

CS6Packet User::sendInput()
{
	if (m_userType == USER_REMOTE || !m_isInGame)
	{
		return CS6Packet();
	}
	else
	{
		Color3b tempColor = Color3b(m_unit.m_color);
		CS6Packet outPacket;// = {2, tempColor.r, tempColor.g, tempColor.b, m_unit.m_position.x, m_unit.m_position.y};//TODO turn this into a real packet
		outPacket.packetType = TYPE_Update;
		memcpy(outPacket.playerColorAndID, (char*)&tempColor, sizeof(tempColor));
		outPacket.data.updated.xPosition = m_unit.m_position.x;
		outPacket.data.updated.yPosition = m_unit.m_position.y;
		outPacket.data.updated.xVelocity = m_unit.m_velocity.x;
		outPacket.data.updated.yVelocity = m_unit.m_velocity.y;
		outPacket.data.updated.yawDegrees = m_unit.m_orientationDegrees;
		g_serverConnection->sendPacket(outPacket);
		return outPacket;
	}
}

void User::processUpdatePacket(CS6Packet newData)
{
	//TODO TODO this
	switch(newData.packetType)
	{
	case 0:
		{
			//BAD
			break;
		}
	case TYPE_Acknowledge:
		{
			//TODO remove the pending packet waiting for ack
			break;
		}
	case TYPE_Update:
		{
			m_unit.m_position.x = newData.data.updated.xPosition;
			m_unit.m_position.y = newData.data.updated.yPosition;
			m_unit.m_velocity.x = newData.data.updated.xVelocity;
			m_unit.m_velocity.y = newData.data.updated.yVelocity;
			m_unit.m_orientationDegrees = newData.data.updated.yawDegrees;
			break;
		}
	}
}

void User::update(float deltaSeconds)
{
	m_unit.update(deltaSeconds);
	sendInput();
}

void User::render()
{
	m_unit.render();
}