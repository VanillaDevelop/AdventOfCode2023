#pragma once
#include "AoCSolution.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>

struct Rule
{
	int x_min;
	int x_max;
	int m_min;
	int m_max;
	int a_min;
	int a_max;
	int s_min;
	int s_max;
	std::string dest;

	//Rule ctor
	Rule(const std::string& rule)
	{
		int colon;

		char target = '#';
		bool is_gt = false;
		int val = INT_MAX;
		std::string dest;
		//find :
		if ((colon = rule.find(':')) != std::string::npos)
		{
			target = rule[0];
			is_gt = rule[1] == '>';
			val = std::stoi(rule.substr(2, colon - 2));
		}
		else
		{
			colon = -1;
		}
		dest = rule.substr(colon + 1);

		//set variables accordingly
		this->x_min = (target == 'x' && is_gt) ? (val+1) : INT_MIN;
		this->x_max = (target == 'x' && !is_gt) ? (val-1) : INT_MAX;
		this->m_min = (target == 'm' && is_gt) ? (val+1) : INT_MIN;
		this->m_max = (target == 'm' && !is_gt) ? (val-1) : INT_MAX;
		this->a_min = (target == 'a' && is_gt) ? (val+1) : INT_MIN;
		this->a_max = (target == 'a' && !is_gt) ? (val-1) : INT_MAX;
		this->s_min = (target == 's' && is_gt) ? (val+1) : INT_MIN;
		this->s_max = (target == 's' && !is_gt) ? (val-1) : INT_MAX;
		this->dest = dest;
	}
};

struct Item
{
	int x;
	int m;
	int a;
	int s;

	//Item ctor
	Item(const std::string& item)
	{
		int last_comma = 0;
		int comma;

		comma = item.find(',');
		this->x = std::stoi(item.substr(2, comma - 2));
		last_comma = comma + 1;

		comma = item.find(',', last_comma);
		this->m = std::stoi(item.substr(last_comma + 2, comma - last_comma - 2));
		last_comma = comma + 1;

		comma = item.find(',', last_comma);
		this->a = std::stoi(item.substr(last_comma + 2, comma - last_comma - 2));
		last_comma = comma + 1;

		this-> s = std::stoi(item.substr(last_comma + 2));
	}
};

struct ItemRange
{
	int x_min;
	int x_max;
	int m_min;
	int m_max;
	int a_min;
	int a_max;
	int s_min;
	int s_max;

	//ItemRange ctor
	ItemRange()
	{
		this->x_min = 1;
		this->x_max = 4000;
		this->m_min = 1;
		this->m_max = 4000;
		this->a_min = 1;
		this->a_max = 4000;
		this->s_min = 1;
		this->s_max = 4000;
	}

	//override equality
	bool operator==(const ItemRange& other) const
	{
		return this->x_min == other.x_min &&
			this->x_max == other.x_max &&
			this->m_min == other.m_min &&
			this->m_max == other.m_max &&
			this->a_min == other.a_min &&
			this->a_max == other.a_max &&
			this->s_min == other.s_min &&
			this->s_max == other.s_max;
	}
};

//forward declare
std::pair<std::unordered_map<std::string, std::vector<Rule>>, std::vector<Item>> GetInput(const std::string& input_file);
std::size_t SumAcceptedRatings(const std::unordered_map<std::string, std::vector<Rule>>& workflows, const std::vector<Item>& items);
std::size_t WhackyRangeCalculator(const std::unordered_map<std::string, std::vector<Rule>>& workflows);

class Day19Solution : public AoCSolution
{
public:
	void RunFirstPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		std::size_t sum = SumAcceptedRatings(input.first, input.second);
		std::cout << "Sum of accepted ratings: " << sum << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		auto input = GetInput(input_file);
		std::size_t sum = WhackyRangeCalculator(input.first);
		std::cout << "Sum of accepted ratings: " << sum << std::endl;
	}
};

