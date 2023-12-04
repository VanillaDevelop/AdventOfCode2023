#include <iostream>
#include <string>
#include <fstream>
#include <unordered_set>


int main()
{
	std::ifstream infile("Inputs/day4-1.txt");
	std::string line;

	std::vector<int> winning_count;

	while (std::getline(infile, line))
	{
		std::unordered_set<int> winning_numbers;
		//separate at :
		std::string::size_type n = line.find(":");
		//input starts 1 character after the :
		std::string input = line.substr(n + 2, line.size() - n - 2);

		//split by | into winning numbers and own numbers
		n = input.find("|");
		std::string winning_numbers_string = input.substr(0, n - 1);
		std::string own_numbers_string = input.substr(n + 2, input.size() - n - 2);

		int num = 0;
		//I didn't really wanna do this whacky logic for parsing the numbers again, but I didn't wanna deal with double space for single digit numbers.
		for (int i = 0; i < winning_numbers_string.size(); i++)
		{
			if (isdigit(winning_numbers_string[i]))
			{
				num = num * 10 + (winning_numbers_string[i] - '0');
			}
			else
			{
				if (num > 0)
				{
					winning_numbers.insert(num);
					num = 0;
				}
			}
		}
		if (num > 0)
		{
			winning_numbers.insert(num);
			num = 0;
		}

		//now parse own numbers and compare with set
		int win_count = 0;

		for (int i = 0; i < own_numbers_string.size(); i++)
		{
			if (isdigit(own_numbers_string[i]))
			{
				num = num * 10 + (own_numbers_string[i] - '0');
			}
			else
			{
				if (num > 0)
				{
					if (winning_numbers.find(num) != winning_numbers.end())
					{
						win_count++;
					}
					num = 0;
				}
			}
		}
		if (num > 0)
		{
			if (winning_numbers.find(num) != winning_numbers.end())
			{
				win_count++;
			}
		}

		//add to vector
		winning_count.push_back(win_count);
	}

	//to calculate the total number of scratch cards
	std::vector<int> counts;
	for (int i = 0; i < winning_count.size(); i++)
	{
		//each card exists once by default
		counts.push_back(1);
	}

	for (int i = 0; i < winning_count.size(); i++)
	{
		if (winning_count[i] > 0)
		{
			for (int j = i + 1; j < i + 1 + winning_count[i]; j++)
			{
				counts[j] += counts[i];
			}
		}
	}

	//sum all cards
	int all_score = 0;
	for (int i = 0; i < counts.size(); i++)
	{
		all_score += counts[i];
	}

	//print the count for each card
	for (int i = 0; i < counts.size(); i++)
	{
		std::cout << "Card " << i << ": " << counts[i] << std::endl;
	}

	std::cout << "Total cards: " << all_score << std::endl;
}