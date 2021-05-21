#ifndef EDITOR_H
#define EDITOR_H

#include "State.h"
#include "Core.h"

#include "Terrain.h"

#include <memory>

#define EDIT_TERRAIN 0 
#define EDIT_TEXTURE 1

class Editor;

/********************************************************************************************************************************************************/

struct EditorWindow {
	EditorWindow(Editor* editor);

	virtual void update() = 0;

	Editor* _editor;
};

/********************************************************************************************************************************************************/

struct BrushWindow : public EditorWindow {
	BrushWindow(Editor* editor);

	void update();

	float _raise_value;

	bool _terrain;
	bool _texture;
	bool _raise;
	bool _set;
	bool _average;
	bool _flatten;
	int  _texture_index;
};

/********************************************************************************************************************************************************/

class Editor : public State {
public:
	Editor(Core* core);
	~Editor();

	void first_frame();
	bool handle_input();
	void update();
	void draw();

	void setup_imgui();

	Core*						_core;
	BrushWindow					_brush_window;
	std::unique_ptr<Terrain>	_terrain;

private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

/********************************************************************************************************************************************************/

#endif