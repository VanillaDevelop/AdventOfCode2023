#pragma once
#include "AoCSolution.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <set>

struct Position
{
	int x;
	int y;

	Position(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	Position()
	{
		this->x = 0;
		this->y = 0;
	}

	bool operator==(const Position& other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const Position& other) const
	{
		return x != other.x || y != other.y;
	}

	bool operator<(const Position& other) const
	{
		return x < other.x || (x == other.x && y < other.y);
	}

	int GetIndex(int width)
	{
		return x + y * width;
	}
};

struct Node
{
	Position Pos;
	int Distance;
	int VisitedIndex;

	Node(int x, int y, int distance, int VisitedIndex)
	{
		this->Pos.x = x;
		this->Pos.y = y;
		this->Distance = distance;
		this->VisitedIndex = VisitedIndex;
	}
};

enum Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct NodeTurn
{
	Position Pos;
	Direction MoveDirection;
};

/// <summary>
/// Get the input from the file
/// </summary>
/// <param name="input_file">The input file</param>
/// <returns>A pair containing a vector (array of chars), and ints declaring the width and height of the map</returns>
std::pair<std::vector<char>, std::pair<int, int>> GetInput(const std::string& input_file)
{
	std::vector<char> input;
	std::ifstream file(input_file);
	std::string line;
	int width = 0;
	int height = 0;
	while (std::getline(file, line))
	{
		for (auto c : line)
		{
			input.push_back(c);
		}
		width = line.size();
		height++;
	}
	return std::make_pair(input, std::make_pair(width, height));
}

void MakeCopy(std::unordered_map<int, std::vector<bool>> & visited, int oldIdx, int newIdx)
{
	for(auto pos : visited[oldIdx])
	{
		visited[newIdx].push_back(pos);
	}
}

long PathfindDFS(const std::vector<char>& map, int width, int height, Position start, Position end)
{
	std::stack<NodeTurn> turns;
	turns.push(NodeTurn{ start, Direction::DOWN});
	std::vector<Position> visited;
	std::unordered_set<int> visited_set;

	//create distance map (initialize to 0 since we are looking for the longest path)
	int max_distance = 0;

	while (turns.size() > 0)
	{
		NodeTurn last_turn = turns.top();
		turns.pop();

		Position current_position = last_turn.Pos;
		//remove every visited position until the previous turn
		while (!visited.empty() && visited.back() != last_turn.Pos)
		{
			visited_set.erase(visited.back().GetIndex(width));
			visited.pop_back();
		}
		//do 1 movement
		switch (last_turn.MoveDirection)
		{
			case Direction::UP:
				current_position.y--;
				break;
			case Direction::DOWN:
				current_position.y++;
				break;
			case Direction::LEFT:
				current_position.x--;
				break;
			case Direction::RIGHT:
				current_position.x++;
				break;
		}

		while (true)
		{
			visited.push_back(current_position);
			visited_set.insert(current_position.GetIndex(width));

			//check each neighbours eligibility
			bool can_move_up = current_position.y > 0 && map[current_position.GetIndex(width) - width] != '#' && visited_set.find(current_position.GetIndex(width) - width) == visited_set.end();
			bool can_move_down = current_position.y < height - 1 && map[current_position.GetIndex(width) + width] != '#' && visited_set.find(current_position.GetIndex(width) + width) == visited_set.end();
			bool can_move_left = current_position.x > 0 && map[current_position.GetIndex(width) - 1] != '#' && visited_set.find(current_position.GetIndex(width) - 1) == visited_set.end();
			bool can_move_right = current_position.x < width - 1 && map[current_position.GetIndex(width) + 1] != '#' && visited_set.find(current_position.GetIndex(width) + 1) == visited_set.end();

			//if no direction is available (dead end)
			if (!can_move_up && !can_move_down && !can_move_left && !can_move_right)
			{
				//update max distance if this is the goal position
				if (current_position == end)
				{
					if (visited.size() > max_distance)
					{
						max_distance = visited.size();
						std::cout << "New max distance: " << max_distance << std::endl;
					}
				}
				break;
			}

			//if no more than one direction is available
			if((can_move_up ? 1 : 0) + (can_move_down ? 1 : 0) + (can_move_left ? 1 : 0) + (can_move_right ? 1 : 0) == 1)
			{
				//we are not at a crossing -> we follow the path
				if(can_move_up) current_position.y--;
				else if(can_move_down) current_position.y++;
				else if(can_move_left) current_position.x--;
				else if(can_move_right) current_position.x++;
				continue;
			}

			//otherwise, we are at a crossing -> we add all directions but the last one to the stack, and continue following the last one
			//update that the last turn was here
			last_turn.Pos = current_position;
			if (can_move_up)
			{
				//this can never be the last if we are at a crossing
				turns.push(NodeTurn{ Position(current_position.x, current_position.y), Direction::UP });
			}
			if (can_move_down)
			{
				if (can_move_left || can_move_right)
				{
					turns.push(NodeTurn{ Position(current_position.x, current_position.y), Direction::DOWN });
				}
				else
				{
					//follow the path
					current_position.y++;
				}
			}
			if (can_move_left)
			{
				if (can_move_right)
				{
					turns.push(NodeTurn{ Position(current_position.x, current_position.y), Direction::LEFT });
				}
				else
				{
					//follow the path
					current_position.x--;
				}
			}
			if (can_move_right)
			{
				//follow the path
				current_position.x++;
			}
		}		
	}

	return max_distance;
};

long Pathfind(const std::vector<char>& map, int width, int height, Node start, Position end, bool ignoreSlopes=false)
{
	std::queue<Node> nodes;
	nodes.push(start);
	std::unordered_map<int, std::vector<bool>> visited;
	visited[0] = std::vector<bool>(width * height, false);

	//create distance map (initialize to 0 since we are looking for the longest path)
	std::vector<int> dist(width * height, 0);
	dist[start.Pos.GetIndex(width)] = INT_MAX;

	//if we need to add a new branch, we use this variable
	int next_visited_idx = 1;

	while (nodes.size() > 0)
	{
		Node current = nodes.front();
		nodes.pop();

		visited[current.VisitedIndex][current.Pos.GetIndex(width)] = true;
		if (dist[current.Pos.GetIndex(width)] < current.Distance) dist[current.Pos.GetIndex(width)] = current.Distance;

		bool first_path = true;

		//get neighbours, push back if they are not # or forced march ><v^
		//left
		if (current.Pos.x > 0 && (map[current.Pos.GetIndex(width) - 1] == '.' || map[current.Pos.GetIndex(width) - 1] == '<' ||
			map[current.Pos.GetIndex(width) - 1] != '#' && ignoreSlopes) && !visited[current.VisitedIndex][current.Pos.GetIndex(width) - 1])
		{
			if (first_path)
			{
				first_path = false;
				nodes.push(Node(current.Pos.x - 1, current.Pos.y, current.Distance + 1, current.VisitedIndex));
			}
			else
			{
				nodes.push(Node(current.Pos.x - 1, current.Pos.y, current.Distance + 1, next_visited_idx));
				MakeCopy(visited, current.VisitedIndex, next_visited_idx);
				next_visited_idx++;
			}
		}
		//right
		if (current.Pos.x < width - 1 && (map[current.Pos.GetIndex(width)+1] == '.' || map[current.Pos.GetIndex(width)+1] == '>' ||
			map[current.Pos.GetIndex(width)+1] != '#' && ignoreSlopes) && !visited[current.VisitedIndex][current.Pos.GetIndex(width) + 1])
		{
			if (first_path)
			{
				first_path = false;
				nodes.push(Node(current.Pos.x + 1, current.Pos.y, current.Distance + 1, current.VisitedIndex));
			}
			else
			{
				nodes.push(Node(current.Pos.x + 1, current.Pos.y, current.Distance + 1, next_visited_idx));
				MakeCopy(visited, current.VisitedIndex, next_visited_idx);
				next_visited_idx++;
			}
		}
		//up
		if (current.Pos.y > 0 && (map[current.Pos.GetIndex(width)-width] == '.' || map[current.Pos.GetIndex(width) - width] == '^' ||
			map[current.Pos.GetIndex(width) - width] != '#' && ignoreSlopes) && !visited[current.VisitedIndex][current.Pos.GetIndex(width) - width])
		{
			if (first_path)
			{
				first_path = false;
				nodes.push(Node(current.Pos.x, current.Pos.y - 1, current.Distance + 1, current.VisitedIndex));
			}
			else
			{
				nodes.push(Node(current.Pos.x, current.Pos.y - 1, current.Distance + 1, next_visited_idx));
				MakeCopy(visited, current.VisitedIndex, next_visited_idx);
				next_visited_idx++;
			}
		}
		//down
		if (current.Pos.y < height - 1 && (map[current.Pos.GetIndex(width) + width] == '.' || map[current.Pos.GetIndex(width) + width] == 'v' ||
			map[current.Pos.GetIndex(width) + width] != '#' && ignoreSlopes) && !visited[current.VisitedIndex][current.Pos.GetIndex(width) + width])
		{
			if (first_path)
			{
				first_path = false;
				nodes.push(Node(current.Pos.x, current.Pos.y + 1, current.Distance + 1, current.VisitedIndex));
			}
			else
			{
				nodes.push(Node(current.Pos.x, current.Pos.y + 1, current.Distance + 1, next_visited_idx));
				MakeCopy(visited, current.VisitedIndex, next_visited_idx);
				next_visited_idx++;
			}
		}
	}

	return dist[end.GetIndex(width)];
};

class Day23Solution : public AoCSolution
{
	void RunFirstPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		auto map = input.first;
		auto width = input.second.first;
		auto height = input.second.second;

		Node startNode(0, 0, 0, 0);
		Position endNode(0, height - 1);

		//find "." in first row (start node)
		for (int i = 0; i < width; i++)
		{
			if (map[i] == '.')
			{
				//found start node
				startNode.Pos.x = i;
				break;
			}
		}

		//find "." in last row (end node)
		for (int i = 0; i < width; i++)
		{
			if (map[(height - 1) * width + i] == '.')
			{
				endNode.x = i;
				break;
			}
		}

		long cost = Pathfind(map, width, height, startNode, endNode);
		std::cout << "Cost: " << cost << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		auto map = input.first;
		auto width = input.second.first;
		auto height = input.second.second;

		Position startNode(0, 0);
		Position endNode(0, height - 1);

		//find "." in first row (start node)
		for (int i = 0; i < width; i++)
		{
			if (map[i] == '.')
			{
				//found start node
				startNode.x = i;
				break;
			}
		}

		//find "." in last row (end node)
		for (int i = 0; i < width; i++)
		{
			if (map[(height - 1) * width + i] == '.')
			{
				endNode.x = i;
				break;
			}
		}

		long cost = PathfindDFS(map, width, height, startNode, endNode);
		std::cout << "Cost: " << cost << std::endl;
	}
};

