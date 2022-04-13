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

	CustomContainer<Type>(const CustomContainer<Type>& other) noexcept;
	CustomContainer<Type>(CustomContainer<Type>&& other) noexcept;
	CustomContainer<Type>& operator=(const CustomContainer<Type>& other) noexcept;
	CustomContainer<Type>& operator=(CustomContainer<Type>&& other) noexcept;

	inline void Push_back(const Type& val) noexcept
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

	void Clear() noexcept
	{
		if constexpr (!std::is_trivially_destructible_v<Type>) // if this is a struct / class with a custom destructor, call it
		{
			const size_t size{ CurrentElement - Head };
			for (size_t index{ 1 }; index < size; ++index)
			{
				CurrentElement = Head + index; // adjust pointer

				delete CurrentElement;
				CurrentElement = nullptr;
			}
		}

		CurrentElement = nullptr;
	}

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

	void Reallocate() noexcept
	{
		const size_t oldSize{ size_t(Tail - Head) }; // also oldCapacity
		const size_t newCapacity{ oldSize + 5 };

		Type* pOldHead{ Head };

		Head = static_cast<Type*>(calloc(newCapacity, SizeOfType));
		Tail = Head + newCapacity;

		for (size_t index{ 1 }; index < oldSize; ++index)
		{
			CurrentElement = Head + index; // adjust pointer
			*CurrentElement = *(pOldHead + index); // move element from old memory over
		}

		ReleaseOldMemory(pOldHead);
	}

	Type* Head{ nullptr }; // does not contain data, only used as start of my memory block
	Type* Tail{ nullptr }; // same thing, only used as end of my memory block
	Type* CurrentElement{ nullptr }; // current element
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
		*CurrentElement = *(other.Head + index + 1);
	}
}

template<typename Type>
CustomContainer<Type>::CustomContainer(CustomContainer<Type>&& other) noexcept
{
	const size_t capacity{ other.GetCapacity() }; // this could be any start value
	/* [TODO]: Change calloc to malloc */
	Head = static_cast<Type*>(calloc(capacity, SizeOfType)); // maybe make m_pHead actually useful instead of storing memory?
	Tail = Head + capacity;

	for (size_t index{  }; index < other.GetSize(); ++index)
	{
		CurrentElement = Head + index + 1;
		*CurrentElement = other.At(index);
	}

	other.Clear();
	other.ReleaseOldMemory(other.Head);
}

template<typename Type>
CustomContainer<Type>& CustomContainer<Type>::operator=(const CustomContainer<Type>& other) noexcept
{
	const size_t capacity{ other.GetCapacity() }; // this could be any start value
	/* [TODO]: Change calloc to malloc */
	Head = static_cast<Type*>(calloc(capacity, SizeOfType)); // maybe make m_pHead actually useful instead of storing memory?
	Tail = Head + capacity;

	for (size_t index{  }; index < other.GetSize(); ++index)
	{
		CurrentElement = Head + index + 1;
		*CurrentElement = other.At(index);
	}
}

template<typename Type>
CustomContainer<Type>& CustomContainer<Type>::operator=(CustomContainer<Type>&& other) noexcept
{
	const size_t capacity{ other.GetCapacity() }; // this could be any start value
	/* [TODO]: Change calloc to malloc */
	Head = static_cast<Type*>(calloc(capacity, SizeOfType)); // maybe make m_pHead actually useful instead of storing memory?
	Tail = Head + capacity;

	for (size_t index{  }; index < other.GetSize(); ++index)
	{
		CurrentElement = Head + index + 1;
		*CurrentElement = other.At(index);
	}

	other.Clear();
	other.ReleaseOldMemory(other.Head);
}