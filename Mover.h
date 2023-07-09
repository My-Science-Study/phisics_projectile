#pragma once

#include <core.h>
#include "particle.h"

class Mover
{
public:
	Mover();
	~Mover();

	cyclone::Vector3 m_position;
	cyclone::Particle* m_particle;
	float size;

	void CheckEdge();
	void update(float duration);
	void stop();
	void draw(int shadow);
};

