#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

//forward declarations
std::size_t CalculateHashSum(const std::string& input);
std::size_t CalculateHash(const std::string& input);
std::string ParseInput(std::string input_file);
void ProcessAll(const std::string& input);
void ProcessCommand(const std::string& input);
std::size_t CalculateLensPower();

struct Lens
{
	int Length;
	std::string name;
	Lens* nextLens;
};

Lens* map[256];

class Day15Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		std::string input = ParseInput(input_file);
		std::cout << "Hash sum: " << CalculateHashSum(input) << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		std::string input = ParseInput(input_file);
		ProcessAll(input);
		std::cout << "Lens power: " << CalculateLensPower() << std::endl;
	}
};

std::size_t CalculateLensPower()
{
	std::size_t power = 0;

	for (int i=0; i<256; i++)
	{
		Lens* lens = map[i];
		int slot = 0;
		while (lens != nullptr)
		{
			power += (i + 1) * (slot + 1) * lens->Length;
			lens = lens->nextLens;
			slot++;
		}
	}

	return power;
}

void ProcessAll(const std::string& input)
{
	std::size_t pos = 0;
	std::size_t last_pos = 0;

	std::string token;
	while ((pos = input.find(",", last_pos)) != std::string::npos)
	{
		token = input.substr(last_pos, pos - last_pos);
		ProcessCommand(token);
		last_pos = pos + 1;
	}
	//last token
	ProcessCommand(input.substr(last_pos, input.size() - last_pos));
}

void ProcessCommand(const std::string& input)
{
	std::size_t pos;
	if((pos = input.find("=")) != std::string::npos)
	{
		std::string name = input.substr(0, pos);
		int length = stoi(input.substr(pos + 1, input.size() - pos - 1));
		int box = CalculateHash(name);

		if (map[box] == nullptr)
		{
			map[box] = new Lens();
			map[box]->Length = length;
			map[box]->name = name;
			map[box]->nextLens = nullptr;
			return;
		}
		else
		{
			Lens* current = map[box];
			while(current->nextLens != nullptr && current->name != name)
			{
				current = current->nextLens;
			}
			if(current->name == name)
			{
				current->Length = length;
				return;
			}
			else
			{
				current->nextLens = new Lens();
				current->nextLens->Length = length;
				current->nextLens->name = name;
				current->nextLens->nextLens = nullptr;
				return;
			}
		}
	}
	else if ((pos = input.find("-")) != std::string::npos)
	{
		std::string name = input.substr(0, pos);
		int box = CalculateHash(name);

		if (map[box] != nullptr)
		{
			Lens* current = map[box];
			if(current->name == name)
			{
				map[box] = current->nextLens;
				delete current;
				return;
			}
			while (current->nextLens != nullptr && current->nextLens->name != name)
			{
				current = current->nextLens;
			}
			if (current->nextLens != nullptr)
			{
				Lens* temp = current->nextLens;
				current->nextLens = current->nextLens->nextLens;
				delete temp;
				return;
			}
		}
	}
}

std::size_t CalculateHashSum(const std::string& input)
{
	std::size_t hash = 0;
	std::size_t pos = 0;
	std::size_t last_pos = 0;

	std::string token;
	while ((pos = input.find(",", last_pos)) != std::string::npos)
	{
		token = input.substr(last_pos, pos-last_pos);
		hash += CalculateHash(token);
		last_pos = pos + 1;
	}
	//last token
	hash += CalculateHash(input.substr(last_pos, input.size() - last_pos));

	return hash;
}

std::size_t CalculateHash(const std::string & input)
{
	std::size_t hash = 0;
	for (auto c : input)
	{
		hash += (int)c;
		hash *= 17;
		hash %= 256;
	}
	return hash;
}

std::string ParseInput(std::string input_file)
{
	std::ifstream input(input_file);
	std::string line;
	std::getline(input, line);
	return line;
}

