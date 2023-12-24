#pragma once
#include "AoCSolution.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <optional>

struct Trajectory
{
	long long x;
	long long y;
	long long z;
	long long v_x;
	long long v_y;
	long long v_z;
};

std::vector<Trajectory> GetTrajectories(const std::string& input_file)
{
	std::string line;
	std::ifstream input(input_file);
	std::vector<Trajectory> trajectories;

	while (std::getline(input, line))
	{
		Trajectory trajectory;
		sscanf_s(line.c_str(), "%I64d, %I64d, %I64d @ %I64d, %I64d, %I64d", &trajectory.x, &trajectory.y, &trajectory.z, &trajectory.v_x, &trajectory.v_y, &trajectory.v_z);
		trajectories.push_back(trajectory);
	}

	return trajectories;
}

bool WillTrajectoriesIntersect(const Trajectory& trajectory1, const Trajectory& trajectory2, double min_pos, double max_pos)
{
	double m1 = (double)trajectory1.v_y / (double)trajectory1.v_x;
	double c1 = trajectory1.y - m1 * trajectory1.x;
	double m2 = (double)trajectory2.v_y / (double)trajectory2.v_x;
	double c2 = trajectory2.y - m2 * trajectory2.x;

	//check if parallel
	if (fabs(m1 - m2) < 0.000001)
	{
		return false;
	}

	double intersectionX = (c2 - c1) / (m1 - m2);
	double intersectionY = m1 * intersectionX + c1;

	if(intersectionX >= min_pos && intersectionX <= max_pos && intersectionY >= min_pos && intersectionY <= max_pos)
	{
		//calculate time of intersection, check if its > 0
		double t1 = (intersectionX - trajectory1.x) / trajectory1.v_x;
		double t2 = (intersectionX - trajectory2.x) / trajectory2.v_x;

		return t1 >= 0 && t2 >= 0;
	}
	else
	{
		return false;
	}
}

class Day24Solution : public AoCSolution
{
	void RunFirstPart(std::string input_file) override
	{
		auto trajectories = GetTrajectories(input_file);

		int sum_collisions = 0;
		for (int i = 0; i < trajectories.size(); i++)
		{
			for (int j = i + 1; j < trajectories.size(); j++)
			{
				if (WillTrajectoriesIntersect(trajectories[i], trajectories[j], 200000000000000, 400000000000000))
				{
					sum_collisions++;
				}
			}
		}

		std::cout << "Sum of collisions: " << sum_collisions << std::endl;
	}

	void RunSecondPart(std::string input_file) override
	{
		auto trajectories = GetTrajectories(input_file);

		std::cout << "t0, t1, t2, mRx, mRy, mRz, bRx, bRy, bRz = var('t0', 't1', 't2', 'mRx', 'mRy', 'mRz', 'bRx', 'bRy', 'bRz')" << std::endl;
		std::cout << "solve([";
		bool first = true;
		for (int i = 0; i < 3;i++) 
		{
			const auto& hail = trajectories[i];
			if (first) 
			{
				first = false;
			}
			else {
				std::cout << ", ";
			}
			printf("%I64d*t%d+%I64d == mRx*t%d+bRx, ", hail.v_x, i, hail.x, i);
			printf("%I64d*t%d+%I64d == mRy*t%d+bRy, ", hail.v_y, i, hail.y, i);
			printf("%I64d*t%d+%I64d == mRz*t%d+bRz", hail.v_z, i, hail.z, i);
		}
		std::cout << "], t0, t1, t2, mRx, mRy, mRz, bRx, bRy, bRz)" << std::endl;
	}
};

