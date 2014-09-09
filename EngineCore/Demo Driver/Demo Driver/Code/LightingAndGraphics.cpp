#include "Rendering\OpenGLFunctions.hpp"
#include "Rendering\Texture.hpp"
#include "Debug Graphics\DebugGraphics.hpp"
#include "Systems\Player.hpp"
#include "Rendering\Skybox.hpp"
#include "Console\FontRenderer.hpp"
#include "Data.hpp"
#include "LightingAndGraphics.hpp"
#include "Utility\XMLParser.hpp"
#include "Event System\EventSystemHelper.hpp"
#include "Importer\MaxImporter.hpp"

bool g_pulse = false;
std::string g_multiPolyExportFile = ".\\Data\\IYAMATEST.CHUMBA";
int g_sceneIter = 0;
int g_locOfMultiPoly = 0;
static const int NUMSCENES = 6;
std::vector<std::string> g_allExplortedSceneFiles = std::vector<std::string>();

void debugUnitTest(std::vector<Element*>& temporarySolution)
{
	//DebugGraphics::point(Vector3f(), Color4f(), 1.f);
	//Element* theLine = new DebugLine(Vector3f(), COLOR::RED, Vector3f(1, 1, 1), COLOR::GREY, 10.f, true);
	//temporarySolution.push_back(theLine);
	//g_thePoint = new DebugPoint(Vector3f(-1, -1, 1), COLOR::WHITE, 15000.f, true);
	//((DebugPoint*)g_thePoint)->m_material.m_useShaderProgram = true;
	//temporarySolution.push_back(g_thePoint);
	Texture* textureCatalog = new Texture(".\\Data\\cobblestonesDiffuse.png");
	Texture* normalMap = new Texture(".\\Data\\cobblestonesNormal.png");
	Texture* specularMap = new Texture(".\\Data\\cobblestonesSpecular.png");
	Texture* emissiveMap = new Texture(".\\Data\\cobblestonesEmissive.png");
	Element* theBox = new DebugAABB3(Vector3f(-1, -1, -1), COLOR::WHITE, Vector3f(1, 1, 1), COLOR::GREY, 200.f, true);
	((DebugAABB3*)theBox)->m_material.m_diffuseID = textureCatalog->m_openGLTextureID;
	((DebugAABB3*)theBox)->m_material.m_useDiffuse = true;
	((DebugAABB3*)theBox)->m_material.m_normalID = normalMap->m_openGLTextureID;
	((DebugAABB3*)theBox)->m_material.m_useNormal = true;
	((DebugAABB3*)theBox)->m_material.m_useVBO = true;
	((DebugAABB3*)theBox)->m_material.m_specularID = specularMap->m_openGLTextureID;
	((DebugAABB3*)theBox)->m_material.m_useSpecular = true;
	((DebugAABB3*)theBox)->m_material.m_emissiveID = emissiveMap->m_openGLTextureID;
	((DebugAABB3*)theBox)->m_material.m_useEmissive = true;
	temporarySolution.push_back(theBox);

	//Element* theSphere = new DebugSphere(Vector3f(-3, -3, -1), 2, COLOR::GREEN, COLOR::BLUE, 20.f, true);
	//temporarySolution.push_back(theSphere);
	//Element* theArrow = new DebugArrow(Vector3f(-1, 1, -1), COLOR::GREY, Vector3f(-1, 0, 1), COLOR::RED, 19.f, false);
	//temporarySolution.push_back(theArrow);
	g_allExplortedSceneFiles.resize(NUMSCENES);
	g_allExplortedSceneFiles[0] = ".\\Data\\Final1THREE.CHUMBA";//".\\Data\\MATRIIIX.CHUMBA";
	g_allExplortedSceneFiles[1] = ".\\Data\\Final2THREE.CHUMBA";
	g_allExplortedSceneFiles[2] = ".\\Data\\Final3THREE.CHUMBA";
	g_allExplortedSceneFiles[3] = ".\\Data\\Final4THREE.CHUMBA";
	g_allExplortedSceneFiles[4] = ".\\Data\\Final5TWO.CHUMBA";
	g_allExplortedSceneFiles[5] = ".\\Data\\Final6THREE.CHUMBA";

	//g_allExplortedSceneFiles[0] = g_multiPolyExportFile;//".\\Data\\singlepoly.CHUMBA";
	//g_allExplortedSceneFiles[1] = ".\\Data\\boxes.CHUMBA";
	//g_allExplortedSceneFiles[2] = ".\\Data\\obj.CHUMBA";
	//g_allExplortedSceneFiles[3] = ".\\Data\\skinnedtube.CHUMBA";
	//g_allExplortedSceneFiles[4] = ".\\Data\\zax_walking.CHUMBA";
	//g_allExplortedSceneFiles[5] = ".\\Data\\NormalBump.CHUMBA";
	//g_allExplortedSceneFiles[6] = ".\\Data\\boy.CHUMBA";
	//g_allExplortedSceneFiles[7] = ".\\Data\\boy-turbosmooth.CHUMBA";
	//g_allExplortedSceneFiles[8] = ".\\Data\\default-material.CHUMBA";
	//g_allExplortedSceneFiles[9] = ".\\Data\\color-lights.CHUMBA";
	//g_allExplortedSceneFiles[10] = ".\\Data\\direct-light.CHUMBA";
	//g_allExplortedSceneFiles[11] = ".\\Data\\sibenik.CHUMBA";
	//g_allExplortedSceneFiles[12] = ".\\Data\\camera-animation.CHUMBA";

	Element* theMultiPoly = MaxImporter::ImportFrom3DSMaxToDebugMultiPoly(g_allExplortedSceneFiles[0]);
	((DebugMultiPoly*)theMultiPoly)->m_material.m_diffuseID = textureCatalog->m_openGLTextureID;
	((DebugMultiPoly*)theMultiPoly)->m_material.m_useDiffuse = true;
	((DebugMultiPoly*)theMultiPoly)->m_material.m_normalID = normalMap->m_openGLTextureID;
	((DebugMultiPoly*)theMultiPoly)->m_material.m_useNormal = true;
	((DebugMultiPoly*)theMultiPoly)->m_material.m_useVBO = true;
	//((DebugMultiPoly*)theMultiPoly)->m_material.m_specularID = specularMap->m_openGLTextureID;
	//((DebugMultiPoly*)theMultiPoly)->m_material.m_useSpecular = true;
	//((DebugMultiPoly*)theMultiPoly)->m_material.m_emissiveID = emissiveMap->m_openGLTextureID;
	//((DebugMultiPoly*)theMultiPoly)->m_material.m_useEmissive = true;
	g_locOfMultiPoly = temporarySolution.size();
	temporarySolution.push_back(theMultiPoly);
}

