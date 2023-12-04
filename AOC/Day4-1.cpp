#include <iostream>
#include <string>
#include <fstream>
#include <unordered_set>


int main()
{
	std::ifstream infile("Inputs/day4-1.txt");
	std::string line;

	int all_score = 0;

	while (std::getline(infile, line))
	{
		std::unordered_set<int> winning_numbers;
		//separate at :
		std::string::size_type n = line.find(":");
		//input starts 1 character after the :
		std::string input = line.substr(n + 2, line.size() - n - 2);

		//split by | into winning numbers and own numbers
		n = input.find("|");
		std::string winning_numbers_string = input.substr(0, n-1);
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
		int final_score = 0;

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
						if(final_score == 0)
							final_score = 1;
						else
							final_score *= 2;
					}
					num = 0;
				}
			}
		}
		if (num > 0)
		{
			if (winning_numbers.find(num) != winning_numbers.end())
			{
				if (final_score == 0)
					final_score = 1;
				else
					final_score *= 2;
			}
			num = 0;
		}

		//add to total score
		all_score += final_score;
	}

	std::cout << "Total score: " << all_score << std::endl;
}