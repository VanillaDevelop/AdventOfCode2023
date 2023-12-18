#pragma once
#include "AoCSolution.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include <map>
#include <unordered_map>
#include <unordered_set>

enum Direction
{
	Up,
	Right,
	Down,
	Left
};

struct State
{
	int x;
	int y;
	Direction Dir;
	int DirectionCounter;
	int Distance;


	State(int x, int y, Direction direction, int directionCounter, int distance)
	{
		this->x = x;
		this->y = y;
		this->Dir = direction;
		this->DirectionCounter = directionCounter;
		this->Distance = distance;
	}

	bool operator==(const State& other) const
	{
		return this->x == other.x && this->y == other.y && this->Dir == other.Dir && this->DirectionCounter == other.DirectionCounter;
	}
};

struct StateHash {
	size_t operator()(const State& state) const {
		size_t seed = 0;
		seed ^= std::hash<int>()(state.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int>()(state.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int>()(state.Dir) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<int>()(state.DirectionCounter) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};


struct CompareState {
	bool operator()(const std::shared_ptr<State>& lhs, const std::shared_ptr<State>& rhs) const
	{
		return lhs->Distance > rhs->Distance;
	}
};

struct pair_hash {
	template <class T1, class T2>
	std::size_t operator() (const std::pair<T1, T2>& pair) const {
		auto h1 = std::hash<T1>{}(pair.first);
		auto h2 = std::hash<T2>{}(pair.second);
		return h1 ^ h2;
	}
};


//forward declare
void Dijkstra(std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, CompareState> & queue,
				std::unordered_map<std::pair<int, int>, int, pair_hash> & costMap,
				std::vector<int> & currentCosts, int width, int height);
void Dijkstra2(std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, CompareState> & queue,
	std::unordered_map<std::pair<int, int>, int, pair_hash> & costMap,
	std::vector<int> & currentCosts, int width, int height);
std::unordered_map<std::pair<int, int>, int, pair_hash> getCostMap(std::string input_file);

class Day17Solution : public AoCSolution
{
	void RunFirstPart(std::string input_file) override
	{
		auto costMap = getCostMap(input_file);
		std::vector<int> currentCosts = { 0 };
		int width = 0;
		int height = 0;
		for(int i = 1; i < costMap.size(); i++)
		{
			currentCosts.push_back(INT_MAX);
		}
		//get max width and height
		for (int i = 0; i < INT_MAX; i++)
		{
			if (costMap.find(std::make_pair(i, 0)) == costMap.end())
			{
				width = i;
				break;
			}
		}
		for (int i = 0; i < INT_MAX; i++)
		{
			if (costMap.find(std::make_pair(0, i)) == costMap.end())
			{
				height = i;
				break;
			}
		}

		std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, CompareState> queue;
		//create starter node
		queue.push(std::make_shared<State>(0, 0, Direction::Down, 0, 0));

		Dijkstra(queue, costMap, currentCosts, width, height);

		std::cout << "Shortest path: " << currentCosts[currentCosts.size() - 1] << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		auto costMap = getCostMap(input_file);
		std::vector<int> currentCosts = { 0 };
		int width = 0;
		int height = 0;
		for (int i = 1; i < costMap.size(); i++)
		{
			currentCosts.push_back(INT_MAX);
		}
		//get max width and height
		for (int i = 0; i < INT_MAX; i++)
		{
			if (costMap.find(std::make_pair(i, 0)) == costMap.end())
			{
				width = i;
				break;
			}
		}
		for (int i = 0; i < INT_MAX; i++)
		{
			if (costMap.find(std::make_pair(0, i)) == costMap.end())
			{
				height = i;
				break;
			}
		}

		std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, CompareState> queue;
		//create starter node
		queue.push(std::make_shared<State>(0, 1, Direction::Down, 1, costMap[std::make_pair(0, 1)]));
		queue.push(std::make_shared<State>(1, 0, Direction::Right, 1, costMap[std::make_pair(1, 0)]));

		Dijkstra2(queue, costMap, currentCosts, width, height);

		std::cout << "Shortest path: " << currentCosts[currentCosts.size() - 1] << std::endl;
	}
};

void Dijkstra2(std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, CompareState>& queue,
	std::unordered_map<std::pair<int, int>, int, pair_hash>& costMap,
	std::vector<int>& currentCosts, int width, int height)
{
	std::unordered_set<State, StateHash> alreadyAdded;

	while (!queue.empty())
	{
		std::shared_ptr<State> current = queue.top();
		queue.pop();

		//right neighbour
		if (current->x < width - 1 &&
			(current->Dir != Direction::Left) &&
			((current->Dir == Direction::Right && current->DirectionCounter < 10) ||
				(current->Dir != Direction::Right && current->DirectionCounter >= 4)))
		{
			int newDirectionCount = current->Dir == Direction::Right ? current->DirectionCounter + 1 : 1;
			int newCost = current->Distance + costMap[std::make_pair(current->x + 1, current->y)];
			if (currentCosts[(current->y * width + current->x + 1)] > newCost && (current->x+1 < width-1 || current->y < height-1 || newDirectionCount >= 4))
			{
				currentCosts[(current->y * width + current->x + 1)] = newCost;
			}
			State newState(current->x + 1, current->y, Direction::Right, newDirectionCount, newCost);
			if (alreadyAdded.find(newState) == alreadyAdded.end())
			{
				queue.push(std::make_shared<State>(newState));
				alreadyAdded.insert(newState);
			}
		}
		//left neighbour
		if(current->x > 0 && current->Dir != Direction::Right &&
			((current->Dir == Direction::Left && current->DirectionCounter < 10)
			|| (current->Dir != Direction::Left && current->DirectionCounter >= 4)))
		{
			int newDirectionCount = current->Dir == Direction::Left ? current->DirectionCounter + 1 : 1;
			int newCost = current->Distance + costMap[std::make_pair(current->x - 1, current->y)];
			if (currentCosts[(current->y * width + current->x - 1)] > newCost && (current->x - 1 > 0 || current->y > 0 || newDirectionCount >= 4))
			{
				currentCosts[(current->y * width + current->x - 1)] = newCost;
			}

			State newState(current->x - 1, current->y, Direction::Left, newDirectionCount, newCost);
			if (alreadyAdded.find(newState) == alreadyAdded.end())
			{
				queue.push(std::make_shared<State>(newState));
				alreadyAdded.insert(newState);
			}
		}
		//up neighbour
		if(current->y > 0 && current->Dir != Direction::Down &&
			((current->Dir == Direction::Up && current->DirectionCounter < 10)
			|| (current->Dir != Direction::Up && current->DirectionCounter >= 4)))
		{
			int newDirectionCount = current->Dir == Direction::Up ? current->DirectionCounter + 1 : 1;
			int newCost = current->Distance + costMap[std::make_pair(current->x, current->y - 1)];
			if (currentCosts[((current->y - 1) * width + current->x)] > newCost && (current->x > 0 || current->y - 1 > 0 || newDirectionCount >= 4))
			{
				currentCosts[((current->y - 1) * width + current->x)] = newCost;
			}
			State newState(current->x, current->y - 1, Direction::Up, newDirectionCount, newCost);
			if (alreadyAdded.find(newState) == alreadyAdded.end())
			{
				queue.push(std::make_shared<State>(newState));
				alreadyAdded.insert(newState);
			}
		}
		//down neighbour
		if(current->y < height - 1 && current->Dir != Direction::Up &&
			((current->Dir == Direction::Down && current->DirectionCounter < 10)
			|| (current->Dir != Direction::Down && current->DirectionCounter >= 4)))
		{
			int newDirectionCount = current->Dir == Direction::Down ? current->DirectionCounter + 1 : 1;
			int newCost = current->Distance + costMap[std::make_pair(current->x, current->y + 1)];
			if (currentCosts[((current->y + 1) * width + current->x)] > newCost && (current->x < width-1 || current->y < height-1 || newDirectionCount >= 4))
			{
				currentCosts[((current->y + 1) * width + current->x)] = newCost;
			}
			State newState(current->x, current->y + 1, Direction::Down, newDirectionCount, newCost);
			if (alreadyAdded.find(newState) == alreadyAdded.end())
			{
				queue.push(std::make_shared<State>(newState));
				alreadyAdded.insert(newState);
			}
		}
	}
}

void Dijkstra(std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, CompareState> & queue,
				std::unordered_map<std::pair<int, int>, int, pair_hash> & costMap,
				std::vector<int> & currentCosts, int width, int height)
{
	std::unordered_set<State, StateHash> alreadyAdded;

	while (!queue.empty())
	{
		std::shared_ptr<State> current = queue.top();
		queue.pop();

		//right neighbour
		if (current->x < width-1 && (current->Dir != Direction::Right || current->DirectionCounter < 3) && current->Dir != Direction::Left)
		{
			int newDirectionCount = current->Dir == Direction::Right ? current->DirectionCounter + 1 : 1;
			int newCost = current->Distance + costMap[std::make_pair(current->x + 1, current->y)];
			if (currentCosts[(current->y * width + current->x + 1)] > newCost)
			{
				currentCosts[(current->y * width + current->x + 1)] = newCost;
			}
			State newState(current->x + 1, current->y, Direction::Right, newDirectionCount, newCost);
			if(alreadyAdded.find(newState) == alreadyAdded.end())
			{
				queue.push(std::make_shared<State>(newState));
				alreadyAdded.insert(newState);
			}
		}
		//left neighbour
		if (current->x > 0 && (current->Dir != Direction::Left || current->DirectionCounter < 3) && current->Dir != Direction::Right)
		{
			int newDirectionCount = current->Dir == Direction::Left ? current->DirectionCounter + 1 : 1;
			int newCost = current->Distance + costMap[std::make_pair(current->x - 1, current->y)];
			if (currentCosts[(current->y * width + current->x - 1)] > newCost)
			{
				currentCosts[(current->y * width + current->x - 1)] = newCost;
			}
			State newState(current->x - 1, current->y, Direction::Left, newDirectionCount, newCost);
			if (alreadyAdded.find(newState) == alreadyAdded.end())
			{
				queue.push(std::make_shared<State>(newState));
				alreadyAdded.insert(newState);
			}
		}
		//up neighbour
		if (current->y > 0 && (current->Dir != Direction::Up || current->DirectionCounter < 3) && current->Dir != Direction::Down)
		{
			int newDirectionCount = current->Dir == Direction::Up ? current->DirectionCounter + 1 : 1;
			int newCost = current->Distance + costMap[std::make_pair(current->x, current->y - 1)];
			if (currentCosts[((current->y - 1) * width + current->x)] > newCost)
			{
				currentCosts[((current->y - 1) * width + current->x)] = newCost;
			}
			State newState(current->x, current->y - 1, Direction::Up, newDirectionCount, newCost);
			if (alreadyAdded.find(newState) == alreadyAdded.end())
			{
				queue.push(std::make_shared<State>(newState));
				alreadyAdded.insert(newState);
			}
		}
		//down neighbour
		if (current->y < height-1 && (current->Dir != Direction::Down || current->DirectionCounter < 3) && current->Dir != Direction::Up)
		{
			int newDirectionCount = current->Dir == Direction::Down ? current->DirectionCounter + 1 : 1;
			int newCost = current->Distance + costMap[std::make_pair(current->x, current->y + 1)];
			if (currentCosts[((current->y + 1) * width + current->x)] > newCost)
			{
				currentCosts[((current->y + 1) * width + current->x)] = newCost;
			}
			State newState(current->x, current->y + 1, Direction::Down, newDirectionCount, newCost);
			if (alreadyAdded.find(newState) == alreadyAdded.end())
			{
				queue.push(std::make_shared<State>(newState));
				alreadyAdded.insert(newState);
			}
		}
	}
}

std::unordered_map<std::pair<int, int>, int, pair_hash> getCostMap(std::string input_file)
{
	std::ifstream file(input_file);
	std::string line;
	std::unordered_map<std::pair<int, int>, int, pair_hash> costMap;

	int y = 0;
	while (std::getline(file, line))
	{
		int x = 0;
		for (char c : line)
		{
			int value = c - '0';
			costMap[std::make_pair(x, y)] = value;
			x++;
		}
		y++;
	}
	file.close();

	return costMap;
}