void renderAxis(float lenght)
{
	//Use Red, Green, Blue for X Y Z respectively
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	//X axis
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(lenght,0.0f,0.0f);
	//Y axis
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,lenght,0.0f);
	//Z axis
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(0.0f,0.0f,0.0f);
	glVertex3f(0.0f,0.0f,lenght);
	glEnd();
}

void LightingAndGraphics::mouseUpdate()
{
	if(m_IOHandler.m_hasFocus)
	{
		POINT p;
		GetCursorPos(&p);
		int x = 200 - p.x;
		int y = 200 - p.y;
		m_player->m_orientation[0] += x*.15f;
		m_player->m_orientation[1] += y*.15f;
		SetCursorPos(200, 200);
	}
}

LightingAndGraphics::LightingAndGraphics()
{
	//TODO delete the player later
	m_player = new Player();
	m_renderer = Renderer();
	debugUnitTest(m_elements);
	m_internalTime = 0.f;
	m_isQuitting = m_renderer.m_fatalError;
	m_console.m_log = ConsoleLog();
	m_displayConsole = false;

	//HACK test values
	m_console.m_log.appendLine("This is a test of the emergency broadcast system");
	m_console.m_log.appendLine("Do not be alarmed or concerned");
	m_console.m_log.appendLine("This is only a test");
	Light* primaryLight = new Light(LOCAL_AMBIENTLIGHT);
	primaryLight->m_origin = Vector3f(0,0,2);
	primaryLight->m_firstColor = Color4f(1,0,0,1);
	primaryLight->genVBO();
	m_elements.push_back(primaryLight);
	m_renderer.m_lightManager.m_lights[0] = primaryLight;
	Light* secondaryLight = new Light(LOCAL_POINTLIGHT);
	secondaryLight->m_origin = Vector3f(0,0,2);
	secondaryLight->m_firstColor = Color4f(1,1,1,1);
	secondaryLight->m_movingLight = true;
	secondaryLight->m_innerRadius = 70.f;
	secondaryLight->m_outerRadius = 500.f;
	secondaryLight->m_pointSize = 20.f;
	secondaryLight->genVBO();
	m_elements.push_back(secondaryLight);
	m_renderer.m_lightManager.m_lights[1] = secondaryLight;
	Light* light2 = new Light(LOCAL_SPOTLIGHT);
	light2->m_origin = Vector3f(0,0,2);
	light2->m_firstColor = Color4f(0,0,1,1);
	light2->genVBO();
	m_elements.push_back(light2);
	m_renderer.m_lightManager.m_lights[2] = light2;
	Light* light3 = new Light(GLOBAL_AMBIENTLIGHT);
	light3->m_origin = Vector3f(0,0,2);
	light3->m_firstColor = Color4f(1,1,1,.6f);
	light3->genVBO();
	m_elements.push_back(light3);
	m_renderer.m_lightManager.m_lights[3] = light3;

	UnitTestXMLParser(".\\Data\\UnitTest.xml");
}

