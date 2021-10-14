#include "Player.h"

Player::Player(float x, float y, Game* game)
	: Actor("res/jugador.png", x, y, 50, 57, game) {
	tipo = Tipo::ROJO;
}

Player::Player(string fichero, float x, float y, Game* game)
	: Actor(fichero, x, y, 50, 57, game) {
	if (fichero == "res/jugador.png")
		tipo = Tipo::ROJO;
	else
		tipo = Tipo::VERDE;
}

void Player::update() {
	if (shootTime > 0) {
		shootTime--;
	}

	x = x + vx;
	y = y + vy;
}

void Player::moveX(float axis) {
	if (tipo == Tipo::ROJO)
		vx = axis * 3;
	else
		vx = axis * 5;
}

void Player::moveY(float axis) {
	if (tipo == Tipo::ROJO)
		vy = axis * 3;
	else
		vy = axis * 5;
}

Projectile* Player::shoot() {
	if (shootTime == 0) {
		if(tipo == Tipo::ROJO)
			shootTime = shootCadence;
		else
			shootTime = 15;
		return new Projectile(x, y, game);
	}
	else {
		return NULL;
	}

}

