#pragma once
#ifndef include_LIGHTINGANDGRAPHICS
#define include_LIGHTINGANDGRAPHICS


#include <vector>

#include "Systems\Game.hpp"
#include "Primitives\Element.hpp"
#include "Rendering\Renderer.hpp"
//#include "FontRenderer.hpp"
#include "Console\ConsoleLog.hpp"
#include "Console\Console.hpp"

class Player;

class LightingAndGraphics : public Game
{
public:
	LightingAndGraphics();


	std::vector<Element*> m_elements;
	Player* m_player;
	Renderer m_renderer;
	float m_internalTime;
	bool m_isQuitting;
	bool m_displayConsole;
	//FontRenderer m_fontRenderer;
	//ConsoleLog m_consoleLog;
	Console m_console;

	virtual void update(float deltaTime);
	virtual void render();
	virtual bool keyDownEvent(unsigned char asKey);
	virtual bool characterEvent(unsigned char asKey, unsigned char scanCode);
	virtual bool typingEvent(unsigned char asKey);
	virtual bool mouseEvent(unsigned int eventType){return false;};

	//HACK
	void mouseUpdate();
};



#endif 

