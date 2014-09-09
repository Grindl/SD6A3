#include "Rendering\OpenGLFunctions.hpp"
#include "Rendering\Texture.hpp"
#include "Debug Graphics\DebugGraphics.hpp"
#include "Systems\Player.hpp"
#include "Rendering\Skybox.hpp"
#include "Console\FontRenderer.hpp"
#include "Data.hpp"
#include "Panzerfaust.hpp"
#include "Utility\XMLParser.hpp"
#include "Event System\EventSystemHelper.hpp"
#include "Entity.hpp"
#include "Connection.hpp"
#include "Primitives/Color3b.hpp"
#include "Utility/StringUtility.hpp"
#include "Console\CommandParser.hpp"

bool g_pulse = false;
Vector2f cursorPositionInCamera; 
Vector2f cursorPositionInWorld;
User g_localUser;
Connection* g_serverConnection;
std::vector<User> g_users;
Entity g_flag;

bool ChangeServer(std::string addressAndPort)
{
	std::vector<std::string> serverAddrSplit = StringUtility::StringSplit(addressAndPort, ":", " ");
	g_serverConnection = new Connection(serverAddrSplit[0].c_str(), serverAddrSplit[1].c_str());
	CS6Packet loginPacket;
	loginPacket.packetType = TYPE_Acknowledge;
	loginPacket.packetNumber = 0;
	loginPacket.data.acknowledged.packetType = TYPE_Acknowledge;
	g_serverConnection->sendPacket(loginPacket);
	return g_serverConnection != NULL;
}

bool ChangeColor(std::string rgbaColor)
{
	g_localUser.m_unit.m_color = Color4f(rgbaColor);
	return true;
}


void Panzerfaust::mouseUpdate()
{
	//TODO this is all very WIN32, abstract this in to the IOHandler
	if(m_IOHandler.m_hasFocus)
	{
		LPPOINT cursorPositionOnScreen = new POINT;
		GetCursorPos(cursorPositionOnScreen);
		HWND localClientWindow = m_IOHandler.m_clientWindow;
		ScreenToClient(localClientWindow, cursorPositionOnScreen);
		Vector2f cursorPositionInClient = Vector2f(cursorPositionOnScreen->x, cursorPositionOnScreen->y);
		cursorPositionInClient.y = SCREEN_HEIGHT - cursorPositionInClient.y;
		cursorPositionInCamera = cursorPositionInClient * (m_worldCamera.m_cameraSizeInWorldUnits.x / m_worldCamera.m_screenDimensionsInPixels.x);
		cursorPositionInWorld = cursorPositionInCamera + Vector2f(m_worldCamera.m_position.x, m_worldCamera.m_position.y); // - (m_worldCamera.m_cameraSizeInWorldUnits * .5);
		//TODO figure out what I want to use the mouse for

	}
	glColor3f(1.0f,1.0f,1.0f);
}

Panzerfaust::Panzerfaust()
{
	
	m_renderer = Renderer();
	//debugUnitTest(m_elements);
	m_internalTime = 0.f;
	m_isQuitting = m_renderer.m_fatalError;
	m_console.m_log = ConsoleLog();
	//m_world = World();
	m_worldCamera = Camera();

	m_displayConsole = false;

	//HACK test values
	m_console.m_log.appendLine("This is a test of the emergency broadcast system");
	m_console.m_log.appendLine("Do not be alarmed or concerned");
	m_console.m_log.appendLine("This is only a test");

	UnitTestXMLParser(".\\Data\\UnitTest.xml");
	unitTestEventSystem();
	//g_serverConnection = new Connection("129.119.246.221", "5000");
	g_serverConnection = new Connection("127.0.0.1", "8080");
	g_localUser = User();
	g_localUser.m_unit = Entity();
	g_localUser.m_unit.m_color = Color4f(0.2f, 1.0f, 0.2f, 1.f);
	g_localUser.m_userType = USER_LOCAL;
	g_localUser.m_unit.m_position = Vector2f(0,0);
	g_flag.m_color = Color4f(1.f, 1.f, 1.f, 1.f);
	CommandParser::RegisterCommand("connect", ChangeServer);
	CommandParser::RegisterCommand("color", ChangeColor);
}

