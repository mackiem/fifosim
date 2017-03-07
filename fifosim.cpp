#include "fifosim.h"
#include <iostream>
#include <random>
#include <fstream>
#include <unordered_map>


//FIFOSim::FIFOSim()
//{
//  seed_ = 1234567; //set the seed only once, at the start.
//}


FIFOSim::~FIFOSim()
{
}

Event::Event(EventType type_, double time_) :  type(type_), time(time_) {
}

//void Event::set_arrival_time(double arrival_time) {
//	arrival_time_ = arrival_time;
//}

bool Event::operator<(const Event& other) const {
	return time > other.time;
}

Server::Server() : idle(true) {
}

void Server::set_idle(bool idle_) {
	idle = idle_;
}

bool Server::is_idle() const {
	return idle;
}

void FIFOSim::ScheduleEvent(const Event& e) {
	calendar_.push(e);
}

Event FIFOSim::GenArrivalEvent(double clock) {
	float mean = 1.f/lambda_;
	double arrival_time = clock + expon(&seed_, mean);
	Event e(EventType::ARRIVAL, arrival_time);
	return e;
}

Event FIFOSim::GenEndOfSeviceEvent(double clock, Event& arrival_event) {
	float mean = 1.f/mu_;
	double end_of_service_time = clock + expon(&seed_, mean);
	Event e(EventType::END_OF_SERVICE, end_of_service_time);
	double delay = end_of_service_time - arrival_event.time;
	if (count_ > start_measuring_from_iteration_) {
		delay_.push_back(delay);
	}
	return e;
}

StatStructs FIFOSim::simulate() {

	count_ = 0;
	//int max_ = 10;
	delay_.reserve(max_);

	// seed simulation
	// 1 . set server idle
	bool done = false;
	Server server;
	server.set_idle(true);


	double clock = 0;

	// generate first event
	ScheduleEvent(GenArrivalEvent(clock));

	// start loop
	while (!done) {
		if (calendar_.empty()) {
			std::cout << "Calendar empty. \n";
		}

		Event e = calendar_.top();
		calendar_.pop();
		double prev_clock = clock;
		clock = e.time;

		switch (e.type) {
		case ARRIVAL: {
			// set arrival time 
			//e.set_arrival_time(clock);

			// generate next arrival
			ScheduleEvent(GenArrivalEvent(clock));
			if (server.is_idle()) {
				ScheduleEvent(GenEndOfSeviceEvent(clock, e));
				server.set_idle(false);
			} else {
				queue_.push(e);
			}
			break;
		}

		case END_OF_SERVICE: {
			count_++;
			if (queue_.empty()) {
				server.set_idle(true);
			} else {
				Event arrival_event = queue_.front();
				queue_.pop();
				ScheduleEvent(GenEndOfSeviceEvent(clock, arrival_event));
			}
			break;
		}

		default: {
			break;
		}

		}
		if (count_ > start_measuring_from_iteration_) {
			queue_size_.push_back(std::make_pair(queue_.size() + 1, clock));
		}
		if (count_ > max_) {
			done = true;
		}
	}

	StatStructs structs;
	structs.queue_size = queue_size_;
	structs.delay = delay_;

	return structs;
}

void recurrence_validation() {
}

