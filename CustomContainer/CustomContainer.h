#pragma once
#include "Utils.h"

template<typename Type>
class CustomContainer final
{
	inline static constexpr size_t SizeOfType = sizeof(Type);
public:
	CustomContainer() = default;

	~CustomContainer()
	{
		ReleaseOldMemory(Head);
	}

	/* If these aren't marked as noexcept, VS keeps complaining */
	CustomContainer<Type>(const CustomContainer<Type>& other) noexcept;
	CustomContainer<Type>(CustomContainer<Type>&& other) noexcept;
	CustomContainer<Type>& operator=(const CustomContainer<Type>& other) noexcept;
	CustomContainer<Type>& operator=(CustomContainer<Type>&& other) noexcept;

	void Add(const Type& val);
	inline void Push_back(Type&& val) noexcept
	{
		if (!CurrentElement) // first time a push_back happens
		{
			Type* pNextFreeBlock{ Head + 1 };

			CurrentElement = pNextFreeBlock;
			*CurrentElement = val;
		}
		else
		{
			// first check if we need to reallocate memory
			Type* pNextFreeBlock{ CurrentElement + 1 };
			if (pNextFreeBlock >= Tail)
			{
				Reallocate(); // we need to reallocate
				pNextFreeBlock = CurrentElement + 1;
			}

			CurrentElement = pNextFreeBlock;
			*CurrentElement = val;
		}
	}

	template<typename ... Values>
	void Emplace(Values&&... val);

	void Clear();

	size_t GetSize() const noexcept
	{
		if (!CurrentElement)
			return 0;

		return (CurrentElement - Head);
	}

	size_t GetCapacity() const noexcept
	{
		return (Tail - Head);
	}

	Type& GetFront() noexcept
	{
		return *(Head + 1);
	}

	const Type& GetFront() const noexcept
	{
		return *(Head + 1);
	}

	Type& GetBack() noexcept
	{
		return *CurrentElement;
	}

	const Type& GetBack() const noexcept
	{
		return *CurrentElement;
	}

	Type& At(const size_t index)
	{
		ASSERT(((Head + (index + 1)) > CurrentElement), "Container::At() > Index was out of range!");

		return *(Head + (index + 1));
	}

	const Type& At(const size_t index) const
	{
		ASSERT(((Head + (index + 1)) > Tail), "Container::At() > Index was out of range!");

		return *(Head + (index + 1));
	}

	const bool IsEmpty() const noexcept
	{
		return CurrentElement == nullptr;
	}

	Type& operator[](const size_t index) noexcept
	{
		return *(Head + (index + 1));
	}

	const Type& operator[](const size_t index) const noexcept
	{
		return *(Head + (index + 1));
	}

private:
	void ReleaseOldMemory(Type*& pOldHead) noexcept
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

	void Reallocate() noexcept
	{
		ASSERT(CurrentElement >= Tail, "Container::Reallocate() > Reallocating while there is still memory left!");

		const size_t oldCapacity{ size_t(Tail - Head) }; // also oldSize
		const size_t newCapacity{ oldCapacity != 0 ? oldCapacity * 2 : 1 };

		Type* const pOldHead{ Head };
		Type* const pOldTail{ Tail };

		/* [TODO]: Change calloc to malloc */
		Head = static_cast<Type*>(calloc(newCapacity, SizeOfType));
		Tail = Head + newCapacity;

		for (size_t index{}; index < oldCapacity; ++index)
		{
			CurrentElement = Head + index; // adjust pointer
			*CurrentElement = std::move(*(pOldHead + index)); // move element from old memory over
		}

		DeleteData(pOldHead, pOldTail);
		ReleaseOldMemory(pOldHead);
	}

	Type* Head{ nullptr };
	Type* Tail{ nullptr };
	Type* CurrentElement{ nullptr };
};

template<typename Type>
CustomContainer<Type>::CustomContainer(const CustomContainer<Type>& other) noexcept
{
	const size_t capacity{ other.GetCapacity() };

	/* [TODO]: Change calloc to malloc */
	Head = static_cast<Type*>(calloc(capacity, SizeOfType));
	Tail = Head + capacity;

	for (size_t index{}; index < other.GetSize(); ++index)
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
	for (size_t index{}; index < other.GetSize(); ++index)
	{
		*(Head + index) = std::move(*(other.Head + index));
	}

	other.Clear();
	other.ReleaseOldMemory(other.Head);

	other.Head = nullptr;
	other.Tail = nullptr;
	other.CurrentElement = nullptr;
}

template<typename Type>
CustomContainer<Type>& CustomContainer<Type>::operator=(const CustomContainer<Type>& other) noexcept
{
	const size_t capacity{ other.GetCapacity() };

	/* [TODO]: Change calloc to malloc */
	Head = static_cast<Type*>(calloc(capacity, SizeOfType));
	Tail = Head + capacity;

	for (size_t index{}; index < other.GetSize(); ++index)
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

	for (size_t index{}; index < other.GetSize(); ++index)
	{
		*(Head + index) = std::move(*(other.Head + index));
	}

	other.Clear();
	other.ReleaseOldMemory(other.Head);

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
template<typename ...Values>
void CustomContainer<Type>::Emplace(Values&&... val)
{

}

template<typename Type>
void CustomContainer<Type>::Clear()
{
	DeleteData(Head, Tail);
}