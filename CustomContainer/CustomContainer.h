#pragma once

#include "Utils.h"
#include "Types.h"
#include "Iterator.h"

#include <functional> /* std::function */

template<typename T>
class Array
{
	using UnaryPred = std::function<bool(const T&)>;
	using BinaryPred = std::function<bool(const T&, const T&)>;

public:
	using It = Iterator<T>;
	using CIt = ConstIterator<T>;

#pragma region Ctors and Dtor
	constexpr Array()
		: m_pHead{}
		, m_pTail{}
		, m_pCurrentEnd{}
	{}
	constexpr Array(const Size_P size)
		: m_pHead{}
		, m_pTail{}
		, m_pCurrentEnd{}
	{
		for (uint64_t i{}; i < size._Size; ++i)
			EmplaceBack(T{});
	}
	constexpr Array(const Size_P size, const T& val)
		: m_pHead{}
		, m_pTail{}
		, m_pCurrentEnd{}
	{
		for (uint64_t i{}; i < size._Size; ++i)
			EmplaceBack(val);
	}
	constexpr Array(const Capacity_P cap)
		: m_pHead{}
		, m_pTail{}
		, m_pCurrentEnd{}
	{
		Reserve(cap._Capacity);
	}
	constexpr Array(std::initializer_list<T> init)
		: m_pHead{}
		, m_pTail{}
		, m_pCurrentEnd{}
	{
		for (const T& elem : init)
			EmplaceBack(elem);
	}
	constexpr Array(It beg, It end)
		: m_pHead{}
		, m_pTail{}
		, m_pCurrentEnd{}
	{
		for (; beg != end; ++beg)
			EmplaceBack(*beg);
	}

	constexpr ~Array()
	{
		DeleteData(m_pHead, m_pCurrentEnd);
		Release(m_pHead);
	}
#pragma endregion

#pragma region Rule of 5
	constexpr Array(const Array& other) noexcept
		: m_pHead{}
		, m_pTail{}
		, m_pCurrentEnd{}
	{
		const uint64_t cap{ other.Capacity() };
		if (cap > 0u)
		{
			m_pHead = new T[cap]{};
			m_pTail = m_pHead + cap;

			const uint64_t size{ other.Size() };
			for (uint64_t i{}; i < size; ++i)
				new (m_pHead + i) T{ *(other.m_pHead + i) }; // dont allow moving 

			m_pCurrentEnd = m_pHead + size;
		}
	}
	constexpr Array(Array&& other) noexcept
		: m_pHead{ __MOVE(other.m_pHead) }
		, m_pTail{ __MOVE(other.m_pTail) }
		, m_pCurrentEnd{ __MOVE(other.m_pCurrentEnd) }
	{
		other.m_pHead = nullptr;
		other.m_pTail = nullptr;
		other.m_pCurrentEnd = nullptr;
	}

	constexpr Array& operator=(const Array& other) noexcept
	{
		if (m_pHead)
		{
			DeleteData(m_pHead, m_pCurrentEnd);
			Release(m_pHead);
		}

		m_pHead = nullptr;
		m_pTail = nullptr;
		m_pCurrentEnd = nullptr;

		const uint64_t cap{ other.Capacity() };
		if (cap > 0u)
		{
			m_pHead = new T[cap]{};
			m_pTail = m_pHead + cap;

			const uint64_t size{ other.Size() };
			for (uint64_t i{}; i < size; ++i)
				new (m_pHead + i) T{ *(other.m_pHead + i) }; // dont allow moving 

			m_pCurrentEnd = m_pHead + size;
		}

		return *this;
	}
	constexpr Array& operator=(Array&& other) noexcept
	{
		if (m_pHead)
		{
			DeleteData(m_pHead, m_pCurrentEnd);
			Release(m_pHead);
		}

		m_pHead = __MOVE(other.m_pHead);
		m_pTail = __MOVE(other.m_pTail);
		m_pCurrentEnd = __MOVE(other.m_pCurrentEnd);

		other.m_pHead = nullptr;
		other.m_pTail = nullptr;
		other.m_pCurrentEnd = nullptr;

		return *this;
	}
#pragma endregion

#pragma region Adding and Removing Elements
	constexpr void Add(const T& val)
	{
		EmplaceBack(val);
	}
	constexpr void Add(T&& val)
	{
		EmplaceBack(__MOVE(val));
	}

