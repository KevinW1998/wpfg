#pragma once

#include <stdlib.h>

class BlockAllocator
{
public:
	static void* Allocate(size_t cb);
	static void Free(void* p);

private:

    class Block
    {
    public:
    
    	bool Init(int cbFixedAlloc, int blockIndex, int cChunks)
    	{
    		m_pPrevFreeBlock = NULL;
    		m_pNextFreeBlock = NULL;
    		m_cAlloc = 0;
    		m_cbFixedAlloc = cbFixedAlloc;
    		m_blockIndex = blockIndex;
    		m_cChunks = cChunks;
    
    		m_pLastByte = (unsigned char*)this + GetAllocSize(m_cbFixedAlloc, m_cChunks) - 1;
    
    		m_ppFreeChunk = NULL;
    		m_pCurrent = (unsigned char*)this + GetHeaderSize(m_cChunks);
    
    		m_cTotal = m_cChunks * 16;
    
    		return true;
    	}
    
    	bool IsEmpty()
    	{
    		return (m_cAlloc == 0);
    	}
    
    	bool isFull()
    	{
    		return m_cAlloc == m_cTotal;
    	}
    
    	void* alloc()
    	{
    		void* pResult;
    
    		if (m_ppFreeChunk)
    		{
    			pResult = m_ppFreeChunk;
    			m_ppFreeChunk = (void**)*m_ppFreeChunk;
    		}
    		else
    		{
    			pResult = m_pCurrent;
    			m_pCurrent += m_cbFixedAlloc;
    		}
    
    		m_cAlloc++;
    		return pResult;
    	}
    
    	void free(void* p)
    	{
    		void **pp = (void**)p;
    		*pp = m_ppFreeChunk;
    		m_ppFreeChunk = (void**)p;
    		m_cAlloc--;
    	}
    
    	static unsigned long GetHeaderSize(int)
    	{
    		return sizeof(Block);
    	}
    
    	static unsigned long GetAllocSize(int fixedAllocSize, int chunks)
    	{
    		return GetHeaderSize(chunks) + (fixedAllocSize * 16 * chunks) + 8;
    	}
    
    	unsigned char* GetLastByte()
    	{
    		return m_pLastByte;
    	}

        int GetBlockFixedSize()
        {
            return m_cbFixedAlloc;
        }
    
    public:
    	Block* m_pNextFreeBlock;
    	Block* m_pPrevFreeBlock;
    	void** m_ppFreeChunk;
    	int m_cTotal;
    	int m_cAlloc;
    
    	unsigned char *m_pCurrent;
    	unsigned short m_cbFixedAlloc;
    	unsigned short m_cChunks;
    	unsigned char m_blockIndex;
    	unsigned char* m_pLastByte;
    };
    

    struct BlockDescriptor
    { 
        int fixedAllocSize;
        int chunks;
    };


    static void Init();
	static int FindBlockIndex(void* b);
	static Block* FindBlockInArray(void* p);

	static void AddBlockToArray(Block* pBlock);
	static void RemoveBlockFromArray(Block* pBlock);


    static const int c_blockDescriptorCount = 129;

    static BlockDescriptor m_rgBlockDescriptors[c_blockDescriptorCount];
	static Block* m_rgFreeBlocks[c_blockDescriptorCount];
	static int m_cBlocks;
	static int m_cBlockArraySize;
	static Block** m_rgpBlocks; 
	static Block** m_rgpBlockEnd;
	static bool m_isInitialized;
	static Block* m_pLastFoundBlock;
	static unsigned char m_rgBlockIndexLookup[1025];
};


