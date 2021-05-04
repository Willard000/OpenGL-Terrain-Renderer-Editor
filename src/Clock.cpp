#include "Clock.h"

#include <Windows.h>
#include <GLFW/glfw3.h>
#include <ctime>

Clock::Clock(const int fps) :
	_limit(fps),
	_ms(1000.0 / _limit),
	_is_limit(_limit > 0 ? true : false),
	_frames(0.0),
	_time(0.0),
	_update_ticks(0),
	_fms(0.0),
	_ticks(0.0),
	_previous_ticks(glfwGetTime())
{}

bool Clock::update(const double interval) {
	update_time();
	_frames++;

	if (_is_limit && (_time < _ms)) {
		const double delay = _ms - _time;
		if (delay > 0.0) {
			Sleep(DWORD(delay));
			update_time();
		}
	}

	if (_previous_ticks >= _update_ticks) {
		_fms = 1000.0 / _frames;
		_frames = 0;
		_update_ticks += interval;
		return true;
	}

	return false;
}

void Clock::reset() {
	_frames = 0;
	_update_ticks = _previous_ticks + UPDATE_INTERVAL;
	update_time();
}

void Clock::limit(const bool limit) {
	_is_limit = limit;
}

void Clock::set_limit(const int limit) {
	_limit = limit;
	_ms = 1000.0 / _limit;
}

void Clock::update_time() {
	_ticks = glfwGetTime() - _previous_ticks;
	_previous_ticks = glfwGetTime();
	_time = _ticks * .001;
}

double Clock::get_time() const {
	return _time;
}

double Clock::get_fms() const {
	return _fms;
}

// Time format (00:00:00:000)
// days : hours : minutes : seconds : miliseconds
std::string Clock::get_display_time() const {
	double miliseconds = glfwGetTime() * .001;
	miliseconds = miliseconds - floor(miliseconds);
	miliseconds *= 1000.0;

	const int ticks = (int)(glfwGetTime() * .001);
	const int seconds = ticks % 60;
	const int minutes = int(ticks / 60) % 60;
	const int hours = int(ticks / 3600) % 24;
	const int days = int(ticks / 86400);

	std::string time = "";

	time += (days > 0) ? std::to_string(days) + ":" : "";
	time += ((hours < 10) ? "0" : "") + std::to_string(hours) + ":";
	time += ((minutes < 10) ? "0" : "") + std::to_string(minutes) + ":";
	time += ((seconds < 10) ? "0" : "") + std::to_string(seconds) + ":";
	time += std::to_string((int)miliseconds);

	return time;
}

std::string Clock::get_system_time() const {
	time_t sys_time = time(NULL);
	std::string str;
	str.resize(26);
	ctime_s(&str[0], str.size(), &sys_time);

	str.resize(str.find('\n'));

	return str;
}