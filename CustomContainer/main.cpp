#include "CustomContainer.h" // CustomContainer also includes iostream, so no need to reinclude it here
#include <numeric> // std::accumulate
#include <chrono> // std::chrono
#include <vector> // std::vector
#include <fstream> // std::ofstream
#include <algorithm> // std::max_element, std::min_element, std::remove_if
#include <deque> /* std::deque */

#include <vld.h>

#include "PoolAllocator.h"

#define UNIT_TESTS
#ifdef UNIT_TESTS
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Testing The Container")
{
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

	moveOperator.Pop();
	REQUIRE(moveOperator.Back() == 18);

	moveOperator.Clear();
	moveOperator.Add(420);
	moveOperator.Pop();
	REQUIRE(moveOperator.Size() == 0);
	REQUIRE(moveOperator.IsEmpty());

	for (int i{}; i < 20; ++i)
		moveOperator.Add(i);

	moveOperator.Resize(50);
	REQUIRE(moveOperator.Size() == 50);

	moveOperator.Resize(10);
	REQUIRE(moveOperator.Size() == 10);

	for (int& i : moveOperator)
	{
		i += 5;
	}

	REQUIRE(moveOperator.Size() == 10);
	REQUIRE(moveOperator[0] == 5);
	REQUIRE(moveOperator.Front() == 5);
	REQUIRE(moveOperator.At(0) == 5);

	for (const int& i : moveOperator)
	{
		/* Will never fire, this for-loop should just compile */
		if (i == -1)
		{
			std::cout << "UwU\n";
		}
	}

	REQUIRE(*moveOperator.cbegin() == moveOperator.Front());
	REQUIRE(*moveOperator.begin() == moveOperator.Front());

	PoolAllocator::ReleaseAllMemory();
}

#else
int main(int argc, char* argv[])
{
	using Timepoint = std::chrono::steady_clock::time_point;

	// std::ofstream file{};
	// file.open("BenchmarksDebugX64.txt");
	// file.open("BenchmarksReleaseX64.txt");

	const int amountOfIterations{ 100 };
	const int amountOfPushbacks{ 500 };

	std::cout << "Amount of Iterations: " << amountOfIterations << std::endl;
	std::cout << "Amount of push_back: " << amountOfPushbacks << std::endl;
	// file << "Amount of push_back: " << amountOfPushbacks << std::endl;

	std::deque<long long> stlTimes{};
	std::deque<long long> customTimes{};

	Timepoint t1{};
	Timepoint t2{};

	for (int i{}; i < amountOfIterations; ++i)
	{
		t1 = std::chrono::high_resolution_clock::now();

		std::vector<int> vector{};

		for (int k{}; k < amountOfPushbacks; ++k)
			vector.push_back(k);

		t2 = std::chrono::high_resolution_clock::now();

		stlTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

		t1 = std::chrono::high_resolution_clock::now();

		CustomContainer<int> container{};

		for (int k{}; k < amountOfPushbacks; ++k)
			container.Add(k);

		t2 = std::chrono::high_resolution_clock::now();

		customTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
	}

	for (int j{}; j < amountOfIterations / 10; ++j)
	{
		stlTimes.pop_back();
		stlTimes.pop_front();

		customTimes.pop_back();
		customTimes.pop_front();
	}

	std::cout << "STL Time Average (in nanoseconds): " << std::accumulate(stlTimes.cbegin(), stlTimes.cend(), (long long)0) / stlTimes.size() << "\n";
	std::cout << "Custom Time Average (in nanoseconds): " << std::accumulate(customTimes.cbegin(), customTimes.cend(), (long long)0) / customTimes.size() << "\n";

	// file.close();

	return 0;
}
#endif // UNIT_TESTS