#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Entity.hpp"



int Entity::s_nextAssignableID = 0;

//----------------------------------------------------------------------------------------
Entity::Entity()
{
	m_entityID = s_nextAssignableID;
	s_nextAssignableID++;
	m_target = Vector2f(0,0);
}

//----------------------------------------------------------------------------------------
void Entity::update(float deltaSeconds)
{
	Vector2f velocity = m_target - m_position;
	if (!(velocity == Vector2f(0,0)))
	{
		velocity = velocity.normalized();
		m_position = m_position + (velocity * deltaSeconds * 50.f);
	}
	
}

//----------------------------------------------------------------------------------------
void Entity::render()
{
	glPushMatrix();
	glTranslatef(m_position.x, m_position.y, 0.f);
	glScalef(10.f, 10.f, 0.f);

	glColor4f(m_color.red, m_color.green, m_color.blue, 1.f);
	glBegin(GL_QUADS);
	{
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.5f, -0.5f);
		glVertex2f(0.5f, 0.5f);
		glVertex2f(-0.5f, 0.5f);
	}
	glEnd();

	glPopMatrix();
}
