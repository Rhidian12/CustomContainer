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

	~CustomContainer();

	/* If these aren't marked as noexcept, VS keeps complaining */
	CustomContainer<Type>(const CustomContainer<Type>& other) noexcept;
	CustomContainer<Type>(CustomContainer<Type>&& other) noexcept;
	CustomContainer<Type>& operator=(const CustomContainer<Type>& other) noexcept;
	CustomContainer<Type>& operator=(CustomContainer<Type>&& other) noexcept;

	void Add(const Type& val);
	void Add(Type&& val);

	template<typename ... Values>
	void Emplace(Values&&... val);

	void Clear();

	size_t Size() const;

	size_t Capacity() const;

	void Reserve(size_t newCapacity);

	Type& Front();
	const Type& Front() const;

	Type& Back();
	const Type& Back() const;

	bool IsEmpty() const;

	Type& At(size_t index);
	const Type& At(size_t index) const;

	Type& operator[](size_t index);
	const Type& operator[](size_t index) const;

	Type* const Data() const;

private:
	void ReleaseMemory(Type*& pOldHead);

	void DeleteData(Type* pHead, Type* const pTail);

	void Reallocate();
	void Reallocate(size_t newCapacity);

	template<typename ... Values>
	void ReallocateAndEmplace(Values&&... values);

	Type* Head{ nullptr };
	Type* Tail{ nullptr };
	Type* CurrentElement{ nullptr };
};

template<typename Type>
CustomContainer<Type>::~CustomContainer()
{
	DeleteData(Head, Tail);
	ReleaseMemory(Head);
}

template<typename Type>
CustomContainer<Type>::CustomContainer(const CustomContainer<Type>& other) noexcept
{
	const size_t capacity{ other.Capacity() };

	Head = static_cast<Type*>(malloc(SizeOfType * capacity));
	Tail = Head + capacity;

	for (size_t index{}; index < other.Size(); ++index)
	{
		CurrentElement = Head + index;
		*CurrentElement = *(other.Head + index);
	}
}

template<typename Type>
CustomContainer<Type>::CustomContainer(CustomContainer<Type>&& other) noexcept
	: Head{ std::move(other.Head) }
	, Tail{ std::move(other.Tail) }
	, CurrentElement{ std::move(other.CurrentElement) }
{
	/* Don't release memory you're not reallocating fucking dumbass */
	other.Head = nullptr;
	other.Tail = nullptr;
	other.CurrentElement = nullptr;
}

template<typename Type>
CustomContainer<Type>& CustomContainer<Type>::operator=(const CustomContainer<Type>& other) noexcept
{
	const size_t capacity{ other.Capacity() };

	Head = static_cast<Type*>(malloc(SizeOfType * capacity));
	Tail = Head + capacity;

	for (size_t index{}; index < other.Size(); ++index)
	{
		CurrentElement = Head + index;
		*CurrentElement = *(other.Head + index);
	}

	return *this;
}

template<typename Type>
CustomContainer<Type>& CustomContainer<Type>::operator=(CustomContainer<Type>&& other) noexcept
{
	Head = std::move(other.Head);
	Tail = std::move(other.Tail);
	CurrentElement = std::move(other.CurrentElement);

	/* Don't release memory you're not reallocating fucking dumbass */

	other.Head = nullptr;
	other.Tail = nullptr;
	other.CurrentElement = nullptr;

	return *this;
}

template<typename Type>
void CustomContainer<Type>::Add(const Type& val)
{
	Emplace(val);
}

template<typename Type>
void CustomContainer<Type>::Add(Type&& val)
{
	Emplace(std::move(val));
}

template<typename Type>
template<typename ...Values>
void CustomContainer<Type>::Emplace(Values&&... val)
{
	Type* const pNextBlock{ CurrentElement != nullptr ? CurrentElement + 1 : Head };

	if (!pNextBlock || pNextBlock >= Tail)
	{
		ReallocateAndEmplace(std::forward<Values>(val)...);

		// CurrentElement = Head;
	}
	else
	{
		/* [TODO]: AN ALLOCATOR SHOULD DO THIS */
		// CurrentElement = new Type(std::forward<Values>(val)...);
		*pNextBlock = Type(std::forward<Values>(val)...);

		CurrentElement = pNextBlock;
	}
}

template<typename Type>
void CustomContainer<Type>::Clear()
{
	DeleteData(Head, Tail);

	CurrentElement = nullptr;
}

