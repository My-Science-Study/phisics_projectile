
#include <iostream>
#include "Mover.h"
#include <gl/glut.h>
#include "timing.h"


Mover::Mover()
{
	size = 2.0f;
	m_particle = new cyclone::Particle();

	//artillery
	m_particle->setPosition(5, 20, 0);
	m_particle->setVelocity(0, 30.0f, 40.0f);
	m_particle->setMass(200.0f);
	m_particle->setDamping(0.99f);
	m_particle->setAcceleration(0.0f,-20.0f,0.0f);
}


Mover::~Mover()
{
}

void Mover::CheckEdge()
{
	cyclone::Vector3 position;
	cyclone::Vector3 velocity;
	m_particle->getPosition(&position);
	m_particle->getVelocity(&velocity);

	if (position.y < size) {
		m_particle->setPosition(position.x,size,position.z);
		m_particle->setVelocity(velocity.x,velocity.y*-1,velocity.z);	
		
	}
	else if (position.x < size || position.x >= 100) {
		m_particle->setPosition(100-size, position.y, position.z);
		m_particle->setVelocity(velocity.x * -1, velocity.y, velocity.z);
	}
	
}

void Mover::update(float duration)
{
	//cyclone::Vector3 wind(1.0f, 0, 0);
	//m_particle->addForce(wind);

	m_particle->integrate(duration);

	//CheckEdge();
	/*
	duration *= 5.0f;
	cyclone::Vector3 d = cyclone::Vector3(1, 0, 0) * duration;
	m_position += d;

	if (m_position.x > 100) m_position.x = 0;*/
}

void Mover::stop()
{

	TimingData::get().deinit();

}
void Mover::draw(int shadow)
{
	cyclone::Vector3 position;
	m_particle->getPosition(&position);

	if (shadow) {
		glColor3f(0,0,0);
	}
	else {
		glColor3f(1.0f, 0,0 );
	}


	glPushMatrix();
	//glTranslatef(0,1,0);
	glTranslatef(position.x, position.y, position.z);
	glutSolidSphere(size, 30, 30);
	glPopMatrix();

}