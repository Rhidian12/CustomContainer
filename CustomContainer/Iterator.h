#pragma once

#include "Utils.h"

template<typename T, typename Diff = uint64_t>
class Iterator final
{
	using Pointer = T*;
	using Reference = T&;

public:
	Iterator(Pointer _pPointer)
		: pPointer{ _pPointer }
	{}

#pragma region Access Data
	Reference operator*() const
	{
		return *pPointer;
	}

	Pointer operator->() const
	{
		return pPointer;
	}
#pragma endregion

#pragma region Arithmetic
	Iterator& operator++()
	{
		++pPointer;
		return *this;
	}

	Iterator& operator--()
	{
		--pPointer;
		return *this;
	}

	Iterator& operator+=(const uint64_t i)
	{
		pPointer += i;
		return *this;
	}

	Iterator& operator-=(const uint64_t i)
	{
		pPointer -= i;
		return *this;
	}

	Iterator operator++(int)
	{
		Iterator tmp = *this;
		++(*this);
		return tmp;
	}

	Iterator operator--(int)
	{
		Iterator tmp = *this;
		--(*this);
		return tmp;
	}

	Iterator operator+(const uint64_t i) const
	{
		return Iterator{ pPointer + i };
	}

	Iterator operator+(const Iterator& it) const
	{
		return Iterator{ pPointer + it.pPointer };
	}

	Iterator operator-(const uint64_t i) const
	{
		return Iterator{ pPointer - i };
	}

	Iterator operator-(const Iterator& it) const
	{
		return Iterator{ pPointer - it.pPointer };
	}
#pragma endregion

#pragma region Comparing Iterators
	bool operator==(const Iterator& other) const
	{
		return pPointer == other.pPointer;
	}

	bool operator!=(const Iterator& other) const
	{
		return pPointer != other.pPointer;
	}

	bool operator>(const Iterator& other) const
	{
		return pPointer > other.pPointer;
	}

	bool operator<(const Iterator& other) const
	{
		return pPointer < other.pPointer;
	}

	bool operator>=(const Iterator& other) const
	{
		return pPointer >= other.pPointer;
	}

	bool operator<=(const Iterator& other) const
	{
		return pPointer <= other.pPointer;
	}
#pragma endregion

private:
	Pointer pPointer;
};

template<typename T, typename Diff = uint64_t>
class ConstIterator final
{
	using Pointer = const T*;
	using Reference = const T&;

public:
	ConstIterator(Pointer _pPointer)
		: pPointer{ _pPointer }
	{}

#pragma region Access Data
	Reference operator*() const
	{
		return *pPointer;
	}

	Pointer operator->() const
	{
		return pPointer;
	}
#pragma endregion

#pragma region Arithmetic
	ConstIterator& operator++()
	{
		++pPointer;
		return *this;
	}

	ConstIterator& operator--()
	{
		--pPointer;
		return *this;
	}

	ConstIterator& operator+=(const uint64_t i)
	{
		pPointer += i;
		return *this;
	}

	ConstIterator& operator-=(const uint64_t i)
	{
		pPointer -= i;
		return *this;
	}

	ConstIterator operator++(int)
	{
		ConstIterator tmp = *this;
		++(*this);
		return tmp;
	}

	ConstIterator operator--(int)
	{
		ConstIterator tmp = *this;
		--(*this);
		return tmp;
	}

	ConstIterator operator+(const uint64_t i) const
	{
		return ConstIterator{ pPointer + i };
	}

	ConstIterator operator+(const ConstIterator& it) const
	{
		return ConstIterator{ pPointer + it.pPointer };
	}

	ConstIterator operator-(const uint64_t i) const
	{
		return ConstIterator{ pPointer - i };
	}

	ConstIterator operator-(const ConstIterator& it) const
	{
		return ConstIterator{ pPointer - it.pPointer };
	}
#pragma endregion

#pragma region Comparing Iterators
	bool operator==(const ConstIterator& other) const
	{
		return pPointer == other.pPointer;
	}

	bool operator!=(const ConstIterator& other) const
	{
		return pPointer != other.pPointer;
	}

	bool operator>(const ConstIterator& other) const
	{
		return pPointer > other.pPointer;
	}

	bool operator<(const ConstIterator& other) const
	{
		return pPointer < other.pPointer;
	}

	bool operator>=(const ConstIterator& other) const
	{
		return pPointer >= other.pPointer;
	}

	bool operator<=(const ConstIterator& other) const
	{
		return pPointer <= other.pPointer;
	}
#pragma endregion

private:
	Pointer pPointer;
};