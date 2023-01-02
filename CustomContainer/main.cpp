#include "CustomContainer.h" // CustomContainer also includes iostream, so no need to reinclude it here
#include <numeric> // std::accumulate
#include <chrono> // std::chrono
#include <vector> // std::vector
#include <fstream> // std::ofstream
#include <algorithm> // std::max_element, std::min_element, std::remove_if
#include <deque> /* std::deque */

#include <vld.h>

//#define UNIT_TESTS
#ifdef UNIT_TESTS
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define ARRAY_TESTS
#ifdef ARRAY_TESTS

TEST_CASE("Testing Basic Array of integers")
{
	Array<int> arr{};

	REQUIRE(arr.Capacity() == 0);
	REQUIRE(arr.Size() == 0);
	REQUIRE(arr.Empty());

	const int nrOfElements{ 10 };

	SECTION("Adding 10 elements which causes several reallocations")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		REQUIRE(arr.Size() == 10);
		REQUIRE(arr.Capacity() >= 10);
		REQUIRE(arr.Front() == 0);
		REQUIRE(arr.Back() == nrOfElements - 1);
		REQUIRE(arr[0] == 0);
		REQUIRE(arr[arr.Size() - 1] == nrOfElements - 1);
		REQUIRE(arr.At(arr.Size() - 1) == nrOfElements - 1);

		arr[0] = 15;
		REQUIRE(arr.Front() == 15);
	}

	SECTION("Reserving and adding elements")
	{
		arr.Reserve(nrOfElements);

		REQUIRE(arr.Capacity() == 10);

		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		REQUIRE(arr.Size() == 10);
		REQUIRE(arr.Capacity() == 10);
		REQUIRE(arr.Front() == 0);
		REQUIRE(arr.Back() == nrOfElements - 1);
		REQUIRE(arr[0] == 0);
		REQUIRE(arr[arr.Size() - 1] == nrOfElements - 1);
	}

	SECTION("Clearing and removing elements")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		arr.Pop();

		REQUIRE(arr.Size() == 9);
		REQUIRE(arr.Capacity() >= 10);

		for (size_t i{}; i < 5; ++i)
		{
			arr.Pop();
		}

		REQUIRE(arr.Size() == 4);
		REQUIRE(arr.Capacity() >= 10);

		arr.Clear();

		REQUIRE(arr.Size() == 0);
		REQUIRE(arr.Capacity() >= 10);
	}

	SECTION("Shrinking to size")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		arr.Pop();
		arr.Pop();
		arr.Pop();

		arr.ShrinkToFit();

		REQUIRE(arr.Capacity() == arr.Size());
	}

	SECTION("Resizing array")
	{
		arr.Resize(nrOfElements);

		for (int i{}; i < nrOfElements; ++i)
		{
			REQUIRE(arr[i] == 0);
		}

		arr.Clear();

		arr.Resize(nrOfElements, 15);

		for (int i{}; i < nrOfElements; ++i)
		{
			REQUIRE(arr[i] == 15);
		}
	}

	SECTION("Adding elements only through insertion")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Insert(i, i);
		}

		REQUIRE(arr[0] == 0);
		REQUIRE(arr.Size() == nrOfElements);
		REQUIRE(arr.Capacity() >= nrOfElements);
	}

	SECTION("Inserting elements into the array")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		arr.Insert(1, 15);

		REQUIRE(arr.Size() == nrOfElements + 1);
		REQUIRE(arr[1] == 15);
	}

	SECTION("Making an array with non-trivial destructor type")
	{
		class Special
		{
		public:
			~Special()
			{
				std::cout << "Getting Destroyed\n";
			}
		};

		Array<Special> specialArr{};

		for (int i{}; i < nrOfElements; ++i)
		{
			specialArr.Add(Special{});
		}

		specialArr.Clear();
	}

	SECTION("Testing copy ctor")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		Array<int> newArr{ arr };

		REQUIRE(newArr.Size() == arr.Size());
		REQUIRE(newArr.Capacity() == arr.Capacity());
		REQUIRE(newArr.Data() != arr.Data());

		for (int i{}; i < nrOfElements; ++i)
		{
			REQUIRE(newArr[i] == arr[i]);
		}
	}

	SECTION("Testing copy operator")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		Array<int> newArr = arr;

		REQUIRE(newArr.Size() == arr.Size());
		REQUIRE(newArr.Capacity() == arr.Capacity());
		REQUIRE(newArr.Data() != arr.Data());

		for (int i{}; i < nrOfElements; ++i)
		{
			REQUIRE(newArr[i] == arr[i]);
		}
}

	SECTION("Testing move ctor")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		Array<int> newArr{ __MOVE(arr) };

		REQUIRE(arr.Size() == 0);
		REQUIRE(arr.Capacity() == 0);
		REQUIRE(arr.Empty());
		REQUIRE(arr.Data() == nullptr);

		REQUIRE(newArr.Size() == nrOfElements);
		REQUIRE(newArr.Capacity() >= nrOfElements);
		REQUIRE(!newArr.Empty());
		REQUIRE(newArr.Data() != nullptr);
	}

	SECTION("Testing move operator")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		Array<int> newArr = __MOVE(arr);

		REQUIRE(arr.Size() == 0);
		REQUIRE(arr.Capacity() == 0);
		REQUIRE(arr.Empty());
		REQUIRE(arr.Data() == nullptr);

		REQUIRE(newArr.Size() == nrOfElements);
		REQUIRE(newArr.Capacity() >= nrOfElements);
		REQUIRE(!newArr.Empty());
		REQUIRE(newArr.Data() != nullptr);
	}

	SECTION("Comparing if two arrays are equal")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		Array<int> newArr{ arr };
		REQUIRE(arr == newArr);

		newArr.Pop();
		REQUIRE(arr != newArr);

		arr.Pop();
		REQUIRE(arr == newArr);

		arr.Back() = 65;
		REQUIRE(arr != newArr);
	}

	SECTION("Selecting a range of an array")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		Array<int> newArr{ arr.Select([](const int& a)->bool
			{
				return a > 5;
			}) };

		for (size_t i{}; i < newArr.Size(); ++i)
		{
			REQUIRE(newArr[i] > 5);
		}
	}

	SECTION("Create a vector with a start size")
	{
		Array<int> newArr{ 10_size, 15 };

		REQUIRE(newArr.Size() == 10);
		REQUIRE(newArr.Capacity() >= 10);

		for (size_t i{}; i < newArr.Size(); ++i)
		{
			REQUIRE(newArr[i] == 15);
		}
	}

	SECTION("Create a vector with a start capacity")
	{
		Array<int> newArr{ 10_capacity };

		REQUIRE(newArr.Size() == 0);
		REQUIRE(newArr.Capacity() == 10);
		REQUIRE(newArr.Empty());
	}

	SECTION("Using iterators on the array")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		int counter{};
		for (int elem : arr)
		{
			REQUIRE(elem == counter++);
		}

		arr.Clear();

		for (int elem : arr)
		{
			elem;
			REQUIRE(false);
		}
	}

	SECTION("Initialize array using iterators")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		Array<int> newArr{ arr.begin(), arr.end() };

		REQUIRE(newArr.Size() == arr.Size());

		int counter{};
		for (const int elem : newArr)
		{
			REQUIRE(elem == arr[counter++]);
		}
	}

	SECTION("Add a range to an array")
	{
		arr.AddRange({ 0,1,2,3,4,5 });

		REQUIRE(arr.Size() == 6);
		REQUIRE(arr.Capacity() >= 6);
		REQUIRE(arr.At(0) == 0);
		REQUIRE(arr.At(5) == 5);

		Array<int> newArr{};

		newArr.AddRange(arr.begin(), arr.Find(4));

		REQUIRE(newArr.Back() == 3);
		REQUIRE(newArr.Size() == arr.Size() - 2);

		for (size_t i{}; i < newArr.Size(); ++i)
		{
			REQUIRE(newArr[i] == arr[i]);
		}

		newArr.Clear();

		newArr.AddRange(arr.begin(), arr.end());

		REQUIRE(newArr.Back() == 5);
		REQUIRE(newArr.Size() == arr.Size());

		for (size_t i{}; i < newArr.Size(); ++i)
		{
			REQUIRE(newArr[i] == arr[i]);
		}
	}

	SECTION("Find an element in the array")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		Array<int>::It it{ arr.Find(5) };

		REQUIRE(it != arr.end());

		it = arr.Find(-1);

		REQUIRE(it == arr.end());

		it = arr.Find([](const int a)->bool
			{
				return a == 6;
			});

		REQUIRE(it != arr.end());
	}

	SECTION("Finding all elements in the array")
	{
		for (int i{}; i < 5; ++i)
		{
			arr.Add(5);
		}
		for (int i{}; i < 5; ++i)
		{
			arr.Add(i);
		}

		Array<int> newArr{ arr.FindAll(5) };

		REQUIRE(newArr.Size() == 5);

		newArr = arr.FindAll(-1);

		REQUIRE(newArr.Size() == 0);
	}

	SECTION("Erasing elements in the array")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		arr.Erase(3);

		REQUIRE(arr.Size() == nrOfElements - 1);
		REQUIRE(arr.Find(3) == arr.end());

		int counter{};
		for (int i{}; i < arr.Size(); ++i)
		{
			REQUIRE(arr[i] == counter++);

			if (counter == 3)
				++counter;
		}

		arr.Erase(arr.begin());
		REQUIRE(arr.Size() == nrOfElements - 2);
		REQUIRE(arr[0] == 1);
	}

	SECTION("Erasing a range of elements in the array")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		arr.EraseRange(arr.Find(2), arr.Find(9));

		REQUIRE(arr.Size() == 2);
		REQUIRE(arr.Front() == 0);
		REQUIRE(arr.Back() == 1);

		arr.Clear();

		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		arr.EraseRange(3, 15); // should not crash

		REQUIRE(arr.Size() == 3);
	}

	SECTION("Popping off the front of the array")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.Add(i);
		}

		arr.PopFront();
		REQUIRE(arr.Size() == nrOfElements - 1);
		REQUIRE(arr.Front() == 1);

		arr.PopFront();
		REQUIRE(arr.Size() == nrOfElements - 2);
		REQUIRE(arr.Front() == 2);
	}

	SECTION("Adding to the front of the array")
	{
		for (int i{}; i < nrOfElements; ++i)
		{
			arr.AddFront(i);
		}

		arr.AddFront(15);
		REQUIRE(arr.Size() == nrOfElements + 1);
		REQUIRE(arr[0] == 15);

		for (uint64_t i{ 1u }; i < arr.Size(); ++i)
			REQUIRE(arr[i] == nrOfElements - i);

		arr.AddFront(396);
		REQUIRE(arr.Size() == nrOfElements + 2);
		REQUIRE(arr[0] == 396);

		for (uint64_t i{ 2u }; i < arr.Size(); ++i)
			REQUIRE(arr[i] == nrOfElements - i + 1);
	}

	SECTION("Sorting an array using Insertion Sort (when array size < 64)")
	{
		std::initializer_list elems{ 5,0,3,6,7,15,356,-5 };
		std::vector<int> list{ elems };
		std::sort(list.begin(), list.end(), std::less<int>{});

		arr.AddRange(elems);

		REQUIRE(arr.Size() == list.size());

		arr.Sort();

		for (int i{}; i < arr.Size(); ++i)
			REQUIRE(arr[i] == list[i]);
	}


	SECTION("Sorting an array using Insertion Sort with specified predicate (when array size < 64)")
	{
		std::initializer_list elems{ 5,0,3,6,7,15,356,-5 };
		std::vector<int> list{ elems };
		std::sort(list.begin(), list.end(), std::less<int>{});

		arr.AddRange(elems);

		REQUIRE(arr.Size() == list.size());

		arr.Sort(std::less<int>{});

		for (int i{}; i < arr.Size(); ++i)
			REQUIRE(arr[i] == list[i]);
	}

	SECTION("Sorting an array using Merge Sort (when array size > 64)")
	{
		std::vector<int> list{};

		for (int i{ 100 }; i >= 0; --i)
		{
			list.push_back(i);
			arr.Add(i);
		}

		REQUIRE(arr.Size() == list.size());

		std::sort(list.begin(), list.end(), std::less<int>{});
		arr.Sort();
		std::sort(list.begin(), list.end());

		for (int i{}; i < arr.Size(); ++i)
			REQUIRE(arr[i] == list[i]);
	}

	SECTION("Sorting an array using Merge Sort with specified predicate (when array size > 64)")
	{
		std::vector<int> list{};

		for (int i{ 100 }; i >= 0; --i)
		{
			list.push_back(i);
			arr.Add(i);
		}

		REQUIRE(arr.Size() == list.size());

		std::sort(list.begin(), list.end(), std::less<int>{});
		arr.Sort(std::less<int>{});
		std::sort(list.begin(), list.end());

		for (int i{}; i < arr.Size(); ++i)
			REQUIRE(arr[i] == list[i]);
	}

	SECTION("Adding elements to the array using a C-array")
	{
		constexpr int size{ 8 };
		int newArr[size]{ 5,3,4,9,65,-15,-7,6 };

		arr.AddRange(newArr, size);

		for (int i{}; i < size; ++i)
			REQUIRE(arr[i] == newArr[i]);
	}
}

