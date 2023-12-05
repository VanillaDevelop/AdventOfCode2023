#pragma once
#include <string>

/// <summary>
/// Abstract class for AoC solutions
/// </summary>
class AoCSolution
{
	public:
		virtual void RunFirstPart(std::string input_file) = 0;
		virtual void RunSecondPart(std::string input_file) = 0;
};