	constexpr void AddFront(const T& val)
	{
		EmplaceFront(val);
	}
	constexpr void AddFront(T&& val)
	{
		EmplaceFront(__MOVE(val));
	}

	constexpr void AddRange(std::initializer_list<T> elems)
	{
		for (const T& elem : elems)
			EmplaceBack(elem);
	}
	constexpr void AddRange(It beg, It end)
	{
		for (; beg != end; ++beg)
			EmplaceBack(*beg);
	}
	constexpr void AddRange(T* pArr, const uint64_t n)
	{
		__ASSERT(pArr != nullptr);

		for (uint64_t i{}; i < n; ++i)
			EmplaceBack(pArr[i]);
	}

	constexpr It EraseByIndex(const uint64_t index)
	{
		__ASSERT(index < Size() && "Array::Erase() > index is out of range");

		const uint64_t oldSize{ Size() };

		if (index == oldSize - 1)
		{
			Pop();

			return end();
		}
		else if (index == 0)
		{
			PopFront();

			return begin();
		}
		else
		{
			(m_pHead + index)->~T();
			MoveRangeBackward(m_pHead + index + 1, m_pCurrentEnd--, m_pHead + index);

			return It{ m_pHead + index };
		}
	}

	constexpr It Erase(It pos)
	{
		__ASSERT(pos != end() && "Array::Erase() > invalid iterator was passed as a parameter");

		return Erase(*pos);
	}
	constexpr It Erase(const T& val)
	{
		const uint64_t size{ Size() };
		for (uint64_t i{}; i < size; ++i)
			if (*(m_pHead + i) == val)
				return EraseByIndex(i);

		return end();
	}
	constexpr It Erase(const UnaryPred& pred)
	{
		It it{ Find(pred) };

		if (it != end())
			return Erase(*it);

		return end();
	}
	constexpr It Erase(UnaryPred&& pred)
	{
		It it{ Find(__MOVE(pred)) };

		if (it != end())
			return Erase(*it);

		return end();
	}

	constexpr void EraseRange(const uint64_t start, const uint64_t count)
	{
		__ASSERT(start < Size() && "Array::EraseRange() > Start is out of range");

		EraseRange(It{ m_pHead + start }, It{ m_pHead + start + count });
	}
	constexpr void EraseRange(It beg, It endIt)
	{
		__ASSERT(beg != end() && "Array::EraseRange() > Cannot iterator past the end");

		if (endIt >= end())
			endIt = It{ m_pCurrentEnd - 1 };

		for (; beg <= endIt; --endIt)
			beg = Erase(beg);
	}

	constexpr void Insert(const uint64_t index, const T& val)
	{
		Emplace(index, val);
	}
	constexpr void Insert(const uint64_t index, T&& val)
	{
		Emplace(index, __MOVE(val));
	}

	constexpr void Pop()
	{
		if (Size() == 0)
			return;

		(--m_pCurrentEnd)->~T();
	}

	constexpr void PopFront()
	{
		if (Size() == 0)
			return;

		m_pHead->~T();

		MoveRangeBackward(m_pHead + 1, m_pCurrentEnd--, m_pHead);
	}

	constexpr void Clear()
	{
		DeleteData(m_pHead, m_pCurrentEnd);

		m_pCurrentEnd = m_pHead;
	}

	template<typename ... Ts>
	constexpr T& EmplaceBack(Ts&&... args)
	{
		/* if we point past our allocated memory we have an issue */
		if (!m_pCurrentEnd || m_pCurrentEnd >= m_pTail)
			Reallocate();

		return *(new (m_pCurrentEnd++) T{ __FORWARD(args)... });
	}

	template<typename ... Ts>
	constexpr T& Emplace(const uint64_t index, Ts&&... args)
	{
		__ASSERT(index <= Size() && "Array::Emplace() > index is out of range");

		const uint64_t oldSize{ Size() };

		if (oldSize + 1 > Capacity())
			Reallocate();

		if (index == 0)
			return EmplaceFront(__FORWARD(args)...);
		else if (oldSize == 0 || index == oldSize || index == oldSize - 1)
			return EmplaceBack(__FORWARD(args)...);
		else
		{
			MoveRangeForward(m_pHead + index, m_pCurrentEnd++ - 1, m_pHead + index + 1);
			return *(new (m_pHead + index) T{ __FORWARD(args)... });
		}
	}

