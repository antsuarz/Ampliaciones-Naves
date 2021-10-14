#pragma once

#include "Layer.h"
#include "Player.h"
#include "Background.h"
#include "Enemy.h"
#include "Projectile.h"
#include "PowerUpBullet.h"
#include "PowerUpLife.h"
#include "Text.h"

#include <list>


class GameLayer : public Layer
{
public:
	GameLayer(Game* game);
	void init() override;
	void processControls() override;
	void update() override;
	void draw() override;
	void actualizarDisparos();
	void keysToControls(SDL_Event event);
	int newEnemyTime = 0;
	int newBulletTime = 200;
	int newLifeTime = 500;
	Text* textPoints;
	Text* textLifes;
	Text* textInvulnerable;
	Text* textShots;
	int points;
	int lifes;

	Player* player;
	Background* background;
	Actor* backgroundPoints;
	Actor* backgroundLifes;
	Actor* backgroundShots;
	bool controlShoot = false;
	int controlMoveY = 0;
	int controlMoveX = 0;

	list<Enemy*> enemies;
	list<PowerUpBullet*> powerUpBullets;
	list<PowerUpLife*> powerUpLifes;
	list<Projectile*> projectiles;

};

