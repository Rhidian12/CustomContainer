#pragma once
#include "Utils.h"

/* [TODO]: Make Allocator */
/* [TODO]: Make Iterators */
template<typename Type>
class CustomContainer final
{
	inline static constexpr size_t SizeOfType = sizeof(Type);

public:
	CustomContainer() = default;
	~CustomContainer()
	{
		DeleteData(Head, Tail);
		ReleaseMemory(Head);
	}

	/* If these aren't marked as noexcept, VS keeps complaining */
	CustomContainer(const CustomContainer& other) noexcept
	{
		const size_t capacity{ other.Capacity() };

		Head = static_cast<Type*>(malloc(SizeOfType * capacity));
		Tail = Head + capacity;

		for (size_t index{}; index < other.Size(); ++index)
		{
			LastElement = Head + index;
			new (LastElement) Type(*(other.Head + index));
		}
	}
	CustomContainer(CustomContainer&& other) noexcept
		: Head{ std::move(other.Head) }
		, Tail{ std::move(other.Tail) }
		, LastElement{ std::move(other.LastElement) }
	{
		/* Don't release memory you're not reallocating fucking dumbass */
		other.Head = nullptr;
		other.Tail = nullptr;
		other.LastElement = nullptr;
	}

	CustomContainer<Type>& operator=(const CustomContainer& other) noexcept
	{
		const size_t capacity{ other.Capacity() };

		Head = static_cast<Type*>(malloc(SizeOfType * capacity));
		Tail = Head + capacity;

		for (size_t index{}; index < other.Size(); ++index)
		{
			LastElement = Head + index;
			new (LastElement) Type(*(other.Head + index));
		}

		return *this;
	}
	CustomContainer<Type>& operator=(CustomContainer&& other) noexcept
	{
		Head = std::move(other.Head);
		Tail = std::move(other.Tail);
		LastElement = std::move(other.LastElement);

		/* Don't release memory you're not reallocating fucking dumbass */

		other.Head = nullptr;
		other.Tail = nullptr;
		other.LastElement = nullptr;

		return *this;
	}

	void Add(const Type& val)
	{
		Emplace(val);
	}
	void Add(Type&& val)
	{
		Emplace(std::move(val));
	}

	template<typename ... Values>
	void Emplace(Values&&... val)
	{
		Type* const pNextBlock{ LastElement != nullptr ? LastElement + 1 : Head };

		if (!pNextBlock || pNextBlock >= Tail)
		{
			ReallocateAndEmplace(std::forward<Values>(val)...);
		}
		else
		{
			/* [TODO]: AN ALLOCATOR SHOULD DO THIS */
			// CurrentElement = new Type(std::forward<Values>(val)...);
			*pNextBlock = Type(std::forward<Values>(val)...);

			LastElement = pNextBlock;
		}
	}

	void Pop()
	{
		if (LastElement)
		{
			if constexpr (!std::is_trivially_destructible_v<Type>)
			{
				LastElement->~Type();
			}

			Type* pPreviousBlock{ LastElement - 1 > Head ? LastElement - 1 : nullptr };

			LastElement = nullptr;
			LastElement = pPreviousBlock;
		}
	}

	void Clear()
	{
		DeleteData(Head, Tail);

		LastElement = nullptr;
	}

	size_t Size() const
	{
		return LastElement != nullptr ? LastElement - Head + 1 : 0;
	}

	size_t Capacity() const
	{
		return Tail - Head;
	}

	void Reserve(size_t newCapacity)
	{
		if (newCapacity <= Capacity())
			return;

		Reallocate(newCapacity);
	}

	void Resize(size_t newSize)
	{
		if (newSize > Size())
		{
			ResizeToBigger(newSize);
		}
		else if (newSize < Size())
		{
			ResizeToSmaller(newSize);
		}
	}

	void ShrinkToFit()
	{
		if (Size() == Capacity())
			return;

		Reallocate(Size());
	}

	Type& Front()
	{
		ASSERT((Head != nullptr), "Container::Front() > Out of range!");

		return *(Head);
	}
	const Type& Front() const
	{
		ASSERT((Head != nullptr), "Container::Front() > Out of range!");

		return *(Head);
	}

	Type& Back()
	{
		ASSERT((LastElement != nullptr), "Container::Back() > Out of range!");

		return *LastElement;
	}
	const Type& Back() const
	{
		ASSERT((LastElement != nullptr), "Container::Back() > Out of range!");

		return *LastElement;
	}

	bool IsEmpty() const
	{
		return LastElement == nullptr;
	}

	Type& At(size_t index)
	{
		ASSERT(((Head + index) < Tail), "Container::At() > Index was out of range!");

		return *(Head + index);
	}
	const Type& At(size_t index) const
	{
		ASSERT(((Head + index) < Tail), "Container::At() > Index was out of range!");

		return *(Head + index);
	}

