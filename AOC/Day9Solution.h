#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

//Forward declarations
std::vector<std::vector<std::size_t>> getSequences(const std::string& input_file);
std::size_t evaluateSequence(std::vector<std::size_t> sequence, bool use_last);
std::size_t evaluateAllSequences(std::vector<std::vector<std::size_t>> sequences, bool use_last);

class Day9Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		std::vector<std::vector<std::size_t>> sequences = getSequences(input_file);

		std::size_t sum = evaluateAllSequences(sequences, true);

		std::cout << "Sum: " << sum << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		std::vector<std::vector<std::size_t>> sequences = getSequences(input_file);

		std::size_t sum = evaluateAllSequences(sequences, false);

		std::cout << "Sum: " << sum << std::endl;
	}
};


std::size_t evaluateAllSequences(std::vector<std::vector<std::size_t>> sequences, bool use_last = true)
{
	std::size_t sum = 0;
	for (std::vector<std::size_t> sequence : sequences)
	{
		sum += evaluateSequence(sequence, use_last);
	}
	return sum;
}


std::size_t evaluateSequence(std::vector<std::size_t> sequence, bool use_last = true)
{
	std::vector<std::size_t> add_stack;

	int eval_len = sequence.size();

	while (true)
	{
		bool all_zero = true;
		if(!use_last)
		{
			add_stack.push_back(sequence[0]);
		}
		for(int i = 0; i < eval_len-1; i++)
		{
			//Set sequence index i to the difference between i+1 and i
			sequence[i] = sequence[i + 1] - sequence[i];
			//check if the resulting value is 0 
			all_zero = all_zero && sequence[i] == 0;
		}
		//add the first/last value to the stack
		if (use_last)
		{
			add_stack.push_back(sequence[eval_len - 1]);
		}
		//If all values are all 0, break the loop
		if (all_zero)
		{
			break;
		}
		//Repeat with the next sequence with 1 less index
		eval_len--;
	}

	if (use_last) 
	{
		//Return the sum of the stack
		std::size_t sum = 0;
		for (std::size_t i : add_stack)
		{
			sum += i;
		}
		return sum;
	}
	else
	{
		//find the difference between current val and next val
		int next = 0;
		for (int i = add_stack.size() - 1; i >= 0; i--)
		{
			next = add_stack[i] - next;
		}
		return next;
	}

}

std::vector<std::vector<std::size_t>> getSequences(const std::string & input_file)
{
	std::vector<std::vector<std::size_t>> sequences;
	std::ifstream file(input_file);
	std::string line;
	while (std::getline(file, line))
	{
		std::vector<std::size_t> sequence;
		std::istringstream iss(line);
		std::size_t number;
		while (iss >> number)
		{
			sequence.push_back(number);
		}
		sequences.push_back(sequence);
	}
	return sequences;
}