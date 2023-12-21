#pragma once
#include "AoCSolution.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include <sstream>
#include <iostream>
#include <fstream>

struct Pulse
{
	std::string source;
	bool signal;
};

class Signal
{
public:
	Signal(std::string name)
	{
		this->m_name = name;
	}

	void AddTarget(std::string target)
	{
		//check if target is already in the list
		for (auto t : this->m_targets)
		{
			if (t == target)
			{
				return;
			}
		}
		this->m_targets.push_back(target);
	}

	virtual void AddSource(std::string source)
	{
		//check if source is already in the list
		for (auto s : this->m_sources)
		{
			if (s == source)
			{
				return;
			}
		}
		this->m_sources.push_back(source);
	}

	std::vector<std::string> GetSources()
	{
		return this->m_sources;
	}

	std::vector<std::string> GetTargets()
	{
		return this->m_targets;
	}

	std::string GetName() 
	{ 
		return this->m_name; 
	}

	virtual std::vector<std::pair<std::string, Pulse>> ReceiveSignal(Pulse signal) = 0;

protected:
	std::vector<std::string> m_sources;
	std::string m_name;
	std::vector<std::string> m_targets;
};

class Broadcaster : public Signal
{
public:
	Broadcaster(std::string name) : Signal(name) {}

	virtual std::vector<std::pair<std::string, Pulse>> ReceiveSignal(Pulse signal) override
	{
		std::vector<std::pair<std::string, Pulse>> out;
		for (auto target : this->m_targets)
		{
			out.push_back(std::make_pair(target, Pulse{this->m_name, signal.signal}));
		}
		return out;
	}
};

class FlipFlop : public Signal
{
public:
	FlipFlop(std::string name) : Signal(name) {}

	virtual std::vector<std::pair<std::string, Pulse>> ReceiveSignal(Pulse signal) override
	{
		if (!signal.signal)
		{
			this->m_on = !this->m_on;
			
			std::vector<std::pair<std::string, Pulse>> out;
			for (auto target : this->m_targets)
			{
				out.push_back(std::make_pair(target, Pulse{ this->m_name, this->m_on}));
			}
			return out;
		}
	}

private:
	bool m_on = false;
};

class Conjunction : public Signal
{
public:
	Conjunction(std::string name) : Signal(name) {}

	virtual void AddSource(std::string source) override
	{
		Signal::AddSource(source);
		m_prev_signals[source] = false;
	}

	bool CheckCurrentSignal()
	{
		bool send_signal = false;
		//send true signal if at least one source is false
		for (auto source : m_prev_signals)
		{
			if (!source.second)
			{
				send_signal = true;
				break;
			}
		}
		return send_signal;
	}

	virtual std::vector<std::pair<std::string, Pulse>> ReceiveSignal(Pulse signal) override
	{
		this->m_prev_signals[signal.source] = signal.signal;
		bool test = signal.source == "sn";
		bool send_signal = this->CheckCurrentSignal();
		//send to all targets
		std::vector<std::pair<std::string, Pulse>> out;
		for (auto target : this->m_targets)
		{
			out.push_back(std::make_pair(target, Pulse{ this->m_name, send_signal }));
		}
		return out;
	}


private:
	std::unordered_map<std::string, bool> m_prev_signals;
};

class NoOp : public Signal
{
public:
	NoOp(std::string name) : Signal(name) {}

	virtual std::vector<std::pair<std::string, Pulse>> ReceiveSignal(Pulse signal) override
	{
		std::vector<std::pair<std::string, Pulse>> out;
		return out;
	}
};

std::pair<int,int> PressButton(std::unordered_map<std::string, std::shared_ptr<Signal>> & signals)
{
	std::queue<std::pair<std::string, Pulse>> out;
	out.push(std::make_pair("broadcaster", Pulse{ "button", false }));

	int low_signals = 0;
	int high_signals = 0;

	while (!out.empty())
	{
		auto signal = out.front();
		out.pop();
		
		//add to high or low count
		if (signal.second.signal)
		{
			high_signals++;
		}
		else
		{
			low_signals++;
		}

		//send signal and receive new signals
		auto signals_new = signals[signal.first]->ReceiveSignal(signal.second);

		//push new signals to the queue
		for (auto signal_new : signals_new)
		{
			out.push(signal_new);
		}
	}

	return std::make_pair(low_signals, high_signals);
}

long long ReturnNPressCount(std::unordered_map<std::string, std::shared_ptr<Signal>> & signals, int n)
{
	long long total_low = 0;
	long long total_high = 0;

	for (int i = 0; i < n; i++)
	{	
		auto counts = PressButton(signals);
		total_low += counts.first;
		total_high += counts.second;
	}

	return total_low * total_high;
}

