#ifndef CLOCK_H
#define CLOCK_H

#include <string>

#define UPDATE_INTERVAL 1

class Clock {
public:
	Clock(const int fps = 144);

	// updates _time 
	// returns true once each interval
	bool update(const double interval = UPDATE_INTERVAL);
	void reset();

	// fps limit
	void limit(const bool limit);
	void set_limit(const int limit);

	double get_time() const;
	double get_fms() const;

	std::string get_display_time() const;
	std::string get_system_time() const;
private:
	int _limit;
	double _frames;
	double _fms, _ms, _time;
	double _ticks, _previous_ticks, _update_ticks;
	bool _is_limit;

	void update_time();
};

#endif