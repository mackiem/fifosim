#pragma once
#include <queue>

extern "C" {
	double uniform(double *);
	double expon(double *, float);
}

using DelayQueue = std::vector < double > ;
using QueueSizeQueue = std::vector < std::pair<int, double > > ;

enum EventType {
	ARRIVAL = 0,
	END_OF_SERVICE = 1
};

struct Event {
	EventType type;
	double time;
	//double arrival_time_;
	Event(EventType type_, double time_);
	//void set_arrival_time(double arrival_time);
	bool operator<(const Event& other) const;
};

class Handler {
};

class Server {
	bool idle;
public:
	Server();
	void set_idle(bool idle);
	bool is_idle() const;
};

struct StatStructs {
	DelayQueue delay;
	QueueSizeQueue queue_size;
};

class FIFOSim
{
	double mu_;
	double lambda_;
	int max_;
	int start_measuring_from_iteration_;
	int count_;

	DelayQueue delay_;
	QueueSizeQueue queue_size_;
	double seed_;
	std::priority_queue<Event> calendar_;
	std::queue<Event> queue_;

	void ScheduleEvent(const Event& e);
	Event GenArrivalEvent(double clock);
	Event GenEndOfSeviceEvent(double clock, Event& arrival_event);

public:
	FIFOSim(double mu, double lambda, int max, int start_measuring_from_iteration, double seed)
		: mu_(mu),
		  lambda_(lambda),
		  max_(max),
		  start_measuring_from_iteration_(start_measuring_from_iteration),
		  seed_(seed), count_(0) {
	}
	StatStructs simulate();
	~FIFOSim();
};

