#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "State.h"

#include <memory>
#include <vector>

typedef std::vector<std::unique_ptr<State>> StateStack;

class StateManager {
public:

	void add(std::unique_ptr<State>&& state);

	void execute(std::unique_ptr<State>& state);

	void run();

	void pop();
private:
	StateStack _states;
};

#endif