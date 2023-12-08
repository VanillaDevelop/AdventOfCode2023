#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <map>

//Define constant strings for the card values in the 1st and 2nd parts of the problem.
const std::string CARD_VALUES_P1 = "AKQJT98765432";
const std::string CARD_VALUES_P2 = "AKQT98765432J";

//Forward declarations
struct Hand;
bool compareHands(const Hand& first_hand, const Hand& second_hand, bool is_p2);
int getHandType(const std::string& hand, bool is_p2);
bool handTiebreaker(const std::string& first_hand, const std::string& second_hand, const std::string& card_values);
std::vector<Hand> getHands(std::ifstream& infile, bool is_p2);

/// <summary>
/// Define a hand as a string of cards and a bid, and overload the < operator to compare hands depending on the is_p1_hand flag.
/// </summary>
struct Hand
{
	std::string cards;
	int bid;
	bool is_p2;

	bool operator<(const Hand& other) const
	{
		return compareHands(*this, other, is_p2);
	}
};

/// <summary>
/// Comparator function for hands (is hand 1 worse than hand 2?)
/// </summary>
/// <param name="first_hand">The first hand</param>
/// <param name="second_hand">The second hand</param>
/// <param name="is_p2">Whether to use 2nd part or 1st part values</param>
/// <returns>Whether or not the first hand is worse than the second hand</returns>
bool compareHands(const Hand& first_hand, const Hand& second_hand, bool is_p2)
{
	int first_hand_type = getHandType(first_hand.cards, is_p2);
	int second_hand_type = getHandType(second_hand.cards, is_p2);

	if (first_hand_type > second_hand_type)
	{
		return false;
	}
	else if (first_hand_type < second_hand_type)
	{
		return true;
	}
	else
	{
		return !handTiebreaker(first_hand.cards, second_hand.cards, is_p2 ? CARD_VALUES_P2: CARD_VALUES_P1);
	}
}

/// <summary>
/// Get a vector of hands from an input file
/// </summary>
/// <param name="infile">The input file</param>
/// <returns>A vector of hand struct objects.</returns>
std::vector<Hand> getHands(std::ifstream& infile, bool is_p2)
{
	std::vector<Hand> hands;
	std::string line;

	while (std::getline(infile, line))
	{
		std::pair<std::string, int> hand;
		std::stringstream ss(line);
		std::string cards;
		std::string bid_str;

		ss >> cards;
		hand.first = cards;

		ss >> bid_str;
		hand.second = std::stoi(bid_str);

		hands.push_back({
				cards,
				std::stoi(bid_str),
				is_p2
			}
		);
	}

	return hands;
}


/// <summary>
/// Distinguish between the different types of hands (higher value = better hand)
/// </summary>
/// <param name="hand">A string containing the hand</param>
/// <param name="is_p2">Whether to calculate hand type based on P1 or P2</param>
/// <returns>An integer based on the value of the hand</returns>
int getHandType(const std::string& hand, bool is_p2)
{
	std::unordered_map<char, int> char_count;

	//if p2, we set a joker_count variable and remove all jokers from the hand calculation
	std::stringstream hand_stream;
	int joker_count = 0;
	if (is_p2)
	{
		for (const char c : hand)
		{
			if (c == 'J')
			{
				joker_count++;
			}
			else
			{
				hand_stream << c;
			}
		}
	}
	else
	{
		hand_stream << hand;
	}

	//Count the number of each card in the remaining hand
	for (const char c : hand_stream.str())
	{
		char_count[c]++;
	}

	//Order the char_count map by value (descending)
	std::vector<std::pair<char, int>> char_count_vec(char_count.begin(), char_count.end());
	std::sort(char_count_vec.begin(), char_count_vec.end(), [](const std::pair<char, int>& a, const std::pair<char, int>& b) { return a.second > b.second; });

	bool has_pair = false;
	bool has_three = false;
	
	//funy edge case where we have 5 jokers (thx bruh)
	if(joker_count == 5)
	{
		return 6;
	}

	for (const std::pair<const char, const int> & kv : char_count_vec)
	{
		if (kv.second + joker_count == 5)
		{
			//instantly 5 of a kind
			return 6;
		}
		if (kv.second + joker_count == 4)
		{
			//instantly 4 of a kind with no jokers left = done
			return 5;
		}
		if (kv.second + joker_count == 3)
		{
			//we have a three of a kind (at least)
			has_three = true;
			if (joker_count > 0) joker_count -= (3 - kv.second);
			continue;
		}
		if (kv.second + joker_count == 2)
		{
			//if we already have a three of a kind, then we have a full house = done
			if (has_three)
			{
				return 4;
			}
			//if we already have a pair, then we have two pairs = done
			if (has_pair)
			{
				return 2;
			}
			//otherwise, we have a pair (at least)
			has_pair = true;
			if (joker_count > 0) joker_count -= (2 - kv.second);
			continue;
		}
	}
	if (has_three)
	{
		//three of a kind
		return 3;
	}
	else if (has_pair)
	{
		//single pair
		return 1;
	}
	else
	{
		//high card
		return 0;
	}
}

/// <summary>
/// Return a boolean indicating whether the first hand has a higher tiebreaker card than the second hand
/// </summary>
bool handTiebreaker(const std::string & first_hand, const std::string & second_hand, const std::string & card_values)
{
	char first_hand_char;
	char second_hand_char;
	std::stringstream first_hand_ss(first_hand);
	std::stringstream second_hand_ss(second_hand);

	while (true)
	{
		try
		{
			first_hand_ss >> first_hand_char;
			second_hand_ss >> second_hand_char;
			//Check which character occurs first in the CARD_VALUES string
			if (card_values.find(first_hand_char) < card_values.find(second_hand_char))
			{
				return true;
			}
			else if (card_values.find(first_hand_char) > card_values.find(second_hand_char))
			{
				return false;
			}
		}
		catch (std::exception e)
		{
			// If we've reached the end of the hand, then the hands are considered "equal" (don't think this should really happen anyhow)
			return false;
		}
	}
}

class Day7Solution : public AoCSolution
{
	public:
		void RunFirstPart(std::string input_file) override
		{
			std::ifstream infile(input_file);

			std::vector<Hand> hands = getHands(infile, false);

			std::sort(hands.begin(), hands.end());

			int score = 0;
			for(int i = 0; i < hands.size(); i++)
			{
				score += hands[i].bid * (i + 1);
			}

			std::cout << "Score: " << score << std::endl;
		}

		void RunSecondPart(std::string input_file) override
		{
			std::ifstream infile(input_file);

			std::vector<Hand> hands = getHands(infile, true);

			std::sort(hands.begin(), hands.end());

			int score = 0;
			for (int i = 0; i < hands.size(); i++)
			{
				std::cout << hands[i].cards << std::endl;
				score += hands[i].bid * (i + 1);
			}

			std::cout << "Score: " << score << std::endl;
		}
};


