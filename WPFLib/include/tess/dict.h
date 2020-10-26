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
** $Header: /cvs/mesa/Mesa/src/glu/sgi/libtess/dict.h,v 1.1 2001/03/17 00:25:41 brianp Exp $
*/

#ifndef __dict_list_h_
#define __dict_list_h_

namespace Tess
{

typedef void *DictKey;

class DictNode
{
public:
    friend class Dict;

    DictNode* get_Next() { return m_pNext; }
    DictNode* get_Prev() { return m_pPrev; }
    DictKey get_Key() { return m_key; }

private:
    DictKey m_key;
    DictNode* m_pNext;
    DictNode* m_pPrev;
};


class Dict
{
public:
    typedef bool (*leq)(void* pFrame, DictKey key1, DictKey key2);

    Dict(void* pFrame, leq pfnLeq);
    ~Dict();

    DictNode* get_Head();
    void* get_Frame();

    /* Search returns the node with the smallest key greater than or equal
     * to the given key.  If there is no such key, returns a node whose
     * key is NULL.  Similarly, Succ(Max(d)) has a NULL key, etc.
     */
    DictNode* Search(DictKey key);
    DictNode* InsertBefore(DictNode* pNode, DictKey key);
    DictNode* Insert(DictKey key) { return InsertBefore(&m_headNode, key); }
    void Delete(DictNode* pNode);

    DictNode* get_Min() { return m_headNode.get_Next(); }
    DictNode* get_Max() { return m_headNode.get_Prev(); }

private:
    leq m_pfnLeq;
    DictNode m_headNode;
    void* m_pFrame;
};


} // namespace tess

#endif