int main(int argc, char** argv) {

//	/*
	double mu = 1.0;
	std::vector<double> lambdas;

	for (int i = 0; i < 10; ++i) {
		double val = 0.05 + i * 0.1;
		lambdas.push_back(val);
	}

	std::random_device rd;
	std::mt19937 eng(rd());

	std::uniform_int_distribution<int> dist(1000000, 9999999);
	//int sim_iterations = 10;

	int no_of_events = 2100;
	int measure_after_event = 100;

	int no_of_iterations = 10;

	std::ofstream file("stats.csv");
	file << "rho" << "delay" << "queuelen" << "\n";

	for (auto& lambda : lambdas) {

		std::vector<double> expected_delays;
		std::vector<double> expected_queue_lengths;

		double avg_expected_delay = 0.0;
		double avg_expected_queue_length = 0.0;

		double rho = lambda / mu;

		for (int k = 0; k < no_of_iterations; ++k) {

			FIFOSim sim(mu, lambda, no_of_events, measure_after_event, dist(eng));
			auto stat = sim.simulate();

			// avg delay
			double expected_delay = 0.0;
			for (auto & delay : stat.delay) {
				expected_delay += delay;
			}
			expected_delay /= stat.delay.size();

			auto& queue_size = stat.queue_size;
			double expected_queue_length = 0.0;
			for (auto i = 0; i < queue_size.size() - 1; ++i) {
				double dt = queue_size[i + 1].second - queue_size[i].second;
				int q = queue_size[i].first;
				expected_queue_length += q * dt;
			}
			double total_time = queue_size[queue_size.size() - 1].second - queue_size[0].second;

			expected_queue_length /= total_time;

			std::cout << rho << ", " << k << ", " << expected_delay << ", " << expected_queue_length << "\n";

			expected_delays.push_back(expected_delay);
			expected_queue_lengths.push_back(expected_queue_length);
			avg_expected_delay += expected_delay;
			avg_expected_queue_length += expected_queue_length;

		}
		avg_expected_delay /= no_of_iterations;
		avg_expected_queue_length /= no_of_iterations;

		double variance_expected_delay = 0.0;
		// std::deviation
		for (auto& delay : expected_delays) {
			variance_expected_delay += std::pow(delay - avg_expected_delay, 2);
		}
		double std_deviation_expected_delay = std::sqrt(variance_expected_delay / no_of_iterations);

		double variance_expected_queue_length = 0.0;
		// std::deviation
		for (auto& queue_length : expected_queue_lengths) {
			variance_expected_queue_length += std::pow(queue_length - avg_expected_queue_length, 2);
		}
		double std_deviation_expected_queue_length = std::sqrt(variance_expected_queue_length / no_of_iterations);

		file << rho << "," << avg_expected_delay << "," <<  std_deviation_expected_delay << "," << rho << "," << avg_expected_queue_length 
		<< "," << std_deviation_expected_queue_length <<  "\n";
		
		std::cout << rho << "," << avg_expected_delay << "," <<  std_deviation_expected_delay << "," << rho << "," << avg_expected_queue_length 
		<< "," << std_deviation_expected_queue_length <<  "\n";
	}
	//*/
	/*
	std::random_device rd;
	std::mt19937 eng(rd());

	std::uniform_int_distribution<int> dist(1000000, 9999999);

	double critical_val = 16.919;
	int no_of_iter = 100;

	int count = 0;

	std::ofstream file("chi_test.csv");
	file << "Y" << ",critical_val" << "\n";

	double seed = 1234567; //set the seed only once, at the start.
	for (int k = 0; k < no_of_iter; ++k) {
		//double seed = dist(eng);

		int m = 400;
		int bins = 10;

		std::unordered_map<int, int> bin_count;
		for (int i = 0; i < bins; ++i) {
			bin_count[i] = 0;
		}

		for (int i = 0; i < m; ++i) {
			double r = uniform(&seed);
			int bin = r * 10;
			if (bin == 10) bin--;
			bin_count[bin]++;
		}

		int expected_val = m / bins;
		double Y = 0.0;
		for (auto& entry : bin_count) {
			Y += std::pow(entry.second - expected_val, 2) / expected_val;
		}

		file << Y <<"," << critical_val << "\n";
		//std::cout << Y << "\n";
		if (Y > critical_val) count++;
	}
	std::cout << count / (float)no_of_iter;
	
*/

/*
double seed = 1234567; //set the seed only once, at the start.

int no_of_iter = 10000;

float max_no = 10.0;
float bin_len = 0.1;
int no_of_bins = max_no / (float)bin_len;
double mean = 1.0;
double avg = 0.0;

std::unordered_map<int, int> bin_map;
for (int i = 0; i < no_of_bins; ++i) {
	bin_map[i] = 0;
}

for (int i = 0; i < no_of_iter; ++i) {
	double r = expon(&seed, mean);
	int bin = r / bin_len;
	if (r > max_no) {
		bin = no_of_bins - 1;
	}
	bin_map[bin]++;
	avg += r;
}

avg /= no_of_iter;

std::ofstream file("expon.csv");
file << "bin,expon,avg\n";
for (int i = 0; i < no_of_bins; ++i) {
	file << 0.1 * i << "," << bin_map[i] / (float)no_of_iter << "," << avg << "\n";
}

std::cout << avg << "\n";


*/



}