void LightingAndGraphics::update(float deltaTime)
{
	bool forwardVelocity = m_IOHandler.m_keyIsDown['W'];
	bool backwardVelocity = m_IOHandler.m_keyIsDown['S'];
	bool leftwardVelocity = m_IOHandler.m_keyIsDown['A'];
	bool rightwardVelocity = m_IOHandler.m_keyIsDown['D'];

	g_pulse = m_IOHandler.m_keyIsDown['G'];



	//HACK
	const float PI = 3.15159f;
	const float SPEED_OF_CAMERA = 40.f;

	m_player->m_velocity[0] = (cos(m_player->m_orientation[0]*(PI/180.f))*(rightwardVelocity - leftwardVelocity)-sin(m_player->m_orientation[0]*(PI/180.f))*(forwardVelocity - backwardVelocity))*SPEED_OF_CAMERA;
	m_player->m_velocity[1] = (sin(m_player->m_orientation[0]*(PI/180.f))*(rightwardVelocity - leftwardVelocity)+cos(m_player->m_orientation[0]*(PI/180.f))*(forwardVelocity - backwardVelocity))*SPEED_OF_CAMERA;
	m_player->flyVertical((m_IOHandler.m_keyIsDown[VK_SHIFT] - m_IOHandler.m_keyIsDown[VK_CONTROL])*SPEED_OF_CAMERA);

	m_player->m_orientation[0] += (m_IOHandler.m_keyIsDown[VK_LEFT] - m_IOHandler.m_keyIsDown[VK_RIGHT])*180.f*deltaTime;
	m_player->m_orientation[1] += (m_IOHandler.m_keyIsDown[VK_UP] - m_IOHandler.m_keyIsDown[VK_DOWN])*180.f*deltaTime; 

	mouseUpdate();

	if(m_player->m_orientation[1] > 89.f)
	{
		m_player->m_orientation[1] = 89.f;
	}
	else if(m_player->m_orientation[1] < -89.f)
	{
		m_player->m_orientation[1] = -89.f;
	}

	m_internalTime += deltaTime;

	m_player->update(deltaTime);

	//((DebugElement*)g_thePoint)->m_origin = Vector3f(2*sin(m_internalTime/1.0f), 2.f, 2*sin(m_internalTime/2.0f));
	//((DebugElement*)g_thePoint)->m_origin = Vector3f(-2, -2, 2.f);

	for (unsigned int ii = 0; ii < m_elements.size(); ii++)
	{
		m_elements[ii]->update(deltaTime);
		//HACK
		((DebugElement*)m_elements[ii])->m_shaderID = m_renderer.m_shaderID;
		((DebugElement*)m_elements[ii])->m_material.m_lightBufferID = m_renderer.m_lightManager.m_lightBufferID;
		((DebugElement*)m_elements[ii])->m_material.m_cameraPosition = m_player->m_position;
		if(m_elements[ii]->shouldBeRemoved())
		{
			Element* temp = m_elements[ii];
			m_elements.erase(m_elements.begin()+ii);
			delete temp;
			ii--;
		}
	}
}

