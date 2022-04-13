#include "CustomContainer.h" // CustomContainer also includes iostream, so no need to reinclude it here
#include <numeric> // std::accumulate
#include <chrono> // std::chrono
#include <vector> // std::vector
#include <fstream> // std::ofstream
#include <algorithm> // std::max_element, std::min_element, std::remove_if

#define UNIT_TESTS
#ifdef UNIT_TESTS
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Testing The Container")
{
	std::vector<int> test(std::vector<int>{});
	CustomContainer<int> intContainer{};

	REQUIRE(intContainer.IsEmpty());

	intContainer.Add(0);
	
	REQUIRE(intContainer.Size() == 1);

	intContainer.Clear();
	REQUIRE(intContainer.Size() == 0);
	REQUIRE(intContainer.IsEmpty());

	for (int i{}; i < 20; ++i)
		intContainer.Add(i);

	REQUIRE(intContainer.Front() == 0);
	REQUIRE(intContainer.Back() == 19);
	REQUIRE(intContainer.Size() == 20);

	intContainer[0] = 1;
	REQUIRE(intContainer.Front() == 1);

	REQUIRE(!intContainer.IsEmpty());

	intContainer.Clear();

	REQUIRE(intContainer.IsEmpty());
	REQUIRE(intContainer.Size() == 0);

	for (int i{}; i < 20; ++i)
		intContainer.Add(i);

	REQUIRE(intContainer.Size() == 20);

	CustomContainer<int> copyConstructor{ intContainer };

	for (size_t i{}; i < copyConstructor.Size(); ++i)
		REQUIRE(copyConstructor.At(i) == intContainer.At(i));

	REQUIRE(intContainer.Back() == 19);
	REQUIRE(copyConstructor.Back() == 19);

	copyConstructor.Clear();
	
	REQUIRE(copyConstructor.IsEmpty());
	REQUIRE(!intContainer.IsEmpty());

	CustomContainer<int> moveConstructor{ std::move(intContainer) };

	REQUIRE(moveConstructor.Front() == 0);
	REQUIRE(moveConstructor.Back() == 19);
	REQUIRE(moveConstructor.Size() == 20);

	CustomContainer<int> copyOperator = moveConstructor;

	for (size_t i{}; i < copyOperator.Size(); ++i)
		REQUIRE(copyOperator[i] == moveConstructor[i]);

	CustomContainer<int> moveOperator = std::move(copyOperator);

	REQUIRE(moveOperator.Front() == 0);
	REQUIRE(moveOperator.Back() == 19);
	REQUIRE(moveOperator.Size() == 20);

	REQUIRE(moveOperator.Data() == &moveOperator.Front());
	
	moveOperator.Reserve(100);
	REQUIRE(moveOperator.Capacity() == 100);

	moveOperator.ShrinkToFit();
	REQUIRE(moveOperator.Capacity() == moveOperator.Size());
}

#else
int main(int argc, char* argv[])
{
	using Timepoint = std::chrono::high_resolution_clock::time_point;

	std::ofstream file{};
	file.open("BenchmarksDebugX64.txt");
	//file.open("BenchmarksReleaseX64.txt");

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
			bool hasValueBeenErased{};
			const long long minElement{ *(std::min_element(times.cbegin(), times.cend())) };
			times.erase(std::remove_if(times.begin(), times.end(), [&hasValueBeenErased, minElement](const long long a)->bool
				{
					if (!hasValueBeenErased)
					{
						if (minElement == a)
						{
							hasValueBeenErased = true;
							return true;
						}
					}
					return false;
				}), times.end());

			hasValueBeenErased = true;
			const long long maxElement{ *(std::max_element(times.cbegin(), times.cend())) };
			times.erase(std::remove_if(times.begin(), times.end(), [&hasValueBeenErased, maxElement](const long long a)->bool
				{
					if (!hasValueBeenErased)
					{
						if (maxElement == a)
						{
							hasValueBeenErased = true;
							return true;
						}
					}
					return false;
				}), times.end());

			file << i << " " << "STL: " << std::fixed << std::accumulate(times.cbegin(), times.cend(), 0.f) / times.size() << std::endl;
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
			bool hasValueBeenErased{};
			const long long minElement{ *(std::min_element(times.cbegin(), times.cend())) };
			times.erase(std::remove_if(times.begin(), times.end(), [&hasValueBeenErased, minElement](const long long a)->bool
				{
					if (!hasValueBeenErased)
					{
						if (minElement == a)
						{
							hasValueBeenErased = true;
							return true;
						}
					}
					return false;
				}), times.end());

			hasValueBeenErased = true;
			const long long maxElement{ *(std::max_element(times.cbegin(), times.cend())) };
			times.erase(std::remove_if(times.begin(), times.end(), [&hasValueBeenErased, maxElement](const long long a)->bool
				{
					if (!hasValueBeenErased)
					{
						if (maxElement == a)
						{
							hasValueBeenErased = true;
							return true;
						}
					}
					return false;
				}), times.end());

			file << i << " " << "CUSTOM: " << std::fixed << std::accumulate(times.cbegin(), times.cend(), 0.f) / times.size() << std::endl;
		}
	}

	file.close();

	return 0;
}
#endif // UNIT_TESTS