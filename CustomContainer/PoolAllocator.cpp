#include "PoolAllocator.h"

void PoolAllocator::ReleaseAllMemory() noexcept
{
	//BlockInformation* pTemp{ Head };

	//while (pTemp)
	//{
	//	BlockInformation* pNext{ pTemp->pNext };

	//	free(pTemp);

	//	pTemp = pNext;
	//}

	for (const auto& list : BlockLists)
	{
		for (void* const pBlock : list)
		{
			free(pBlock);
		}
	}
}