std::pair<int, int> GetCycleOf(std::unordered_map<std::string, std::shared_ptr<Signal>>& signals, std::string name, bool target)
{
	int first = 0;
	int cycle = 0;

	std::queue<std::pair<std::string, Pulse>> out;

	int current = 0;
	while (true)
	{
		if (out.empty())
		{
			out.push(std::make_pair("broadcaster", Pulse{ "button", false }));
			current++;
		}

		auto signal = out.front();
		out.pop();

		//send signal and receive new signals
		auto signals_new = signals[signal.first]->ReceiveSignal(signal.second);

		//push new signals to the queue
		for (auto signal_new : signals_new)
		{
			out.push(signal_new);
			if(signal_new.first == name && signal_new.second.signal == target)
			{
				if (first == 0)
				{
					first = current;
				}
				else
				{
					cycle = current - first;
					return std::make_pair(first, cycle);
				}
			}
		}
	}
}

std::unordered_map<std::string, std::shared_ptr<Signal>> ParseInput(std::string input_file)
{
	std::unordered_map<std::string, std::shared_ptr<Signal>> signals;

	std::ifstream input(input_file);
	std::string line;

	std::unordered_map<std::string, std::vector<std::string>> signal_targets;

	while (std::getline(input, line))
	{
		//find -> 
		std::size_t arrow_pos = line.find("->");
		//name goes from 1 until 1 character before arrow
		std::string name = line.substr(1, arrow_pos - 2);
		//targets start at 1 character after arrow
		std::string targets = line.substr(arrow_pos + 3) + ",";
		//type is char at 0
		char type = line[0];

		std::shared_ptr<Signal> sig_ptr;
		//depending on the char, create the signal type
		if (type == 'b')
		{
			sig_ptr = std::make_shared<Broadcaster>("broadcaster");
			name = "broadcaster";
		}
		else if (type == '%')
		{
			sig_ptr = std::make_shared<FlipFlop>(name);
		}
		else if (type == '&')
		{
			sig_ptr = std::make_shared<Conjunction>(name);
		}
		else
		{
			sig_ptr = std::make_shared<NoOp>(name);
		}

		//write down the targets for this signal
		std::size_t last_comma = 0;
		std::size_t comma_pos;

		while ((comma_pos = targets.find(",", last_comma)) != std::string::npos)
		{
			std::string target = targets.substr(last_comma, comma_pos - last_comma);
			signal_targets[name].push_back(target);
			last_comma = comma_pos + 2;
		}

		//add the signal to the map
		signals[name] = sig_ptr;
	}

	//add all targets to the signals
	for (auto signal : signal_targets)
	{
		for (auto target : signal.second)
		{
			signals[signal.first]->AddTarget(target);
			//we can have the case here that the target doesn't do anything, in this case we create a NoOp
			if (signals.find(target) == signals.end())
			{
				signals[target] = std::make_shared<NoOp>(target);
			}
			signals[target]->AddSource(signal.first);
		}
	}

	return signals;
}

class Day20Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		auto signals = ParseInput(input_file);
		long long result = ReturnNPressCount(signals, 1000);

		std::cout << "Result: " << result << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		auto signals = ParseInput(input_file);
		//get rx
		std::shared_ptr<Signal> rx = signals["rx"];
		//Get single source of rx -> we want this conjunction to return low -> all its sources must be high
		std::shared_ptr<Signal> src = signals[rx->GetSources()[0]];
		//Get all sources of src
		auto sources = src->GetSources();

		std::vector<std::pair<int,int>> cycles;
		//For each source, get the cycle lengths of its source
		for(auto source : sources)
		{
			signals = ParseInput(input_file);
			cycles.push_back(GetCycleOf(signals, source, false));
		}

		//Get the LCM of all cycle lengths
		long long current = 0;
		int min_cycle = INT_MAX;
		int min_initial = INT_MAX;

		//Find the max initial distance and min cycle distance
		for (std::pair<int, int> cycle : cycles)
		{
			if (cycle.second < min_cycle)
			{
				min_initial = cycle.first;
				min_cycle = cycle.second;
			}
		}

		//set initial to min_initial
		current = min_initial - min_cycle;
		bool found = false;

		while (!found)
		{
			current += min_cycle;
			found = true;
			for (auto cycle : cycles)
			{
				if (((current - cycle.first) % cycle.second) != 0)
				{
					found = false;
					break;
				}
			}
		}

		std::cout << "Result: " << current << std::endl;
	}
};

