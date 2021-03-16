#pragma once
#include <iostream>
class OutOfRangeException
{
public:
	OutOfRangeException(const int line, const char* pFile) { std::cout << "Calling At() from file: " << pFile << " at line: " << line << " was out of bounds" << std::endl; }
};

template<typename Type>
class CustomContainer
{
	inline static constexpr size_t size = sizeof(Type);
public:
	CustomContainer() noexcept
	{
		const uint16_t capacity{ 5 }; // this could be any start value
		m_pHead = static_cast<Type*>(calloc(capacity, size)); // maybe make m_pHead actually useful instead of storing memory?
		m_pTail = m_pHead + capacity;
	}

	~CustomContainer() noexcept
	{
		ReleaseOldMemory(m_pHead);
	}

	inline void Push_back(const Type& val) noexcept
	{
		if (!m_pCurrentElement) // first time a push_back happens
		{
			Type* pNextFreeBlock{ m_pHead + 1 };

			m_pCurrentElement = pNextFreeBlock;
			*m_pCurrentElement = val;
		}
		else
		{
			// first check if we need to reallocate memory
			Type* pNextFreeBlock{ m_pCurrentElement + 1 };
			if (pNextFreeBlock >= m_pTail)
			{
				Reallocate(); // we need to reallocate
				pNextFreeBlock = m_pCurrentElement + 1;
			}

			m_pCurrentElement = pNextFreeBlock;
			*m_pCurrentElement = val;
		}
	}

	inline void Push_back(Type&& val) noexcept
	{
		if (!m_pCurrentElement) // first time a push_back happens
		{
			Type* pNextFreeBlock{ m_pHead + 1 };

			m_pCurrentElement = pNextFreeBlock;
			*m_pCurrentElement = val;
		}
		else
		{
			// first check if we need to reallocate memory
			Type* pNextFreeBlock{ m_pCurrentElement + 1 };
			if (pNextFreeBlock >= m_pTail)
			{
				Reallocate(); // we need to reallocate
				pNextFreeBlock = m_pCurrentElement + 1;
			}

			m_pCurrentElement = pNextFreeBlock;
			*m_pCurrentElement = val;
		}
	}

	void Clear() noexcept
	{
		if constexpr (!std::is_trivially_destructible_v<Type>) // if this is a struct / class with a custom destructor, call it
		{
			const size_t size{ m_pCurrentElement - m_pHead };
			for (size_t index{ 1 }; index < size; ++index)
			{
				m_pCurrentElement = m_pHead + index; // adjust pointer

				delete m_pCurrentElement;
				m_pCurrentElement = nullptr;
			}
		}

		m_pCurrentElement = nullptr;
	}

	[[nodiscard]] inline size_t GetSize() const noexcept
	{
		if (!m_pCurrentElement)
			return 0;

		return (m_pCurrentElement - m_pHead);
	}

	[[nodiscard]] inline size_t GetCapacity() const noexcept
	{
		return (m_pTail - m_pHead);
	}

	[[nodiscard]] inline Type& GetFront() noexcept
	{
		return *(m_pHead + 1);
	}

	[[nodiscard]] inline const Type& GetFront() const noexcept
	{
		return *(m_pHead + 1);
	}

	[[nodiscard]] inline Type& GetBack() noexcept
	{
		return *m_pCurrentElement;
	}

	[[nodiscard]] inline const Type& GetBack() const noexcept
	{
		return *m_pCurrentElement;
	}

	[[nodiscard]] inline Type& At(const int index)
	{
		if ((m_pHead + (index + 1)) > m_pCurrentElement)
			throw OutOfRangeException{ __LINE__, __FILE__ };

		return *(m_pHead + (index + 1));
	}

	[[nodiscard]] inline const Type& At(const int index) const
	{
		if ((m_pHead + (index + 1)) > m_pTail)
			throw OutOfRangeException{ __LINE__, __FILE__ };

		return *(m_pHead + (index + 1));
	}

	[[nodiscard]] inline bool IsEmpty() const noexcept
	{
		return m_pCurrentElement == nullptr;
	}

	[[nodiscard]] inline Type& operator[](const int index) noexcept
	{
		return *(m_pHead + (index + 1));
	}

	[[nodiscard]] inline const Type& operator[](const int index) const noexcept
	{
		return *(m_pHead + (index + 1));
	}

private:
	void ReleaseOldMemory(Type* pOldHead) noexcept
	{
		if (pOldHead)
		{
			free(pOldHead);
			pOldHead = nullptr;
		}
	}

	void Reallocate() noexcept
	{
		const size_t oldSize{ size_t(m_pTail - m_pHead) }; // also oldCapacity
		const size_t newCapacity{ oldSize + 5 };

		Type* pOldHead{ m_pHead };

		m_pHead = static_cast<Type*>(calloc(newCapacity, size));
		m_pTail = m_pHead + newCapacity;

		for (size_t index{ 1 }; index < oldSize; ++index)
		{
			m_pCurrentElement = m_pHead + index; // adjust pointer
			*m_pCurrentElement = std::move(*(pOldHead + index)); // move element from old memory over
		}

		ReleaseOldMemory(pOldHead);
	}

	Type* m_pHead{ nullptr }; // does not contain data, only used as start of my memory block
	Type* m_pTail{ nullptr }; // same thing, only used as end of my memory block
	Type* m_pCurrentElement{ nullptr }; // current element
};