template<typename Type>
size_t CustomContainer<Type>::Size() const
{
	return CurrentElement != nullptr ? CurrentElement - Head + 1 : 0;
}

template<typename Type>
size_t CustomContainer<Type>::Capacity() const
{
	return Tail - Head;
}

template<typename Type>
void CustomContainer<Type>::Reserve(size_t newCapacity)
{
	if (newCapacity <= Capacity())
		return;

	Reallocate(newCapacity);
}

template<typename Type>
Type& CustomContainer<Type>::Front()
{
	ASSERT((Head != nullptr), "Container::Front() > Out of range!");

	return *(Head);
}

template<typename Type>
const Type& CustomContainer<Type>::Front() const
{
	ASSERT((Head != nullptr), "Container::Front() > Out of range!");

	return *(Head);
}

template<typename Type>
Type& CustomContainer<Type>::Back()
{
	ASSERT((CurrentElement != nullptr), "Container::Back() > Out of range!");

	return *CurrentElement;
}

template<typename Type>
const Type& CustomContainer<Type>::Back() const
{
	ASSERT((CurrentElement != nullptr), "Container::Back() > Out of range!");

	return *CurrentElement;
}

template<typename Type>
bool CustomContainer<Type>::IsEmpty() const
{
	return CurrentElement == nullptr;
}

template<typename Type>
Type& CustomContainer<Type>::At(size_t index)
{
	ASSERT(((Head + index) < Tail), "Container::At() > Index was out of range!");

	return *(Head + index);
}

template<typename Type>
const Type& CustomContainer<Type>::At(size_t index) const
{
	ASSERT(((Head + index) < Tail), "Container::At() > Index was out of range!");

	return *(Head + index);
}

template<typename Type>
Type& CustomContainer<Type>::operator[](size_t index)
{
	return *(Head + index);
}

template<typename Type>
const Type& CustomContainer<Type>::operator[](size_t index) const
{
	return *(Head + index);
}

template<typename Type>
Type* const CustomContainer<Type>::Data() const
{
	return Head;
}

template<typename Type>
void CustomContainer<Type>::ReleaseMemory(Type*& pOldHead)
{
	if (pOldHead)
	{
		free(pOldHead);
		pOldHead = nullptr;
	}
}

template<typename Type>
void CustomContainer<Type>::DeleteData(Type* pHead, Type* const pTail)
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

template<typename Type>
void CustomContainer<Type>::Reallocate()
{
	if (CurrentElement)
		ASSERT(((CurrentElement + 1) >= Tail), "Container::Reallocate() > Reallocating while there is still memory left!");

	const size_t oldCapacity{ size_t(Tail - Head) }; // also oldSize
	const size_t newCapacity{ oldCapacity != 0 ? oldCapacity * 2 : 1 };

	Type* pOldHead{ Head };
	Type* const pOldTail{ Tail };

	Head = static_cast<Type*>(malloc(SizeOfType * newCapacity));
	Tail = Head + newCapacity;

	for (size_t index{}; index < oldCapacity; ++index)
	{
		CurrentElement = Head + index; // adjust pointer
		*CurrentElement = std::move(*(pOldHead + index)); // move element from old memory over
	}

	DeleteData(pOldHead, pOldTail);
	ReleaseMemory(pOldHead);
}

template<typename Type>
void CustomContainer<Type>::Reallocate(size_t newCapacity)
{
	const size_t oldCapacity{ size_t(Tail - Head) }; // also oldSize

	Type* pOldHead{ Head };
	Type* const pOldTail{ Tail };

	Head = static_cast<Type*>(malloc(SizeOfType * newCapacity));
	Tail = Head + newCapacity;

	for (size_t index{}; index < oldCapacity; ++index)
	{
		CurrentElement = Head + index; // adjust pointer
		*CurrentElement = std::move(*(pOldHead + index)); // move element from old memory over
	}

	DeleteData(pOldHead, pOldTail);
	ReleaseMemory(pOldHead);
}

template<typename Type>
template<typename ...Values>
void CustomContainer<Type>::ReallocateAndEmplace(Values && ...values)
{
	Reallocate();

	if (!CurrentElement)
	{
		CurrentElement = Head;
	}
	else
	{
		++CurrentElement;
	}

	/* [TODO]: AN ALLOCATOR SHOULD DO THIS */
	// CurrentElement = new Type(std::forward<Values>(val)...);
	*CurrentElement = Type(std::forward<Values>(values)...);
}