void Panzerfaust::update(float deltaTime)
{
	bool forwardVelocity = m_IOHandler.m_keyIsDown['W'];
	bool backwardVelocity = m_IOHandler.m_keyIsDown['S'];
	bool leftwardVelocity = m_IOHandler.m_keyIsDown['A'];
	bool rightwardVelocity = m_IOHandler.m_keyIsDown['D'];

	//HACK
	const float SPEED_OF_CAMERA = 50.f;

	g_localUser.m_unit.m_target.x += (rightwardVelocity - leftwardVelocity)*SPEED_OF_CAMERA*deltaTime;
	g_localUser.m_unit.m_target.y += (forwardVelocity - backwardVelocity)*SPEED_OF_CAMERA*deltaTime;
	g_localUser.update(deltaTime);

	if (g_localUser.m_unit.m_position.distanceSquared(g_flag.m_position) < 100.f)
	{
		CS6Packet vicPacket;
		vicPacket.packetType = TYPE_Victory;
		Color3b userColor = Color3b(g_localUser.m_unit.m_color);
		memcpy(vicPacket.playerColorAndID, &userColor, sizeof(userColor));
		memcpy(vicPacket.data.victorious.playerColorAndID, &userColor, sizeof(userColor));
		g_serverConnection->sendPacket(vicPacket);
	}

	CS6Packet currentPacket;
	do 
	{
		bool newUser = true;
		currentPacket = g_serverConnection->receivePackets();
		if (currentPacket.packetType == TYPE_Reset)
		{
			//Reset type things
			g_localUser.m_unit.m_position = Vector2f(currentPacket.data.reset.playerXPosition, currentPacket.data.reset.playerYPosition);
			g_localUser.m_unit.m_target = g_localUser.m_unit.m_position;
			Color3b packetColor = Color3b();
			memcpy(&packetColor, currentPacket.playerColorAndID, sizeof(packetColor));
			g_localUser.m_unit.m_color = Color4f(packetColor.r/255.f, packetColor.g/255.f, packetColor.b/255.f, 1.f);
			g_flag.m_position = Vector2f(currentPacket.data.reset.flagXPosition, currentPacket.data.reset.flagYPosition);
			g_flag.m_target = g_flag.m_position;
			g_localUser.m_isInGame = true;

		}
		else if (currentPacket.packetType != 0)
		{
			Color3b packetColor = Color3b();
			memcpy(&packetColor, currentPacket.playerColorAndID, sizeof(packetColor));

			for (unsigned int ii = 0; ii < g_users.size(); ii++)
			{
				
				if (Color3b(g_users[ii].m_unit.m_color) == packetColor)
				{
					newUser = false;
					g_users[ii].processUpdatePacket(currentPacket);
				}
			}
			if (newUser)
			{
				User tempUser = User();
				tempUser.processUpdatePacket(currentPacket);
				//tempUser.m_unit.m_position = Vector2f(currentPacket.x, currentPacket.y);
				//tempUser.m_unit.m_target = Vector2f(currentPacket.x, currentPacket.y);
				//tempUser.m_unit.m_color = Color4f(packetColor.r/255.f, packetColor.g/255.f, packetColor.b/255.f, 1.f);
				tempUser.m_userType = USER_REMOTE;
				g_users.push_back(tempUser);
			}
		}
	} while (currentPacket.packetType != 0);

	for (unsigned int ii = 0; ii < g_users.size(); ii++)
	{
		g_users[ii].update(deltaTime);
	}

	mouseUpdate();

	m_internalTime += deltaTime;

	//m_world.update(deltaTime);
}

void Panzerfaust::render()
{
	m_renderer.m_singleFrameBuffer.preRenderStep();

	m_worldCamera.preRenderStep();

	//m_world.render(m_playerController.m_possessedActor);

	g_localUser.render();
	g_flag.render();

	for (unsigned int ii = 0; ii < g_users.size(); ii++)
	{
		g_users[ii].render();
	}

	glUseProgram(m_renderer.m_shaderID);

	float crossOffset = 0.3f;
	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3.0f);
	glBegin(GL_LINES);

	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(cursorPositionInWorld.x+crossOffset,cursorPositionInWorld.y+crossOffset,0.0f);
	glVertex3f(cursorPositionInWorld.x-crossOffset,cursorPositionInWorld.y-crossOffset,0.0f);
	glVertex3f(cursorPositionInWorld.x+crossOffset,cursorPositionInWorld.y-crossOffset,0.0f);
	glVertex3f(cursorPositionInWorld.x-crossOffset,cursorPositionInWorld.y+crossOffset,0.0f);

	glEnd();

	m_worldCamera.postRenderStep();

	m_renderer.m_singleFrameBuffer.postRenderStep();


	if(m_displayConsole)
	{
		m_console.render();
	}
}

bool Panzerfaust::keyDownEvent(unsigned char asKey)
{
	
	if (asKey == VK_OEM_3)
	{
		m_displayConsole = !m_displayConsole;
	}
	else if(!m_displayConsole)
	{
		m_IOHandler.KeyDownEvent(asKey);
	}
	else
	{
		if(asKey == VK_LEFT)
		{
			if(m_console.m_currentTextOffset > 0)
			{
				m_console.m_currentTextOffset--;
			}		
		}

		else if (asKey == VK_RIGHT)
		{
			if(m_console.m_currentTextOffset < m_console.m_command.size())
			{
				m_console.m_currentTextOffset++;
			}
		}
	}
	return true;
}

bool Panzerfaust::characterEvent(unsigned char asKey, unsigned char scanCode)
{
	return false;
}

bool Panzerfaust::typingEvent(unsigned char asKey)
{
	if (m_displayConsole && asKey != '`' && asKey != '~')
	{
		if(asKey == '\n' || asKey == '\r')
		{
			m_console.executeCommand();
		}
		else if (asKey == '\b')
		{
			if(m_console.m_currentTextOffset > 0)
			{
				m_console.m_command.erase(m_console.m_command.begin()+(m_console.m_currentTextOffset-1));
				m_console.m_currentTextOffset--;
			}
			
		}
		//HACK reject all other non-printable characters
		else if(asKey > 31)
		{
			m_console.insertCharacterIntoCommand(asKey);
		}
		
		return true;
	}
	else
	{
		return false;
	}
}

bool Panzerfaust::mouseEvent(unsigned int eventType)
{
	switch(eventType)
	{
	case WM_LBUTTONDOWN:
		{
			//m_playerController.pathToLocation(cursorPositionInWorld);
			break;
		}
	case WM_RBUTTONDOWN:
		{
			//m_playerController.attackTarget(cursorPositionInWorld);
			break;
		}
	}

	return true;
}