	template<typename ... Ts>
	constexpr T& EmplaceFront(Ts&&... args)
	{
		/* if we point past our allocated memory we have an issue */
		if (!m_pCurrentEnd || m_pCurrentEnd >= m_pTail)
			Reallocate();

		MoveRangeForward(m_pHead, m_pCurrentEnd++, m_pHead + 1);

		return *(new (m_pHead) T{ __FORWARD(args)... });
	}
#pragma endregion

#pragma region Array Information
	__NODISCARD constexpr bool Empty() const
	{
		return Size() == 0;
	}

	__NODISCARD constexpr uint64_t Size() const
	{
		return m_pCurrentEnd - m_pHead;
	}

	__NODISCARD constexpr uint64_t Capacity() const
	{
		return m_pTail - m_pHead;
	}

	__NODISCARD constexpr uint64_t MaxSize() const
	{
		return std::numeric_limits<uint64_t>::max();
	}

	__NODISCARD constexpr bool operator==(const Array& other) const
	{
		const uint64_t size{ Size() };

		if (size != other.Size())
			return false;

		for (uint64_t i{}; i < size; ++i)
			if (*(m_pHead + i) != *(other.m_pHead + i))
				return false;

		return true;
	}

	__NODISCARD constexpr bool operator!=(const Array& other) const
	{
		return !(*this == other);
	}
#pragma endregion

#pragma region Manipulating Array
	constexpr void Reserve(const uint64_t newCap)
	{
		if (newCap > Capacity())
			if (newCap < MaxSize())
				ReallocateExactly(newCap);
	}

	constexpr void Resize(const uint64_t newSize)
	{
		static_assert(std::is_default_constructible_v<T>, "Array::Resize() > T is not default constructable!");

		Resize(newSize, T{});
	}
	template<typename U>
	constexpr void Resize(const uint64_t newSize, U&& val)
	{
		static_assert(std::is_default_constructible_v<U>, "Array::Resize() > T is not default constructable!");
		static_assert(std::is_same_v<T, U>, "Array::Resize() > U and T must be the same!");

		const uint64_t oldSize{ Size() };

		if (newSize > oldSize)
		{
			const uint64_t diff{ newSize - oldSize };

			for (uint64_t i{}; i < diff; ++i)
				EmplaceBack(__FORWARD(val));

			return;
		}

		if (newSize < oldSize)
		{
			const uint64_t diff{ oldSize - newSize };

			for (uint64_t i{}; i < diff; ++i)
				Pop();

			return;
		}
	}

	constexpr void ShrinkToFit()
	{
		if (Size() == Capacity())
			return;

		ReallocateExactly(Size());
	}

	constexpr Array Select(const UnaryPred& pred) const
	{
		const uint64_t size{ Size() };

		Array arr{ Capacity_P{ size } };

		for (uint64_t i{}; i < size; ++i)
		{
			const T* const elem{ m_pHead + i };

			if (pred(*elem))
				arr.Add(*elem);
		}

		return arr;
	}
	constexpr Array Select(UnaryPred&& pred) const
	{
		const uint64_t size{ Size() };

		Array arr{ Capacity_P{ size } };

		for (uint64_t i{}; i < size; ++i)
		{
			const T* const elem{ m_pHead + i };

			if (pred(*elem))
				arr.Add(*elem);
		}

		return arr;
	}

	constexpr void Sort() const
	{
		Sort([](const T& a, const T& b)->bool
			{
				return a < b;
			});
	}
	constexpr void Sort(const BinaryPred& pred) const
	{
		if (!m_pHead)
			return;

		const uint64_t size{ Size() };

		if (size < 64u)
			InsertionSort(pred);
		else
			MergeSort(0u, size - 1u, pred);
	}
#pragma endregion

#pragma region Accessing Elements
	constexpr T& Front()
	{
		__ASSERT(Size() > 0 && "Array::Front() > Array is empty");

		return *m_pHead;
	}
	constexpr const T& Front() const
	{
		__ASSERT(Size() > 0 && "Array::Front() > Array is empty");

		return *m_pHead;
	}

