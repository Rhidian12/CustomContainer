#include "CustomContainer.h" // CustomContainer also includes iostream, so no need to reinclude it here
#include <numeric> // std::accumulate
#include <chrono> // std::chrono
#include <vector> // std::vector
#include <fstream> // std::ofstream
#include <algorithm> // std::max_element

int main(int argc, char* argv[])
{
	using Timepoint = std::chrono::high_resolution_clock::time_point;

	std::ofstream file{};
	file.open("BenchmarksReleaseX64.txt");

	const int amountOfIterations{ 100 };
	const int amountOfPushbacks{ 50 };

	std::cout << "Amount of Iterations: " << amountOfIterations << std::endl;
	std::cout << "Amount of push_back: " << amountOfPushbacks << std::endl;
	file << "Amount of push_back: " << amountOfPushbacks << std::endl;

	for (int i{}; i < amountOfIterations; ++i)
	{
		{
			std::vector<long long> times{};
			for (int j{}; j < amountOfIterations; ++j)
			{
				const Timepoint t1{ std::chrono::high_resolution_clock::now() };

				std::vector<int> vector{};

				for (int k{}; k < amountOfPushbacks * i; ++k)
					vector.push_back(k);

				const Timepoint t2{ std::chrono::high_resolution_clock::now() };

				const long long time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
				times.push_back(time);
			}
			times.erase(std::remove(times.begin(), times.end(), *(std::max_element(times.cbegin(), times.cend()))), times.end());
			times.erase(std::remove(times.begin(), times.end(), *(std::min_element(times.cbegin(), times.cend()))), times.end());

			file << i << " " << "STL: " << std::fixed << std::accumulate(times.cbegin(), times.cend(), 0.f) << std::endl;
		}
		{
			std::vector<long long> times{};
			for (int j{}; j < amountOfIterations; ++j)
			{
				const Timepoint t1{ std::chrono::high_resolution_clock::now() };

				CustomContainer<int> custom{};

				for (int k{}; k < amountOfPushbacks * i; ++k)
					custom.Push_back(k);

				const Timepoint t2{ std::chrono::high_resolution_clock::now() };

				const long long time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
				times.push_back(time);
			}
			times.erase(std::remove(times.begin(), times.end(), *(std::max_element(times.cbegin(), times.cend()))), times.end());
			times.erase(std::remove(times.begin(), times.end(), *(std::min_element(times.cbegin(), times.cend()))), times.end());

			file << i << " " << "CUSTOM: " << std::fixed << std::accumulate(times.cbegin(), times.cend(), 0.f) << std::endl;
		}
	}

	file.close();

	return 0;
}