#pragma once
#include "AoCSolution.h"
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <queue>

struct Node
{
	std::string Name;
	std::vector<std::weak_ptr<struct Edge>> Edges;

	bool operator==(const Node& other) const
	{
		return Name == other.Name;
	}

	bool operator<(const Node& other) const
	{
		return Name < other.Name;
	}
};

struct Edge
{
	std::weak_ptr<Node> Node1, Node2;

	bool operator==(const Edge& other) const
	{
		return (Node1.lock().get() == other.Node1.lock().get() && Node2.lock().get() == other.Node2.lock().get());
	}

	bool operator<(const Edge& other) const
	{
		if (Node1.lock().get() == other.Node1.lock().get())
		{
			return Node2.lock().get() < other.Node2.lock().get();
		}
		else
		{
			return Node1.lock().get() < other.Node1.lock().get();
		}
	}
};

struct CompareWeakPtrNode {
	bool operator()(const std::weak_ptr<Node>& a, const std::weak_ptr<Node>& b) const {
		auto aPtr = a.lock();
		auto bPtr = b.lock();
		if (!aPtr || !bPtr) return false; // handle expired pointers
		return aPtr->Name < bPtr->Name;
	}
};

struct CompareWeakPtrEdge {
	bool operator()(const std::weak_ptr<Edge>& a, const std::weak_ptr<Edge>& b) const {
		auto aPtr = a.lock();
		auto bPtr = b.lock();
		if (!aPtr || !bPtr) return false; // handle expired pointers
		return *aPtr < *bPtr;
	}
};

struct SortCostPair {
	bool operator()(const std::pair<std::shared_ptr<Edge>, int>& a, const std::pair<std::shared_ptr<Edge>, int>& b) const
	{ 
		if(a.second == b.second)
		{
			return *(a.first) < *(b.first);
		}
		else
		{
			return a.second > b.second;
		}
	}
};

std::pair<std::map<std::string, std::shared_ptr<Node>>, std::set<std::shared_ptr<Edge>>> ParseInput(const std::string & input_file)
{
	std::map<std::string, std::shared_ptr<Node>> nodes;
	std::set<std::shared_ptr<Edge>> edges;

	std::ifstream file(input_file);
	std::string line;
	
	while (std::getline(file, line))
	{
		//read nodes/edges from line
		std::string name = line.substr(0, 3);
		std::stringstream others(line.substr(5, line.length() - 5));
		std::vector<std::string> other_nodes;
		std::string token;

		while (others >> token)
		{
			other_nodes.push_back(token);
		}

		//create nodes if they don't exist
		if (nodes.find(name) == nodes.end())
		{
			nodes[name] = std::make_shared<Node>();
			nodes[name]->Name = name;
		}

		for (const auto& other_name : other_nodes)
		{
			if (nodes.find(other_name) == nodes.end())
			{
				nodes[other_name] = std::make_shared<Node>();
				nodes[other_name]->Name = other_name;
			}
		}

		//create edges
		for (const auto& other_name : other_nodes)
		{
			auto edge = std::make_shared<Edge>();
			edge->Node1 = nodes[name];
			edge->Node2 = nodes[other_name];
			edges.insert(edge);
			nodes[name]->Edges.push_back(edge);
			nodes[other_name]->Edges.push_back(edge);
		}
	}

	return std::make_pair(nodes, edges);
}

