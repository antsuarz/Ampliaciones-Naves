#pragma once

#include "Actor.h"
#include "Projectile.h" 

class Player : public Actor
{
public:
	Player(float x, float y, Game* game);
	Player(string fichero, float x, float y, Game* game);
	void update();
	void moveX(float axis);
	void moveY(float axis); 
	Projectile* shoot();
	int shootCadence = 30;
	int shootTime = 0;
	int lifes = 3;
	int invulTime = 0;
	void loseLife();
	enum class Tipo {ROJO, VERDE};
	Tipo tipo;
	enum class State {ALIVE, DEAD};
	State estado;
};

