#include <AVUICommon.h>
#include <AVUIBlockAllocator.h>
#include <stdlib.h>

namespace AVUI
{

BlockAllocator::BlockDescriptor BlockAllocator::m_rgBlockDescriptors[c_blockDescriptorCount];
BlockAllocator::Block* BlockAllocator::m_rgFreeBlocks[c_blockDescriptorCount];
int BlockAllocator::m_cBlocks;
int BlockAllocator::m_cBlockArraySize;
BlockAllocator::Block** BlockAllocator::m_rgpBlocks; 
BlockAllocator::Block** BlockAllocator::m_rgpBlockEnd;
bool BlockAllocator::m_isInitialized;
BlockAllocator::Block* BlockAllocator::m_pLastFoundBlock;
unsigned char BlockAllocator::m_rgBlockIndexLookup[1025];

unsigned int g_cbTotalAllocated = 0;


#define ALLOCJR_FULLBLOCK (Block*)0xffffffff

void BlockAllocator::Init() 
{
	if (m_isInitialized) 
    {
        return;
    }

    m_isInitialized = true;

    m_cBlockArraySize = 10000;

	m_rgBlockDescriptors[0].fixedAllocSize = 4;
	m_rgBlockDescriptors[0].chunks = 3;

	for (int i = 1; i < c_blockDescriptorCount; i++)
	{
		int allocSize = i*8;
		int chunks = 11;

		m_rgBlockDescriptors[i].fixedAllocSize = allocSize;

		if (allocSize == 8) chunks = 3;
		else if (allocSize == 1008) chunks = 1;
		else if (allocSize == 1016) chunks = 1;
		else if (allocSize == 1024) chunks = 1;

		m_rgBlockDescriptors[i].chunks = chunks;
	}

	for (int i = 0; i < c_blockDescriptorCount; i++)
	{
		m_rgFreeBlocks[i] = NULL;
	}

	for (int i = 0; i <= 1024; i++)
	{
		m_rgBlockIndexLookup[i] = -1;
		for (int j = 0; j < c_blockDescriptorCount; j++)
		{
			if (i <= m_rgBlockDescriptors[j].fixedAllocSize)
			{
				m_rgBlockIndexLookup[i] = j;
				break;
			}
		}
	}

	m_rgpBlocks = NULL;
}

void* BlockAllocator::Allocate(size_t ls)
{
    if(!m_isInitialized)
    {
        Init();
    }

	if (ls == 0) ls = 1;
	int bdIndex = -1;
	if (ls <= 1024) bdIndex = m_rgBlockIndexLookup[ls];

	if (bdIndex < 0)
	{
		return malloc(ls);
	}
	else
	{
		void* result = NULL;

		if (!m_rgFreeBlocks[bdIndex])
		{
			m_rgBlockDescriptors[bdIndex].chunks*=2;
			if (m_rgBlockDescriptors[bdIndex].chunks > 20) m_rgBlockDescriptors[bdIndex].chunks = 1;

            Block* b = (Block*)malloc(Block::GetAllocSize(m_rgBlockDescriptors[bdIndex].fixedAllocSize, m_rgBlockDescriptors[bdIndex].chunks));
			if (b)
			{
				b->Init(m_rgBlockDescriptors[bdIndex].fixedAllocSize, bdIndex, m_rgBlockDescriptors[bdIndex].chunks);

				AddBlockToArray(b);
				m_rgFreeBlocks[bdIndex] = b;
			}
		}
		if (m_rgFreeBlocks[bdIndex])
		{
			result = m_rgFreeBlocks[bdIndex]->alloc();
			Block *b = m_rgFreeBlocks[bdIndex];

			if (b->m_pNextFreeBlock != ALLOCJR_FULLBLOCK && b->isFull())
			{
				if (b->m_pNextFreeBlock)
				{
					b->m_pNextFreeBlock->m_pPrevFreeBlock = b->m_pPrevFreeBlock;
				}
				if (b->m_pPrevFreeBlock)
				{
					b->m_pPrevFreeBlock->m_pNextFreeBlock = b->m_pNextFreeBlock;
				}

				m_rgFreeBlocks[bdIndex] = b->m_pNextFreeBlock;
				b->m_pNextFreeBlock = ALLOCJR_FULLBLOCK; // special value means removed from free list
				b->m_pPrevFreeBlock = ALLOCJR_FULLBLOCK;
			}

            g_cbTotalAllocated += b->GetBlockFixedSize();
		}

		return result;
	}
}

void BlockAllocator::Free(void* p)
{
    if(!m_isInitialized)
    {
        Init();
    }

	if (!p) 
    {
        return;
    }

	Block* b = FindBlockInArray(p);
	if (b)
	{
        g_cbTotalAllocated -= b->GetBlockFixedSize();

		b->free(p);

        if (b->IsEmpty())
		{
			if (b->m_pNextFreeBlock)
			{
				b->m_pNextFreeBlock->m_pPrevFreeBlock = b->m_pPrevFreeBlock;
			}
			if (b->m_pPrevFreeBlock)
			{
				b->m_pPrevFreeBlock->m_pNextFreeBlock = b->m_pNextFreeBlock;
			}
			if (m_rgFreeBlocks[b->m_blockIndex] == b) 
            {
                m_rgFreeBlocks[b->m_blockIndex] = b->m_pNextFreeBlock;
            }

			RemoveBlockFromArray(b);

            free(b);
		}
		else
		{
			// need to see if block is not in free list if not add it back
			if (b->m_pNextFreeBlock == ALLOCJR_FULLBLOCK)
			{
				b->m_pPrevFreeBlock = NULL;
				b->m_pNextFreeBlock = m_rgFreeBlocks[b->m_blockIndex];
				if (m_rgFreeBlocks[b->m_blockIndex])
                {
                    m_rgFreeBlocks[b->m_blockIndex]->m_pPrevFreeBlock = b;
                }

				m_rgFreeBlocks[b->m_blockIndex] = b;
			}
		}
	}
	else
	{
		free(p);
	}
}

void BlockAllocator::AddBlockToArray(Block* b)
{
	if (!m_rgpBlocks)
	{
		m_rgpBlocks = (Block**)malloc(sizeof(Block**)*m_cBlockArraySize);
		m_rgpBlockEnd = m_rgpBlocks+m_cBlocks-1;
	}
	if (m_cBlockArraySize < m_cBlocks+1)
	{
		m_cBlockArraySize += 10000;
		m_rgpBlocks = (Block**)realloc(m_rgpBlocks, sizeof(Block**)*m_cBlockArraySize);
		m_rgpBlockEnd = m_rgpBlocks+m_cBlocks-1;
	}
	bool done = false;
	long s = 0; 
	long e = m_cBlocks-1;
	long m = s + ((e-s+1)/2);
	while (s < e)
	{
		if (b < m_rgpBlocks[m])
		{
			e = m-1;
		}
		else if ((unsigned char*)b > m_rgpBlocks[m]->GetLastByte())
		{
			s = m+1;
		}
		m = s+ ((e-s+1)/2);
	}
	if (s<m_cBlocks && (unsigned char*)b > (unsigned char*)m_rgpBlocks[s]->GetLastByte())
	{
		s++;
	}

	// We want to insert as s
	for (long i = m_cBlocks-1; i >= s; i--)
	{
		m_rgpBlocks[i+1] = m_rgpBlocks[i];
	}

	m_rgpBlocks[s] = b;
	m_cBlocks++;
	m_rgpBlockEnd = m_rgpBlocks+m_cBlocks-1;
}	


int BlockAllocator::FindBlockIndex(void* b)
{
	int result = -1;

	if (m_cBlocks > 0)
	{
		Block** s = m_rgpBlocks;
		Block** e = m_rgpBlockEnd;
		Block** m = s + ((e-s+1)>>1);

		while (s < m) 
		{
			if ((unsigned char*)b <  (unsigned char*)*m) e = m-1;
			else if ((unsigned char*)b >= (unsigned char*)*m) s = m;
			m = s + ((e-s+1)>>1);
		}

		if (m >= m_rgpBlocks && m <= m_rgpBlockEnd) // valid block
			if ((unsigned char*)b >= (unsigned char*)*m && (unsigned char*)b <= (*m)->GetLastByte())
				result = m-m_rgpBlocks;

	}

	return result;
}


void BlockAllocator::RemoveBlockFromArray(Block* b)
{
	int m = FindBlockIndex(b);

	if (m >=0)
	{
		if (m_pLastFoundBlock == m_rgpBlocks[m]) 
        {
            m_pLastFoundBlock = NULL;
        }

		for (long i = m+1; i < m_cBlocks; i++)
		{
			m_rgpBlocks[i-1] = m_rgpBlocks[i];
		}
		m_cBlocks--;
		m_rgpBlockEnd = m_rgpBlocks+m_cBlocks-1;

		if (m_cBlocks == 0)
		{
			free(m_rgpBlocks);
			m_rgpBlocks = NULL;
			m_rgpBlockEnd = NULL;
		}	
	}
}


BlockAllocator::Block* BlockAllocator::FindBlockInArray(void* p)
{
	if (m_pLastFoundBlock)
	{
		if ((unsigned char*)m_pLastFoundBlock <= p && p <= m_pLastFoundBlock->GetLastByte())
		{
			return m_pLastFoundBlock;
		}
	}

	Block* result = NULL;
	if (m_cBlocks > 0)
	{

		Block** s = m_rgpBlocks;
		Block** e = m_rgpBlockEnd;
		Block** m = s + ((e-s+1)>>1);
		while (s < m) 
		{
			if ((unsigned char*)p <  (unsigned char*)*m) e = m-1;
			else if ((unsigned char*)p >= (unsigned char*)*m) s = m;
			m = s + ((e-s+1)>>1);
		}
		if (m >= m_rgpBlocks && m <= m_rgpBlockEnd) // valid block
			if ((unsigned char*)p >= (unsigned char*)*m && (unsigned char*)p <= (*m)->GetLastByte())
				result = m_pLastFoundBlock = *m;
	}
	return result;
}

}

