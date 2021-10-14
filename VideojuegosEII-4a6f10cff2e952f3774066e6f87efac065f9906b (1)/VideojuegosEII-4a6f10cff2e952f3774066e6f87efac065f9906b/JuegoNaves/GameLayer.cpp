#include "GameLayer.h"

GameLayer::GameLayer(Game* game)
	: Layer(game) {
	//llama al constructor del padre : Layer(renderer)
	init();
}

void GameLayer::init() {
	points = 0;
	
	player = new Player(50, 100, game);
	textPoints = new Text("0", WIDTH * 0.92, HEIGHT * 0.05, game);
	textPoints->content = to_string(points);

	textShots = new Text("0", WIDTH * 0.15, HEIGHT * 0.9, game);
	textShots->content = to_string(player-> shots);

	textLifes = new Text("0", WIDTH * 0.07, HEIGHT * 0.1, game);
	textLifes->content = to_string(player->lifes);

	textInvulnerable = new Text("Invulnerable", WIDTH * 0.30, HEIGHT * 0.1, game); 

	background = new Background("res/fondo.png", WIDTH * 0.5, HEIGHT * 0.5, game);
	//Laposición es relativa a la pantalla, en este caso el 85% del aancho y el 5% del alto
	backgroundPoints = new Actor("res/icono_puntos.png",
		WIDTH * 0.85, HEIGHT * 0.05, 24, 24, game);
	backgroundLifes = new Actor("res/corazon.png",
		WIDTH * 0.07, HEIGHT * 0.1, 50, 40, game);
	backgroundShots = new Actor("res/power-up_UI.png",
		WIDTH * 0.05, HEIGHT * 0.9, 16, 41, game);

	projectiles.clear(); // Vaciar por si reiniciamos el juego
	enemies.clear(); // Vaciar por si reiniciamos el juego
	enemies.push_back(new Enemy(300, 50, game));
	enemies.push_back(new Enemy(300, 200, game));

}

void GameLayer::processControls() {
	// obtener controles
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		keysToControls(event);
	}
	//procesar controles
	// Disparar
	if (controlShoot) {
		Projectile* newProjectile = player->shoot();
		actualizarDisparos();
		
		if (newProjectile != NULL) {
			projectiles.push_back(newProjectile);
		}

	}

	// Eje X
	if (controlMoveX > 0) {
		player->moveX(1);
	}
	else if (controlMoveX < 0) {
		player->moveX(-1);
	}
	else {
		player->moveX(0);
	}

	// Eje Y
	if (controlMoveY > 0) {
		player->moveY(1);
	}
	else if (controlMoveY < 0) {
		player->moveY(-1);
	}
	else {
		player->moveY(0);
	}

}

void GameLayer::keysToControls(SDL_Event event) {
	//Cerrar pulsando el aspa
	if (event.type == SDL_QUIT) {
		game->loopActive = false;
	}

	if (event.type == SDL_KEYDOWN) {
		int code = event.key.keysym.sym;
		float posx = player->x;
		float posy = player->y;
		int l = player->lifes;
		int s = player->shots;
		// Pulsada
		switch (code) {
		case SDLK_ESCAPE:
			game->loopActive = false;
			break;
		case SDLK_3: //Escalar la pantalla usando la tecla 3
			game->scale();
			break;
		case SDLK_1:
			player = new Player("res/jugador.png", posx, posy, game);
			player->shots = s;
			player->lifes = l;
			break;
		case SDLK_2:
			player = new Player("res/jugador-verde.png", posx, posy, game);
			player->shots = s;
			player->lifes = l;
			break;
		case SDLK_d: // derecha
			controlMoveX = 1;
			break;
		case SDLK_a: // izquierda
			controlMoveX = -1;
			break;
		case SDLK_w: // arriba
			controlMoveY = -1;
			break;
		case SDLK_s: // abajo
			controlMoveY = 1;
			break;
		case SDLK_SPACE: // dispara
			controlShoot = true;
			break;
		}

	}
	if (event.type == SDL_KEYUP) {
		int code = event.key.keysym.sym;
		// Levantada
		switch (code) {
		case SDLK_d: // derecha
			if (controlMoveX == 1) {
				controlMoveX = 0;
			}
			break;
		case SDLK_a: // izquierda
			if (controlMoveX == -1) {
				controlMoveX = 0;
			}
			break;
		case SDLK_w: // arriba
			if (controlMoveY == -1) {
				controlMoveY = 0;
			}
			break;
		case SDLK_s: // abajo
			if (controlMoveY == 1) {
				controlMoveY = 0;
			}
			break;
		case SDLK_SPACE: // dispara
			controlShoot = false;
			break;
		}

	}

}