	Type& operator[](size_t index)
	{
		return *(Head + index);
	}
	const Type& operator[](size_t index) const
	{
		return *(Head + index);
	}

	Type* const Data()
	{
		return Head;
	}
	const Type* const Data() const
	{
		return Head;
	}

private:
	void ReleaseMemory(Type*& pOldHead)
	{
		if (pOldHead)
		{
			free(pOldHead);
			pOldHead = nullptr;
		}
	}

	void DeleteData(Type* pHead, Type* const pTail)
	{
		if constexpr (!std::is_trivially_destructible_v<Type>) // if this is a struct / class with a custom destructor, call it
		{
			while (pHead <= pTail)
			{
				pHead->~Type();
				++pHead;
			}
		}
	}

	void Reallocate(size_t newCapacity)
	{
		const size_t size{ Size() };

		Type* pOldHead{ Head };
		Type* const pOldTail{ Tail };

		Head = static_cast<Type*>(malloc(SizeOfType * newCapacity));
		Tail = Head + newCapacity;

		for (size_t index{}; index < size; ++index)
		{
			LastElement = Head + index; // adjust pointer
			*LastElement = std::move(*(pOldHead + index)); // move element from old memory over
		}

		DeleteData(pOldHead, pOldTail);
		ReleaseMemory(pOldHead);
	}

	template<typename ... Values>
	void ReallocateAndEmplace(Values && ...values)
	{
		Reallocate(Size() + Size() / 2 + 1);

		if (!LastElement)
		{
			LastElement = Head;
		}
		else
		{
			++LastElement;
		}

		/* [TODO]: AN ALLOCATOR SHOULD DO THIS */
		// CurrentElement = new Type(std::forward<Values>(val)...);
		*LastElement = Type(std::forward<Values>(values)...);
	}

	void ResizeToBigger(size_t newSize)
	{
		static_assert(std::is_default_constructible_v<Type>, "Container::Resize() > Type is not default constructable!");

		const size_t sizeDifference{ newSize - Size() };

		for (size_t i{}; i < sizeDifference; ++i)
		{
			Emplace(Type{});
		}
	}
	void ResizeToSmaller(size_t newSize)
	{
		const size_t sizeDifference{ Size() - newSize };

		for (size_t i{}; i < sizeDifference; ++i)
		{
			Pop();
		}
	}

	Type* Head{ nullptr };
	Type* Tail{ nullptr };
	Type* LastElement{ nullptr };
};

template<typename Type>
class Iterator final
{
public:
	using difference_type = std::ptrdiff_t;

	Iterator(Type* const pPointer)
		: Pointer{ pPointer }
	{}

	/* Default rule of 5 is good enough */
	Iterator(const Iterator&) noexcept = default;
	Iterator(Iterator&&) noexcept = default;
	Iterator& operator=(const Iterator&) noexcept = default;
	Iterator& operator=(Iterator&&) noexcept = default;

	Iterator& operator+=(difference_type diff) { Pointer += diff; return *this; }
	Iterator& operator-=(difference_type diff) { Pointer -= diff; return *this; }

	Type& operator*() const { return *Pointer; }
	Type* operator->() const { return Pointer; }
	Type& operator[](difference_type diff) const { return Pointer[diff]; }

	Iterator& operator++() { ++Pointer; return *this; }
	Iterator& operator--() { --Pointer; return *this; }
	Iterator& operator++(int) { Iterator temp(*this); ++Pointer; return temp; }
	Iterator& operator--(int) { Iterator temp(*this); --Pointer; return temp; }

	difference_type operator-(const Iterator& other) const { return Pointer - other.Pointer; }

	Iterator operator+(difference_type diff) const { return Iterator(Pointer + diff); }
	Iterator operator-(difference_type diff) const { return Iterator(Pointer - diff); }

	friend Iterator operator+(difference_type diff, const Iterator& it) { return Iterator(diff + it.Pointer); }
	friend Iterator operator-(difference_type diff, const Iterator& it) { return Iterator(diff - it.Pointer); }

	bool operator==(const Iterator& it) const { return Pointer == it.Pointer; };
	bool operator!=(const Iterator& it) const { return Pointer != it.Pointer; };
	
	bool operator>(const Iterator& it) const { return Pointer > it.Pointer; };
	bool operator<(const Iterator& it) const { return Pointer < it.Pointer; };
	
	bool operator>=(const Iterator& it) const { return Pointer >= it.Pointer; };
	bool operator<=(const Iterator& it) const { return Pointer <= it.Pointer; };

private:
	Type* Pointer{ nullptr };
};