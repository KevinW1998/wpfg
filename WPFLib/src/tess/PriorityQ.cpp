/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
** 
** http://oss.sgi.com/projects/FreeB
** 
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
** 
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
** 
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
**
*/
/*
** Author: Eric Veach, July 1994.
**
** $Date: 2001/03/17 00:25:41 $ $Revision: 1.1 $
** $Header: /cvs/mesa/Mesa/src/glu/sgi/libtess/priorityq-heap.c,v 1.1 2001/03/17 00:25:41 brianp Exp $
*/

#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include "PriorityQ.h"

namespace Tess
{


PriorityQ::PriorityQ(compareCallback pfnCallback) : m_pfnCallback(pfnCallback)
{
    m_size = 0;
    m_max = c_initSize;
    m_nodes = new PQNode[m_max + 1];
    m_handles = new PQHandleElem[m_max + 1];
    m_isInitialized = false;
    m_freeList = NULL;

    m_nodes[1].handle = 1;
    m_handles[1].key = NULL;
}

PriorityQ::~PriorityQ()
{
    delete[] m_handles;
    delete[] m_nodes;
}


void PriorityQ::FloatDown(int curr)
{
    PQNode *n = m_nodes;
    PQHandleElem *h = m_handles;
    PQHandle hCurr, hChild;
    int child;

    hCurr = n[curr].handle;
    for ( ;; )
    {
        child = curr * 2;
        if ( child < m_size && m_pfnCallback( h[n[child+1].handle].key,
                                              h[n[child].handle].key ))
        {
            ++child;
        }

        assert(child <= m_max);

        hChild = n[child].handle;
        if ( child > m_size || m_pfnCallback( h[hCurr].key, h[hChild].key ))
        {
            n[curr].handle = hCurr;
            h[hCurr].node = curr;
            break;
        }
        n[curr].handle = hChild;
        h[hChild].node = curr;
        curr = child;
    }
}


void PriorityQ::FloatUp(int curr)
{
    PQNode *n = m_nodes;
    PQHandleElem *h = m_handles;
    PQHandle hCurr, hParent;
    int parent;

    hCurr = n[curr].handle;
    for ( ;; )
    {
        parent = curr / 2;
        hParent = n[parent].handle;
        if (parent == 0 || m_pfnCallback(h[hParent].key, h[hCurr].key))
        {
            n[curr].handle = hCurr;
            h[hCurr].node = curr;
            break;
        }
        n[curr].handle = hParent;
        h[hParent].node = curr;
        curr = parent;
    }
}

void PriorityQ::Init()
{
    int i;

    /* This method of building a heap is O(n), rather than O(n lg n). */

    for ( i = m_size; i >= 1; --i )
    {
        FloatDown(i);
    }
    m_isInitialized = true;
}


PQHandle PriorityQ::Insert(PQKey keyNew)
{
    int curr;
    PQHandle freeHandle;

    curr = ++m_size;
    if ( (curr*2) > m_max )
    {
        PQNode *saveNodes= m_nodes;
        PQHandleElem *saveHandles= m_handles;

        /* If the heap overflows, double its size. */
        m_max *= 2;

        m_nodes = new PQNode[m_max + 1];
        memcpy(m_nodes, saveNodes, ((m_max / 2) + 1) * sizeof(PQNode));
        delete[] saveNodes;

        m_handles = new PQHandleElem[m_max + 1];
        memcpy(m_handles, saveHandles, ((m_max / 2) + 1) * sizeof(PQHandleElem));
        delete[] saveHandles;
    }

    if ( m_freeList == 0 )
    {
        freeHandle = curr;
    }
    else
    {
        freeHandle = m_freeList;
        m_freeList = m_handles[freeHandle].node;
    }

    m_nodes[curr].handle = freeHandle;
    m_handles[freeHandle].node = curr;
    m_handles[freeHandle].key = keyNew;

    if ( m_isInitialized )
    {
        FloatUp(curr);
    }
    assert(freeHandle != -1);

    return freeHandle;
}

PQKey PriorityQ::ExtractMin()
{
    PQNode *n = m_nodes;
    PQHandleElem *h = m_handles;
    PQHandle hMin = n[1].handle;
    PQKey min = h[hMin].key;

    if (m_size > 0)
    {
        n[1].handle = n[m_size].handle;
        h[n[1].handle].node = 1;

        h[hMin].key = NULL;
        h[hMin].node = m_freeList;
        m_freeList = hMin;

        if ( --m_size > 0 )
        {
            FloatDown(1);
        }
    }
    return min;
}

void PriorityQ::Delete(PQHandle hCurr)
{
    PQNode *n = m_nodes;
    PQHandleElem *h = m_handles;
    int curr;

    assert( hCurr >= 1 && hCurr <= m_max && h[hCurr].key != NULL );

    curr = h[hCurr].node;
    n[curr].handle = n[m_size].handle;
    h[n[curr].handle].node = curr;

    if ( curr <= --m_size )
    {
        if ( curr <= 1 || m_pfnCallback( h[n[curr>>1].handle].key, h[n[curr].handle].key ))
        {
            FloatDown(curr);
        }
        else
        {
            FloatUp(curr);
        }
    }

    h[hCurr].key = NULL;
    h[hCurr].node = m_freeList;
    m_freeList = hCurr;
}


} // namespace Tess