void LightingAndGraphics::render()
{
	m_renderer.m_singleFrameBuffer.preRenderStep();
	
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f );
	glClearDepth(1.f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//g_skybox.render();
	
	
	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(true);
	

	//int shaderTime = glGetUniformLocation(m_renderer.m_shaderID, "u_time");
	//if(g_pulse)
	//{
	//	glUniform1f(shaderTime, m_internalTime);
	//}
	//else
	//{
	//	glUniform1f(shaderTime, 0.f);
	//}

	//HACK
	const float PI = 3.15159f;


	glPushMatrix();

	gluPerspective(40.0, (16.f/9.f), 0.1, 500.0);
	glRotatef(-90.f, 1.f, 0.f, 0.f);
	glRotatef(-1.f*m_player->m_orientation[0], 0.f, 0.f, 1.f);
	glRotatef(-1.f*m_player->m_orientation[1], cos(m_player->m_orientation[0]*(PI/180.f)), sin(m_player->m_orientation[0]*(PI/180.f)), 0.f);

	glTranslatef(-1.f*m_player->m_position[0], -1.f*m_player->m_position[1], -1.f*m_player->m_position[2]);

	//TODO get the font to have an up vector as well as forward vector
	//m_fontRenderer.drawString(Vector3f(0,0,0), "Hello World");

	glUseProgram(m_renderer.m_shaderID);

	for (unsigned int ii = 0; ii < m_elements.size(); ii++)
	{
		//HACK fix this ASAP
		m_renderer.m_lightManager.updateLightsInOpenGL(((DebugElement*)m_elements[ii])->m_material.m_shaderID);
		m_elements[ii]->render();
	}

	if(m_IOHandler.m_keyIsDown['O'])
	{
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		renderAxis(5.f);
	}

	glPopMatrix();

	m_renderer.m_singleFrameBuffer.postRenderStep();

	if(m_displayConsole)
	{
		m_console.render();
	}
	

}

bool LightingAndGraphics::keyDownEvent(unsigned char asKey)
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
		if(asKey == VK_PRIOR)
		{
			Texture* textureCatalog = Texture::CreateOrGetTexture(".\\Data\\cobblestonesDiffuse.png");
			Texture* normalMap = Texture::CreateOrGetTexture(".\\Data\\cobblestonesNormal.png");
			g_sceneIter--;
			if (g_sceneIter < 0)
			{
				g_sceneIter += NUMSCENES;
			}
			Element* theMultiPoly = MaxImporter::ImportFrom3DSMaxToDebugMultiPoly(g_allExplortedSceneFiles[g_sceneIter]);
			((DebugMultiPoly*)theMultiPoly)->m_material.m_diffuseID = textureCatalog->m_openGLTextureID;
			((DebugMultiPoly*)theMultiPoly)->m_material.m_useDiffuse = true;
			((DebugMultiPoly*)theMultiPoly)->m_material.m_normalID = normalMap->m_openGLTextureID;
			((DebugMultiPoly*)theMultiPoly)->m_material.m_useNormal = true;
			((DebugMultiPoly*)theMultiPoly)->m_material.m_useVBO = true;
			m_elements[g_locOfMultiPoly] = theMultiPoly; //HACK causes memory leak
		}
		else if(asKey == VK_NEXT)
		{
			Texture* textureCatalog = Texture::CreateOrGetTexture(".\\Data\\cobblestonesDiffuse.png");
			Texture* normalMap = Texture::CreateOrGetTexture(".\\Data\\cobblestonesNormal.png");
			g_sceneIter++;
			if (g_sceneIter >= NUMSCENES)
			{
				g_sceneIter -= NUMSCENES;
			}
			Element* theMultiPoly = MaxImporter::ImportFrom3DSMaxToDebugMultiPoly(g_allExplortedSceneFiles[g_sceneIter]);
			((DebugMultiPoly*)theMultiPoly)->m_material.m_diffuseID = textureCatalog->m_openGLTextureID;
			((DebugMultiPoly*)theMultiPoly)->m_material.m_useDiffuse = true;
			((DebugMultiPoly*)theMultiPoly)->m_material.m_normalID = normalMap->m_openGLTextureID;
			((DebugMultiPoly*)theMultiPoly)->m_material.m_useNormal = true;
			((DebugMultiPoly*)theMultiPoly)->m_material.m_useVBO = true;
			m_elements[g_locOfMultiPoly] = theMultiPoly; //HACK causes memory leak
		}
	
	return true;
}

bool LightingAndGraphics::characterEvent(unsigned char asKey, unsigned char scanCode)
{
	/*if(m_displayConsole)
	{
		//TODO fully understand LPWSTR

		LPWSTR trueCharacter = new short[10]; 
		ToUnicode(asKey, scanCode, 0, trueCharacter, 1, 0);
		return true;
	}
	else
	{
		return false;
	}*/
	return false;
}

bool LightingAndGraphics::typingEvent(unsigned char asKey)
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