int RemovedEdgeCost(const std::map<std::string, std::shared_ptr<Node>> & nodes, const std::set<std::shared_ptr<Edge>> & edges, std::shared_ptr<Edge> removeEdge)
{
	//get the two nodes that are connected by the edge
	auto node1 = removeEdge->Node1.lock();
	auto node2 = removeEdge->Node2.lock();

	//traverse BFS from node1 to node2, counting the number of edges crossed
	int cost = 0;
	
	//it will always be possible to still reach node2 from node1, since there are three edges connecting the two sides of the graph
	std::set<std::weak_ptr<Node>, CompareWeakPtrNode> visited;
	std::queue<std::weak_ptr<Node>> nodeQueue;
	nodeQueue.push(node1);
	while (true)
	{
		//step 1: test if one of the nodes in the visited set is node2. If yes, return the current cost
		if(visited.find(node2) != visited.end())
		{
			return cost;
		}

		//step 2: get all items currently in the queue
		std::vector<std::weak_ptr<Node>> queueItems;
		while (!nodeQueue.empty())
		{
			queueItems.push_back(nodeQueue.front());
			nodeQueue.pop();
		}

		//step 3: handle all items in the local queue, add new items to the global queue (+1 depth)
		for (const auto& item : queueItems)
		{
			//add item to visited set
			visited.insert(item);

			//add all unvisited neighbours to the global queue
			for (const auto & edge : item.lock()->Edges)
			{
				//skip the edge we're removing
				if (edge.lock() == removeEdge) continue;

				auto otherNode = edge.lock()->Node1.lock() == item.lock() ? edge.lock()->Node2.lock() : edge.lock()->Node1.lock();
				if (visited.find(otherNode) == visited.end())
				{
					nodeQueue.push(otherNode);
				}
			}
		}

		//step 4: increment cost
		cost++;

		//step 5: if the global queue is empty, return -1 (no path found) - this is a fallback and should never happen
		if (nodeQueue.empty())
		{
			return -1;
		}
	}
}

int CheckSeparated(const std::map<std::string, std::shared_ptr<Node>>& nodes, const std::set<std::shared_ptr<Edge>>& edges, std::set<std::weak_ptr<Edge>, CompareWeakPtrEdge> removedEdges)
{
	//take an arbitrary node and traverse BFS from it, counting the number of nodes visited, while ignoring the removed edges
	std::set<std::weak_ptr<Node>, CompareWeakPtrNode> visited;
	std::queue<std::weak_ptr<Node>> nodeQueue;
	nodeQueue.push(nodes.begin()->second);
	while (!nodeQueue.empty())
	{
		auto currentNode = nodeQueue.front();
		nodeQueue.pop();

		//add to visited set
		visited.insert(currentNode);

		//add all unvisited neighbours to the global queue
		for (const auto& edge : currentNode.lock()->Edges)
		{
			//skip any removed edge
			if (removedEdges.find(edge) != removedEdges.end()) continue;

			auto otherNode = edge.lock()->Node1.lock() == currentNode.lock() ? edge.lock()->Node2.lock() : edge.lock()->Node1.lock();
			if (visited.find(otherNode) == visited.end())
			{
				nodeQueue.push(otherNode);
			}
		}
	}

	return visited.size();
}

class Day25Solution : public AoCSolution
{
public: 
	void RunFirstPart(std::string input_file) override
	{
		//holy moly I didn't know you could do this
		auto [nodes, edges] = ParseInput(input_file);

		//iterate over all edges and store their corresponding costs
		std::map<std::shared_ptr<Edge>, int> edgeCosts;
		for (const auto& edge : edges)
		{
			edgeCosts[edge] = RemovedEdgeCost(nodes, edges, edge);
		}

		//store as a set of pairs, sorted by cost
		std::set<std::pair<std::shared_ptr<Edge>, int>, SortCostPair> sortedEdgeCosts;
		for (const auto& edgeCost : edgeCosts)
		{
			sortedEdgeCosts.insert(edgeCost);
		}

		for(int i = 0; i < sortedEdgeCosts.size(); i++)
		{
			for (int j = i + 1; j < sortedEdgeCosts.size(); j++)
			{
				for (int k = j + 1; k < sortedEdgeCosts.size(); k++)
				{
					//kinda cringe iteration but oh well
					std::set<std::weak_ptr<Edge>, CompareWeakPtrEdge> removedEdges;
					auto it = sortedEdgeCosts.begin();
					std::advance(it, i);
					removedEdges.insert(it->first);
					it = sortedEdgeCosts.begin();
					std::advance(it, j);
					removedEdges.insert(it->first);
					it = sortedEdgeCosts.begin();
					std::advance(it, k);
					removedEdges.insert(it->first);

					int reachable = CheckSeparated(nodes, edges, removedEdges);
					if (reachable != nodes.size())
					{
						std::cout << "Set Split: " << reachable << "/" << nodes.size() - reachable << std::endl;
					}
					return;
				}
			}
		}
	}

	void RunSecondPart(std::string input_file) override
	{

	}
};