	constexpr T& Back()
	{
		__ASSERT(Size() > 0 && "Array::Back() > Array is empty");

		return *(m_pCurrentEnd - 1);
	}
	constexpr const T& Back() const
	{
		__ASSERT(Size() > 0 && "Array::Back() > Array is empty");

		return *(m_pCurrentEnd - 1);
	}

	constexpr T& At(const uint64_t index)
	{
		__ASSERT((index < Size()) && "Array::At() > Index is out of range");

		return *(m_pHead + index);
	}
	constexpr const T& At(const uint64_t index) const
	{
		__ASSERT((index < Size()) && "Array::At() > Index is out of range");

		return *(m_pHead + index);
	}

	constexpr T& operator[](const uint64_t index)
	{
		return *(m_pHead + index);
	}
	constexpr const T& operator[](const uint64_t index) const
	{
		return *(m_pHead + index);
	}

	constexpr T* const Data()
	{
		return m_pHead;
	}
	constexpr const T* const Data() const
	{
		return m_pHead;
	}

	constexpr It Find(const T& val) const
	{
		const uint64_t size{ Size() };
		for (uint64_t i{}; i < size; ++i)
			if (*(m_pHead + i) == val)
				return It{ m_pHead + i };

		return It{ m_pCurrentEnd };
	}
	constexpr It Find(const UnaryPred& pred) const
	{
		const uint64_t size{ Size() };
		for (uint64_t i{}; i < size; ++i)
			if (pred(*(m_pHead + i)))
				return It{ m_pHead + i };

		return It{ m_pCurrentEnd };
	}
	constexpr It Find(UnaryPred&& pred) const
	{
		const uint64_t size{ Size() };
		for (uint64_t i{}; i < size; ++i)
			if (pred(*(m_pHead + i)))
				return It{ m_pHead + i };

		return It{ m_pCurrentEnd };
	}

	constexpr Array FindAll(const T& val) const
	{
		Array arr{};

		const uint64_t size{ Size() };
		for (uint64_t i{}; i < size; ++i)
			if (*(m_pHead + i) == val)
				arr.EmplaceBack(*(m_pHead + i));

		return arr;
	}
	constexpr Array FindAll(const UnaryPred& pred) const
	{
		Array arr{};

		const uint64_t size{ Size() };
		for (uint64_t i{}; i < size; ++i)
			if (pred(*(m_pHead + i)))
				arr.EmplaceBack(*(m_pHead + i));

		return arr;
	}
	constexpr Array FindAll(UnaryPred&& pred) const
	{
		Array arr{};

		const uint64_t size{ Size() };
		for (uint64_t i{}; i < size; ++i)
			if (pred(*(m_pHead + i)))
				arr.EmplaceBack(*(m_pHead + i));

		return arr;
	}
#pragma endregion

#pragma region Iterators
	constexpr It begin() { return m_pHead; }
	constexpr CIt begin() const { return m_pHead; }

	constexpr It end() { return m_pCurrentEnd; }
	constexpr CIt end() const { return m_pCurrentEnd; }

	constexpr CIt cbegin() const { return m_pHead; }
	constexpr CIt cend() const { return m_pCurrentEnd; }
#pragma endregion

private:
#pragma region Internal Helpers
	constexpr void Reallocate()
	{
		const uint64_t oldSize{ Size() };
		const uint64_t newCap{ CalculateNewCapacity(oldSize + 1) };

		T* pOldHead{ m_pHead };
		T* pOldTail{ m_pTail };

		m_pHead = new T[newCap]{};
		m_pTail = m_pHead + newCap;

		for (uint64_t i{}; i < oldSize; ++i)
		{
			if constexpr (std::is_move_assignable_v<T>)
				new (m_pHead + i) T{ __MOVE(*(pOldHead + i)) };
			else
				new (m_pHead + i) T{ *(pOldHead + i) };
		}

		m_pCurrentEnd = m_pHead + oldSize;

		DeleteData(pOldHead, pOldTail);
		Release(pOldHead);
	}
	constexpr void ReallocateExactly(const uint64_t newCap)
	{
		const uint64_t oldSize{ Size() };

		T* pOldHead{ m_pHead };
		T* pOldTail{ m_pTail };

		m_pHead = new T[newCap]{};
		m_pTail = m_pHead + newCap;

		for (uint64_t i{}; i < oldSize; ++i)
		{
			if constexpr (std::is_move_assignable_v<T>)
				new (m_pHead + i) T{ __MOVE(*(pOldHead + i)) };
			else
				new (m_pHead + i) T{ *(pOldHead + i) };
		}

		m_pCurrentEnd = m_pHead + oldSize;

		DeleteData(pOldHead, pOldTail);
		Release(pOldHead);
	}

