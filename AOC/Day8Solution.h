#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

struct GraphNode
{
	std::shared_ptr<GraphNode> left;
	std::shared_ptr<GraphNode> right;
	bool is_start_node;
	bool is_end_node;
};

//Forward declarations
std::vector<bool> getDirections(std::ifstream& input_stream);
std::pair<std::shared_ptr<GraphNode>, std::shared_ptr<GraphNode>> getGraphNetwork(std::ifstream& input_stream);
std::shared_ptr<GraphNode> getNode(std::unordered_map<std::string, std::shared_ptr<GraphNode>>& node_map, std::string node_name);
int traverseToEnd(std::shared_ptr<GraphNode> start_node, std::shared_ptr<GraphNode> end_node, std::vector<bool> directions);
std::vector<std::shared_ptr<GraphNode>> getGraphNetworkPart2(std::ifstream& input_stream);
int traverseConsecutive(std::vector<std::shared_ptr<GraphNode>> start_nodes, std::vector<bool> directions);

class Day8Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		//parse the input from the file
		std::ifstream infile(input_file);
		std::vector<bool> directions = getDirections(infile);
		//Throw away the empty line
		std::string line;
		std::getline(infile, line);
		std::pair<std::shared_ptr<GraphNode>, std::shared_ptr<GraphNode>> network = getGraphNetwork(infile);
		int distance = traverseToEnd(network.first, network.second, directions);

		std::cout << "Length of traversal: " << distance << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		//parse the input from the file
		std::ifstream infile(input_file);
		std::vector<bool> directions = getDirections(infile);
		//Throw away the empty line
		std::string line;
		std::getline(infile, line);
		std::vector<std::shared_ptr<GraphNode>> start_nodes = getGraphNetworkPart2(infile);

		int distance = traverseConsecutive(start_nodes, directions);
		std::cout << "Length of traversal: " << distance << std::endl;
	}
};


/// <summary>
/// Get a vector of directions from the input stream.
/// </summary>
/// <param name="input_stream">The file stream of the input file</param>
/// <returns>A vector of directions for traversal (false = left, true = right)</returns>
std::vector<bool> getDirections(std::ifstream& input_stream)
{
	std::vector<bool> directions;

	std::string line;
	std::getline(input_stream, line);
	std::stringstream ss(line);

	char c;
	while (ss >> c)
	{
		directions.push_back(c == 'R');
	}

	return directions;
}

/// <summary>
/// Get the graph network from the input stream and return a pair of the start and end nodes.
/// </summary>
/// <param name="input_file">The file stream of the input file.</param>
/// <returns>The start node and end node of the network.</returns>
std::pair<std::shared_ptr<GraphNode>, std::shared_ptr<GraphNode>> getGraphNetwork(std::ifstream& input_stream)
{
	std::unordered_map<std::string, std::shared_ptr<GraphNode>> node_map;
	std::string line;
	std::shared_ptr<GraphNode> start_node = nullptr;
	std::shared_ptr<GraphNode> end_node = nullptr;

	while (std::getline(input_stream, line))
	{
		std::stringstream ss(line);
		std::string word;

		//get target node
		ss >> word;
		std::shared_ptr<GraphNode> target_node = getNode(node_map, word);
		if(word == "AAA")
		{
			start_node = target_node;
		}
		else if(word == "ZZZ")
		{
			end_node = target_node;
		}

		//Skip = sign and whitespace
		ss >> word;

		//Get left node
		ss >> word;
		std::shared_ptr<GraphNode> left_node = getNode(node_map, word.substr(1, 3));

		//Get right node
		ss >> word;
		std::shared_ptr<GraphNode> right_node = getNode(node_map, word.substr(0, 3));

		//Set the left and right nodes of the target node
		target_node->left = std::shared_ptr<GraphNode>(left_node);
		target_node->right = std::shared_ptr<GraphNode>(right_node);
	}

	return std::make_pair(start_node, end_node);
}