void GameLayer::update() {

	// Generar enemigos
	newEnemyTime--;
	if (newEnemyTime <= 0) {
		int rX = (rand() % (600 - 500)) + 1 + 500;
		int rY = (rand() % (300 - 60)) + 1 + 60;
		enemies.push_back(new Enemy(rX, rY, game));
		newEnemyTime = 110;
	}

	// Generar powerUp
	newBulletTime--;
	if (newBulletTime == 0) {
		float x = (rand() % 100) + 300;
		float y = (rand() % 240);
		if(powerUpBullets.size() < 1)
			powerUpBullets.push_back(new PowerUpBullet(x, y, game));
		newBulletTime = 500;
	}
	player->update();
	for (auto const& enemy : enemies) {
		enemy->update();
	}
	
	// Colisiones
	for (auto const& enemy : enemies) {
		if (player->isOverlap(enemy)) {
			player->loseLife();
			textLifes->content = to_string(player->lifes);

			if(player -> invulTime == 100)
				enemies.remove(enemy);
			draw();
			if (player->estado == Player::State::DEAD)
				init();
			return; // Cortar el for
		}
	}

	for (auto const& pu : powerUpBullets) {
		if (player->isOverlap(pu)) {
			player->addShots(10);
			actualizarDisparos();
			powerUpBullets.clear();
			return;
		}
	}
	for (auto const& projectile : projectiles) {
		projectile->update();
	}

	// Colisiones , Enemy - Projectile

	list<Enemy*> deleteEnemies;
	list<Projectile*> deleteProjectiles;

	//Elimina los proyectiles si se salen de la pantalla
	for (auto const& projectile : projectiles) {
		if (projectile->isInRender() == false) {

			bool pInList = std::find(deleteProjectiles.begin(),
				deleteProjectiles.end(),
				projectile) != deleteProjectiles.end();

			if (!pInList) {
				deleteProjectiles.push_back(projectile);
			}
		}
	}

	for (auto const& enemy : enemies) {
		for (auto const& projectile : projectiles) {
			if (enemy->isOverlap(projectile)) {
				player->addShots(1);
				actualizarDisparos();
				bool pInList = std::find(deleteProjectiles.begin(),
					deleteProjectiles.end(),
					projectile) != deleteProjectiles.end();

				if (!pInList) {
					deleteProjectiles.push_back(projectile);
				}

				bool eInList = std::find(deleteEnemies.begin(),
					deleteEnemies.end(),
					enemy) != deleteEnemies.end();

				if (!eInList) {
					deleteEnemies.push_back(enemy);
					points++;
					textPoints->content = to_string(points);
				}
				
			}
		}
	}

	for (auto const& delEnemy : deleteEnemies) {
		enemies.remove(delEnemy);
	}
	deleteEnemies.clear();

	for (auto const& delProjectile : deleteProjectiles) {
		projectiles.remove(delProjectile);
	}
	deleteProjectiles.clear();
}

void GameLayer::draw() {
	background->draw();

	for (auto const& projectile : projectiles) {
		projectile->draw();
	}


	player->draw();

	for (auto const& enemy : enemies) {
		enemy->draw();
	}

	for (auto const& pu : powerUpBullets) {
		pu->draw();
	}
	if(player-> invulTime != 0)
		textInvulnerable->draw(0, 255, 0, 255);
	textPoints->draw(255,233,0,255);
	backgroundPoints->draw();

	backgroundLifes->draw();
	textLifes->draw(255,255,255,255);

	backgroundShots->draw();
	textShots->draw(227, 255, 0, 255);
	
	SDL_RenderPresent(game->renderer); // Renderiza
}

void GameLayer::actualizarDisparos() {
	textShots->content = to_string(player->shots);
}