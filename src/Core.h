#ifndef CORE_H
#define CORE_H

#include <memory>

class Clock;
class Window;
class Camera;
class StateManager;
class ShaderManager;

struct Core {
	std::unique_ptr<Clock> _clock;
	std::unique_ptr<Window> _window;
	std::unique_ptr<Camera> _camera;
	std::unique_ptr<StateManager> _state_manager;
	std::unique_ptr<ShaderManager> _shader_manager;
};

#endif