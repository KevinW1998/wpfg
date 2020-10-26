#ifndef __poolallocator_h_
#define __poolallocator_h_

namespace Tess
{


template <class T, int CHUNKSIZE>
class PoolAllocator
{
public:
    PoolAllocator() : m_pFreeList(NULL), m_pAllocationChunks(NULL) { }
    ~PoolAllocator()
    {
        AllocationChunk* pChunkCur = m_pAllocationChunks;
        AllocationChunk* pChunkNext = NULL;

        while(pChunkCur != NULL)
        {
            pChunkNext = pChunkCur->pNext;
            delete[] pChunkCur->prgAllocation;

            delete pChunkCur;

            pChunkCur = pChunkNext;
        }
    }

    T* Allocate()
    {
        if(m_pFreeList == NULL)
        {
            AllocateNewChunk();
        }

        T* pReturn = m_pFreeList;
        m_pFreeList = m_pFreeList->pNext;

        return pReturn;
    }

    void Free(T* pElement)
    {
        TElementNode* pElementNode = static_cast<TElementNode *>(pElement);

        pElementNode->pNext = m_pFreeList;
        m_pFreeList = pElementNode;
    }

private:

    void AllocateNewChunk()
    {
        AllocationChunk* pChunkNew = new AllocationChunk();

        pChunkNew->prgAllocation = new TElementNode[CHUNKSIZE];
        for(int i = 0; i < CHUNKSIZE - 1; i++)
        {
            pChunkNew->prgAllocation[i].pNext = &(pChunkNew->prgAllocation[i + 1]);
        }
        pChunkNew->prgAllocation[CHUNKSIZE - 1].pNext = m_pFreeList;

        pChunkNew->pNext = m_pAllocationChunks;
        m_pAllocationChunks = pChunkNew;

        m_pFreeList = &(pChunkNew->prgAllocation[0]);
    }


    class TElementNode : public T
    {
    public:
        TElementNode* pNext;
    };

    struct AllocationChunk
    {
        TElementNode* prgAllocation;
        AllocationChunk* pNext;
    };

    TElementNode* m_pFreeList;
    AllocationChunk* m_pAllocationChunks;
};

}; // namespace Tess

#endif
