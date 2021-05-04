#ifndef GAME_H
#define GAME_H

#include "State.h"
#include "Core.h"

#include "Terrain.h"

class Game : public State {
public:
	Game(Core* core);

	bool handle_input();
	void update();
	void draw();
private:
	Core* _core;

	std::unique_ptr<Terrain> _terrain;
};

#endif