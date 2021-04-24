#include "StateManager.h"

void StateManager::add(std::unique_ptr<State>&& state) {
	_states.push_back(std::move(state));
}

void StateManager::execute(std::unique_ptr<State>& state) {

}

void StateManager::run() {
	bool exit = false;
	while(!_states.empty() && !exit) {
		auto& current_state = _states.back();

		while (!current_state->_close && !exit) {
			exit = current_state->handle_input();

			current_state->update();

			current_state->draw();
		}
	}
}

void StateManager::pop() {
	_states.pop_back();
}