	constexpr void Release(T*& pData)
	{
		if (pData)
		{
			delete[] pData;
			pData = nullptr;
		}
	}

	constexpr void DeleteData(T* head, T* const tail) const
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			while (head < tail)
			{
				head->~T();
				++head;
			}
		}
	}

	__NODISCARD constexpr uint64_t CalculateNewCapacity(const uint64_t min) const
	{
		const uint64_t oldCap{ Capacity() };
		const uint64_t maxCap{ MaxSize() };

		if (oldCap > maxCap - oldCap / 2u)
			return maxCap;

		const uint64_t newCap{ oldCap + oldCap / 2u };

		// If our growth is insufficient, return just the bare minimum
		if (newCap < min)
			return min;

		return newCap;
	}

	constexpr void MoveRangeBackward(T* head, T* end, T* newHead) const
	{
		const uint64_t size{ static_cast<uint64_t>(end - head) };
		for (uint64_t i{}; i < size; ++i)
		{
			if constexpr (std::is_move_assignable_v<T>)
				new (newHead + i) T{ __MOVE(*(head + i)) };
			else
				new (newHead + i) T{ *(head + i) };
		}
	}

	constexpr void MoveRangeForward(T* head, T* end, T* newHead) const
	{
		const int64_t size{ static_cast<int64_t>(end - head) };
		for (int64_t i{ size - 1 }; i >= 0; --i)
		{
			if constexpr (std::is_move_assignable_v<T>)
				new (newHead + i) T{ __MOVE(*(head + i)) };
			else
				new (newHead + i) T{ *(head + i) };
		}
	}
#pragma endregion

#pragma region Sorters
	constexpr void InsertionSort(const BinaryPred& pred) const
	{
		/* Don't use At() to make sure elements get copied instead of referenced around! */

		const uint64_t size{ Size() };
		for (int i{ 1 }; i < size; ++i)
		{
			T key = *(m_pHead + i);
			int j{ i - 1 };

			while (j >= 0 && pred(key, *(m_pHead + j)))
			{
				*(m_pHead + j + 1) = *(m_pHead + j);
				--j;
			}

			*(m_pHead + j + 1) = key;
		}
	}
	constexpr void MergeSort(const uint64_t begin, const uint64_t end, const BinaryPred& pred) const
	{
		/* Check recursion stop */
		if (begin >= end)
			return;

		const uint64_t mid{ begin + (end - begin) / 2u };

		MergeSort(begin, mid, pred);
		MergeSort(mid + 1u, end, pred);
		Merge(begin, mid, end, pred);
	}
	constexpr void Merge(uint64_t left, uint64_t mid, const uint64_t right, const BinaryPred& pred) const
	{
		uint64_t halfStart{ mid + 1u };

		if (pred(*(m_pHead + mid), *(m_pHead + halfStart)))
			return;

		while (left <= mid && halfStart <= right)
		{
			if (pred(*(m_pHead + left), *(m_pHead + halfStart)))
				++left;
			else
			{
				T val{ *(m_pHead + halfStart) };
				uint64_t i{ halfStart };

				while (i != left)
				{
					*(m_pHead + i) = *(m_pHead + i - 1);
					--i;
				}

				*(m_pHead + left) = val;

				++left;
				++mid;
				++halfStart;
			}
		}
	}
#pragma endregion

	T* m_pHead;
	T* m_pTail;
	T* m_pCurrentEnd /* points PAST the last element */;
};