#include <string>
TEST_CASE("Testing Basic Array of characters")
{
	Array<char> arr{};

	SECTION("Adding characters")
	{
		const std::string letters{ "abcdefgh" };

		for (const char c : letters)
			arr.Add(c);

		for (size_t i{}; i < arr.Size(); ++i)
			REQUIRE(arr[i] == letters[i]);
	}

	SECTION("Adding Character to the front")
	{
		const std::string letters{ "abcdefgh" };

		for (const char c : letters)
			arr.AddFront(c);

		size_t counter{};
		for (int i{ static_cast<int>(arr.Size() - 1) }; i >= 0; --i)
			REQUIRE(letters[i] == arr[counter++]);
	}
}
#endif // ARRAY_TESTS

#else
#define STL
//#define CUSTOM
int main(int argc, char* argv[])
{
	using Timepoint = std::chrono::steady_clock::time_point;

	const int amountOfIterations{ 100 };
	const int amountOfPushbacks{ 100'000 };

	std::cout << "Amount of Iterations: " << amountOfIterations << std::endl;
	std::cout << "Amount of push_back: " << amountOfPushbacks << std::endl;

	std::deque<long long> stlTimes{};
	std::deque<long long> customTimes{};

	Timepoint t1{}, t2{};

	for (int i{}; i < amountOfIterations; ++i)
	{
#ifdef STL
		std::vector<int> vector{};

		t1 = std::chrono::steady_clock::now();

		for (int j{}; j < amountOfPushbacks; ++j)
		{
			vector.push_back(j);
		}

		t2 = std::chrono::steady_clock::now();

		stlTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
#endif
#ifdef CUSTOM
		Array<int> container{};

		t1 = std::chrono::steady_clock::now();

		for (int j{}; j < amountOfPushbacks; ++j)
		{
			container.Add(j);
		}

		t2 = std::chrono::steady_clock::now();

		customTimes.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());
#endif
	}

	for (int j{}; j < amountOfIterations / 10; ++j)
	{
#ifdef STL
		stlTimes.pop_back();
		stlTimes.pop_front();
#endif
#ifdef CUSTOM
		customTimes.pop_back();
		customTimes.pop_front();
#endif
	}

#ifdef STL
	std::cout << "STL Time Average (in nanoseconds): " << std::accumulate(stlTimes.cbegin(), stlTimes.cend(), (long long)0) / stlTimes.size() << "\n";
#endif
#ifdef CUSTOM
	std::cout << "Custom Time Average (in nanoseconds): " << std::accumulate(customTimes.cbegin(), customTimes.cend(), (long long)0) / customTimes.size() << "\n";
#endif

	return 0;
}
#endif // UNIT_TESTS