std::size_t WhackyRangeCalculator(const std::unordered_map<std::string, std::vector<Rule>>& workflows)
{
	std::size_t accepted = 0;
	std::vector<ItemRange> acceptedRanges;

	std::queue<std::pair<ItemRange, std::vector<Rule>>> currentRanges;
	currentRanges.push(std::make_pair(ItemRange(), workflows.at("in")));

	while (!currentRanges.empty())
	{
		auto current = currentRanges.front();
		currentRanges.pop();

		ItemRange range = current.first;
		//split the range at the next rule
		for (const auto& rule : current.second)
		{
			//create a copy of the range's current status
			ItemRange otherRange = range;
			//check if the rule applies to the range
			//a rule is always ONLY min/max, which we can just create a single split index
			//anyway this is the big copy paste
			if(rule.x_min > range.x_min)
			{
				otherRange.x_min = rule.x_min;
				range.x_max = rule.x_min - 1;
			}
			else if(rule.x_max < range.x_max)
			{
				otherRange.x_max = rule.x_max;
				range.x_min = rule.x_max + 1;
			}
			else if (rule.m_min > range.m_min)
			{
				otherRange.m_min = rule.m_min;
				range.m_max = rule.m_min - 1;
			}
			else if (rule.m_max < range.m_max)
			{
				otherRange.m_max = rule.m_max;
				range.m_min = rule.m_max + 1;
			}
			else if (rule.a_min > range.a_min)
			{
				otherRange.a_min = rule.a_min;
				range.a_max = rule.a_min - 1;
			}
			else if (rule.a_max < range.a_max)
			{
				otherRange.a_max = rule.a_max;
				range.a_min = rule.a_max + 1;
			}
			else if (rule.s_min > range.s_min)
			{
				otherRange.s_min = rule.s_min;
				range.s_max = rule.s_min - 1;
			}
			else if (rule.s_max < range.s_max)
			{
				otherRange.s_max = rule.s_max;
				range.s_min = rule.s_max + 1;
			}
			//we now have "range" which does not adhere to the rule, and "otherRange" which does
			//IF the other range is the same as the current range, that means the rule did not change anything, and we can discard the other range and continue with the current range
			if (otherRange == range)
			{
				continue;
			}
			//Otherwise, we need to determine what to do with other range based on its destination
			else
			{
				//if the destination is A, we can add the range to the accepted ranges
				if (rule.dest == "A")
				{
					acceptedRanges.push_back(otherRange);
				}
				//if the destination is R, we can discard the range
				else if (rule.dest == "R")
				{
					continue;
				}
				//if the destination is not R or A, we need to add the range to the current ranges
				else
				{
					currentRanges.push(std::make_pair(otherRange, workflows.at(rule.dest)));
				}
			}
		}
		//now we have a range left that adheres to all rules, so we need to add it to the destination of the last rule
		//if the last rule was R, we can discard the range
		if(current.second.back().dest == "R")
		{
			continue;
		}
		//if the last rule was A, we can add the range to the accepted ranges
		else if (current.second.back().dest == "A")
		{
			acceptedRanges.push_back(range);
		}
		//if the last rule was not R or A, we need to add the range to the current ranges
		else
		{
			currentRanges.push(std::make_pair(range, workflows.at(current.second.back().dest)));
		}
	}

	//now we have all the accepted ranges, we need to sum them up
	for (const auto& range : acceptedRanges)
	{
		std::size_t rangeSum = (range.x_max - range.x_min + 1);
		rangeSum *= (range.m_max - range.m_min + 1);
		rangeSum *= (range.a_max - range.a_min + 1);
		rangeSum *= (range.s_max - range.s_min + 1);
		accepted += rangeSum;
	}

	return accepted;
}

std::size_t SumAcceptedRatings(const std::unordered_map<std::string, std::vector<Rule>>& workflows, const std::vector<Item>& items)
{
	std::size_t sum = 0;
	std::vector<Rule> rules;
	for(const auto& item : items)
	{
		rules = workflows.at("in");
		bool found = false;
		while (!found)
		{
			for (const auto& rule : rules)
			{
				if (item.x >= rule.x_min && item.x <= rule.x_max &&
					item.m >= rule.m_min && item.m <= rule.m_max &&
					item.a >= rule.a_min && item.a <= rule.a_max &&
					item.s >= rule.s_min && item.s <= rule.s_max)
				{
					if (rule.dest == "A")
					{
						found = true;
						sum += item.x + item.m + item.a + item.s;
					}
					else if (rule.dest == "R")
					{
						found = true;
					}
					else if(rule.dest != "R")
					{
						rules = workflows.at(rule.dest);
					}
					break;
				}
			}
		}
	}
	return sum;
}

std::pair<std::unordered_map<std::string, std::vector<Rule>>, std::vector<Item>> GetInput(const std::string & input_file)
{
	std::unordered_map<std::string, std::vector<Rule>> workflows;
	std::vector<Item> items;

	std::ifstream file(input_file);
	std::string line;

	bool parse_rules = true;
	while(std::getline(file, line))
	{
		//toggle from rules to items
		if(line.empty())
		{
			parse_rules = false;
			continue;
		}

		if (parse_rules)
		{
			//get opening bracket {
			int bracket = line.find('{');
			std::string name = line.substr(0, bracket);
			std::string rules = line.substr(bracket + 1, line.size() - bracket - 2) + ",";
			int comma;
			while((comma = rules.find(',')) != std::string::npos)
			{
				std::string rule = rules.substr(0, comma);
				rules = rules.substr(comma + 1);
				workflows[name].push_back(Rule(rule));
			}
		}
		else
		{
			//remove opening and closing bracket
			line = line.substr(1, line.size() - 2);
			items.push_back(Item(line));
		}
	}

	return std::make_pair(workflows, items);
}