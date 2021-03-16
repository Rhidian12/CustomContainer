#include "CustomContainer.h" // CustomContainer also includes iostream, so no need to reinclude it here
#include <numeric> // std::accumulate
#include <chrono> // std::chrono
#include <vector> // std::vector

int main(int argc, char* argv[])
{
	using Timepoint = std::chrono::high_resolution_clock::time_point;

	std::vector<float> timeOfSTL{};
	std::vector<float> timeOfCustom{};

	const int amountOfIterations{ 500 };
	const int amountOfPushbacks{ 500 };

	std::cout << "Amount of Iterations: " << amountOfIterations << std::endl;
	std::cout << "Amount of push_back: " << amountOfPushbacks << std::endl;

	for (int i{}; i < amountOfIterations; ++i)
	{
		const Timepoint t1{ std::chrono::high_resolution_clock::now() };

		std::vector<int> vector{};

		for (int j{}; j < amountOfPushbacks; ++j)
			vector.push_back(j);

		const size_t size = vector.size();
		const size_t capacity = vector.capacity();
		const int front = vector.front();
		const int back = vector.back();
		const bool isEmpty = vector.empty();
		vector.clear();

		const Timepoint t2{ std::chrono::high_resolution_clock::now() };

		const float time = std::chrono::duration<float>(t2 - t1).count();
		timeOfSTL.push_back(time);
	}
	for (int i{}; i < amountOfIterations; ++i)
	{
		const Timepoint t1{ std::chrono::high_resolution_clock::now() };

		CustomContainer<int> custom{};

		for (int j{}; j < amountOfPushbacks; ++j)
			custom.Push_back(j);

		const size_t size = custom.GetSize();
		const size_t capacity = custom.GetCapacity();
		const int front = custom.GetFront();
		const int back = custom.GetBack();
		const bool isEmpty = custom.IsEmpty();
		custom.Clear();

		const Timepoint t2{ std::chrono::high_resolution_clock::now() };

		const float time = std::chrono::duration<float>(t2 - t1).count();
		timeOfCustom.push_back(time);
	}

	std::cout << "STL Average: " << std::accumulate(timeOfSTL.begin(), timeOfSTL.end(), 0.f) << std::endl;
	std::cout << "Custom Average: " << std::accumulate(timeOfCustom.begin(), timeOfCustom.end(), 0.f) << std::endl;

	return 0;
}