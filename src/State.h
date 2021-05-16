#ifndef STATE_H
#define STATE_H

class State {
public:
	State() : _close	( false )	{}

	virtual void first_frame() = 0;
	virtual bool handle_input() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;

	bool _close;
};

#endif