/// <summary>
/// Helper function for retrieving a node from the node map. If the node does not exist, it is created.
/// </summary>
/// <param name="node_map">The node map</param>
/// <param name="node_name">The name of the node to get</param>
/// <returns>A pointer to the node with the corresponding name.</returns>
std::shared_ptr<GraphNode> getNode(std:: unordered_map<std::string, std::shared_ptr<GraphNode>>& node_map, std::string node_name)
{
	if (node_map.find(node_name) == node_map.end())
	{
		GraphNode node = GraphNode();
		node_map[node_name] = std::make_shared<GraphNode>(node);
	}
	node_map[node_name]->is_start_node = node_name[node_name.size() - 1] == 'A';
	node_map[node_name]->is_end_node = node_name[node_name.size() - 1] == 'Z';

	return node_map[node_name];
}

/// <summary>
/// Traverse through the graph network from the start node to the end node using the given directions.
/// </summary>
/// <param name="start_node">The start node</param>
/// <param name="end_node">The end node</param>
/// <param name="directions">The array of repeating directions</param>
/// <returns>The number of nodes travelled (distance) until the end node.</returns>
int traverseToEnd(std::shared_ptr<GraphNode> start_node, std::shared_ptr<GraphNode> end_node, std::vector<bool> directions)
{
	int distance = 0;
	std::shared_ptr<GraphNode> current_node = start_node;
	while (true)
	{
		for (bool direction : directions)
		{
			if (direction)
			{
				current_node = current_node->right;
			}
			else
			{
				current_node = current_node->left;
			}
			distance++;
			if(current_node == end_node)
			{
				break;
			}
		}
		if(current_node == end_node)
		{
			break;
		}
	}

	return distance;
}

/// <summary>
/// Get the graph network from the input stream and return a list of all start nodes of the network.
/// </summary>
/// <param name="input_file">The file stream of the input file.</param>
/// <returns>A list of all start nodes of the network.</returns>
std::vector<std::shared_ptr<GraphNode>> getGraphNetworkPart2(std::ifstream& input_stream)
{
	std::unordered_map<std::string, std::shared_ptr<GraphNode>> node_map;
	std::string line;
	std::vector<std::shared_ptr<GraphNode>> start_nodes;

	while (std::getline(input_stream, line))
	{
		std::stringstream ss(line);
		std::string word;

		//get target node
		ss >> word;
		std::shared_ptr<GraphNode> target_node = getNode(node_map, word);
		if (target_node->is_start_node)
		{
			start_nodes.push_back(target_node);
		}

		//Skip = sign and whitespace
		ss >> word;

		//Get left node
		ss >> word;
		std::shared_ptr<GraphNode> left_node = getNode(node_map, word.substr(1, 3));

		//Get right node
		ss >> word;
		std::shared_ptr<GraphNode> right_node = getNode(node_map, word.substr(0, 3));

		//Set the left and right nodes of the target node
		target_node->left = left_node;
		target_node->right = right_node;
	}

	return start_nodes;
}

/// <summary>
/// Consecutively traverse through the graph network from all start nodes using the given directions until all nodes are end nodes.
/// </summary>
/// <param name="start_nodes">The list of start nodes</param>
/// <param name="directions">The array of repeating directions</param>
/// <returns>The number of nodes travelled (distance) until all nodes are end nodes.</returns>
int traverseConsecutive(std::vector<std::shared_ptr<GraphNode>> start_nodes, std::vector<bool> directions)
{
	int distance = 0;
	std::vector<std::shared_ptr<GraphNode>> current_nodes = start_nodes;
	bool any_false = true;
	while (true)
	{
		for (bool direction : directions)
		{
			any_false = false;
			for (int i = 0; i < current_nodes.size(); i++)
			{
				if (direction)
				{
					current_nodes[i] = current_nodes[i]->right;
				}
				else
				{
					current_nodes[i] = current_nodes[i]->left;
				}
				if (!(current_nodes[i]->is_end_node))
				{
					any_false = true;
				}
			}
			distance++;
			if (!any_false)
			{
				break;
			}
		}
		if (!any_false)
		{
			break;
		}
	}

	return distance;
}