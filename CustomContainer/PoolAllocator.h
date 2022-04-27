#pragma once

#include <assert.h> /* assert() */
#include <cstdlib> /* malloc() */
#include <type_traits> /* std::type_traits */
#include <vector>

/* Based on the following article: */
/* https://arjunsreedharan.org/ */

inline static constexpr size_t AlignSize{ 8 };

/* MOVING THE VARIABLES INSIDE THIS STRUCT WILL BREAK THE ALLOCATOR, SEE PoolAllocator::deallocate() !!! */
struct alignas(AlignSize) BlockInformation final
{
	size_t BlockSize;
	bool IsFree;

	BlockInformation* pNext;
	void* Data;
};

class PoolAllocator final
{
public:
	PoolAllocator(const PoolAllocator& other) noexcept = delete;
	PoolAllocator(PoolAllocator&& other) noexcept = delete;
	PoolAllocator& operator=(const PoolAllocator& other) noexcept = delete;
	PoolAllocator& operator=(PoolAllocator&& other) noexcept = delete;

	template<typename Type>
	static Type* allocate(const size_t nrOfElementsToAllocate);

	template<typename Type>
	static void deallocate(Type*& pBlock);

	template<typename Type, typename ... Values>
	static void construct(Type* p, Values&&... value);

	template<typename Type>
	static void destroy(Type* p);

	static void ReleaseAllMemory() noexcept;

private:
	template<typename Type>
	static BlockInformation* GetFreeBlock(size_t bucket);

	inline static std::vector<std::vector<void*>> BlockLists =
	{
		std::vector<void*>{}, /* 8 bytes */
		std::vector<void*>{}, /* 16 bytes */
		std::vector<void*>{}, /* 32 bytes */
		std::vector<void*>{}, /* 64 bytes */
		std::vector<void*>{}, /* 128 bytes */
		std::vector<void*>{} /* > 128 bytes */
	};

	inline static constexpr size_t AmountOfLists{ 6 };
};

template<typename Type>
Type* PoolAllocator::allocate(const size_t nrOfElementsToAllocate)
{
	assert(nrOfElementsToAllocate != 0);

	const size_t totalSize{ sizeof(BlockInformation) + nrOfElementsToAllocate * sizeof(Type) };
	size_t bucket{ totalSize / sizeof(void*) - 1 };

	if (bucket >= AmountOfLists)
	{
		bucket = AmountOfLists - 1;
	}
	
	BlockInformation* pBlockInfo{ GetFreeBlock<Type>(bucket) };

	if (pBlockInfo)
	{
		pBlockInfo->IsFree = false;
		pBlockInfo->Data = nullptr;
		return new (&pBlockInfo->Data) Type();
	}

	void* const pBlock{ malloc(totalSize) };

	assert(pBlock);

	pBlockInfo = static_cast<BlockInformation*>(pBlock);
	pBlockInfo->BlockSize = nrOfElementsToAllocate * sizeof(Type);
	pBlockInfo->IsFree = false;
	pBlockInfo->pNext = nullptr;
	pBlockInfo->Data = nullptr;
	Type* pData = new (&pBlockInfo->Data) Type();

	//if (!Head)
	//{
	//	Head = pBlockInfo;
	//}

	//if (Tail)
	//{
	//	Tail->pNext = pBlockInfo;
	//}

	//Tail = pBlockInfo;

	BlockLists[bucket].push_back(pBlockInfo);

	return pData;
}

template<typename Type>
void PoolAllocator::deallocate(Type*& pBlock)
{
	assert(pBlock);

	/* THIS IS EXTREMELY HARDCODED, BUT THIS COULD ONLY BE SOLVED VIA REFLECTION */

	/* Move back 3 addresses for the start of the BlockInformation */
	BlockInformation* Block{ reinterpret_cast<BlockInformation*>(
		reinterpret_cast<char*>(&pBlock) - sizeof(BlockInformation*) - sizeof(bool) * AlignSize - sizeof(size_t)) };

	Block->IsFree = true;
}

template<typename Type, typename ... Values>
void PoolAllocator::construct(Type* p, Values&&... value)
{
	new (p) Type(std::forward<Values>(value)...);
}

template<typename Type>
void PoolAllocator::destroy(Type* p)
{
	if constexpr (!std::is_trivially_destructible_v<Type>)
	{
		if (p)
		{
			p->~Type();
		}
	}
}

template<typename Type>
BlockInformation* PoolAllocator::GetFreeBlock(size_t bucket)
{
	//BlockInformation* pCurrent{ BlockLists[bucket] };

	//while (pCurrent)
	//{
	//	if (pCurrent->IsFree && pCurrent->BlockSize >= sizeInBytes * sizeof(Type))
	//	{
	//		return pCurrent;
	//	}

	//	pCurrent = pCurrent->pNext;
	//}

	//return nullptr;

	const std::vector<void*>& blockInfos{ BlockLists[bucket] };

	for (void* pBlock : blockInfos)
	{
		if (static_cast<BlockInformation*>(pBlock)->IsFree)
		{
			return static_cast<BlockInformation*>(pBlock);
		}
	}

	return nullptr;
}