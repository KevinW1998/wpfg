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
** $Header: /cvs/mesa/Mesa/src/glu/sgi/libtess/mesh.c,v 1.1 2001/03/17 00:25:41 brianp Exp $
*/

#include <stdlib.h>
#include <assert.h>
#include "tess.h"
#include "mesh.h"
#include "dict.h"

namespace Tess
{

ActiveRegion* ActiveRegion::get_RegionBelow()
{
    return(ActiveRegion*) (get_NodeUp()->get_Prev()->get_Key());
}

ActiveRegion* ActiveRegion::get_RegionAbove()
{
    return(ActiveRegion*) (get_NodeUp()->get_Next()->get_Key());
}

ActiveRegion *ActiveRegion::get_TopLeftRegion(Mesh* pMesh)
{
    ActiveRegion* reg = this;
    Vertex *org = get_EUp()->get_Org();
    HalfEdge *e;

    /* Find the region above the uppermost edge with the same origin */
    do
    {
        reg = reg->get_RegionAbove();

    } while ( reg->get_EUp()->get_Org() == org );

    /* If the edge above was a temporary edge introduced by ConnectRightVertex,
     * now is the time to fix it.
     */
    if ( reg->get_FixUpperEdge())
    {
        e = pMesh->Connect(reg->get_RegionBelow()->get_EUp()->get_Sym(), reg->get_EUp()->get_LNext());
        if (e == NULL)
        {
            return NULL;
        }

        if( !reg->FixUpperEdge(e, pMesh))
        {
            return NULL;
        }

        reg = reg->get_RegionAbove();
    }
    return reg;
}

ActiveRegion *ActiveRegion::get_TopRightRegion()
{
    ActiveRegion* reg = this;
    Vertex *dst = reg->get_EUp()->get_Dst();

    /* Find the region above the uppermost edge with the same destination */
    do 
    {
        reg = reg->get_RegionAbove();

    } while( reg->get_EUp()->get_Dst() == dst );
    
    return reg;
}

/*
 * Replace an upper edge which needs fixing (see ConnectRightVertex).
 */
int ActiveRegion::FixUpperEdge(HalfEdge* pNewEdge, Mesh* pMesh)
{    
    assert(get_FixUpperEdge());
    if ( !pMesh->Delete(get_EUp()))
    {
        return 0;
    }

    set_FixUpperEdge(false);
    set_EUp(pNewEdge);
    pNewEdge->set_ActiveRegion(this);

    return 1;

}



/************************ Utility Routines ************************/

/* MakeEdge creates a new pair of half-edges which form their own loop.
 * No vertex or face structures are allocated, but these must be assigned
 * before the current edge operation is completed.
 */
HalfEdge *HalfEdge::MakeEdge(HalfEdgeAllocator* pAllocator)
{
    HalfEdge *e =  pAllocator->Allocate();
    HalfEdge *eSym = pAllocator->Allocate();
    HalfEdge *ePrev;

    /* Insert in circular doubly-linked list before eNext.
     * Note that the prev pointer is stored in Sym->next.
     */

    ePrev = get_Sym()->get_Next();
    eSym->set_Next(ePrev);
    ePrev->get_Sym()->set_Next(e);
    e->set_Next(this);
    get_Sym()->set_Next(eSym);

    e->set_Sym(eSym);
    e->set_ONext(e);
    e->set_LNext(eSym);
    e->set_Org(NULL);
    e->set_LFace(NULL);
    e->set_Winding(0);
    e->set_ActiveRegion(NULL);
    e->set_IsMarked(false);

    eSym->set_Sym(e);
    eSym->set_ONext(eSym);
    eSym->set_LNext(e);
    eSym->set_Org(NULL);
    eSym->set_LFace(NULL);
    eSym->set_Winding(0);
    eSym->set_ActiveRegion(NULL);
    eSym->set_IsMarked(false);

    return e;
}

/* Splice( a, b ) is best described by the Guibas/Stolfi paper or the
 * CS348a notes (see mesh.h).  Basically it modifies the mesh so that
 * a->Onext and b->Onext are exchanged.  This can have various effects
 * depending on whether a and b belong to different face or vertex rings.
 * For more explanation see __gl_meshSplice() below.
 */
void HalfEdge::Splice( HalfEdge *a, HalfEdge *b )
{
    HalfEdge *aOnext = a->get_ONext();
    HalfEdge *bOnext = b->get_ONext();

    aOnext->get_Sym()->set_LNext(b);
    bOnext->get_Sym()->set_LNext(a);
    a->set_ONext(bOnext);
    b->set_ONext(aOnext);
}

/* MakeVertex( newVertex, eOrig, vNext ) attaches a new vertex and makes it the
 * origin of all edges in the vertex loop to which eOrig belongs. "vNext" gives
 * a place to insert the new vertex in the global vertex list.  We insert
 * the new vertex *before* vNext so that algorithms which walk the vertex
 * list will not see the newly created vertices.
 */

void Vertex::MakeVertex(HalfEdge *eOrig, Vertex *vNext)
{
    /* insert in circular doubly-linked list before vNext */

    Vertex *vPrev = vNext->get_Prev();

    set_Prev(vPrev);
    vPrev->set_Next(this);
    set_Next(vNext);
    vNext->set_Prev(this);

    m_pEdge = eOrig;
    /* leave coords, s, t undefined */

    /* fix other edges on this vertex loop */
    HalfEdge *e = eOrig;
    do
    {

        e->set_Org(this);
        e = e->get_ONext();

    } while ( e != eOrig );

}

/* MakeFace( newFace, eOrig, fNext ) attaches a new face and makes it the left
 * face of all edges in the face loop to which eOrig belongs.  "fNext" gives
 * a place to insert the new face in the global face list.  We insert
 * the new face *before* fNext so that algorithms which walk the face
 * list will not see the newly created faces.
 */
void Face::MakeFace(HalfEdge *eOrig, Face *fNext )
{

    /* insert in circular doubly-linked list before fNext */
    Face *fPrev = fNext->get_Prev();

    set_Prev(fPrev);
    fPrev->set_Next(this);

    set_Next(fNext);
    fNext->set_Prev(this);

    m_pEdge = eOrig;
    m_pTrail = NULL;
    m_isMarked = false;

    /* The new face is marked "inside" if the old one was.  This is a
     * convenience for the common case where a face has been split in two.
     */
    m_isInside = fNext->m_isInside;

    /* fix other edges on this face loop */
    HalfEdge* e = eOrig;
    do
    {

        e->set_LFace(this);
        e = e->get_LNext();

    } while ( e != eOrig );
}

/* KillEdge( eDel ) destroys an edge (the half-edges eDel and eDel->Sym),
 * and removes from the global edge list.
 */
void HalfEdge::KillEdge(HalfEdgeAllocator* pAllocator)
{
    HalfEdge *ePrev, *eNext;

    /* delete from circular doubly-linked list */
    eNext = get_Next();
    ePrev = get_Sym()->get_Next();
    eNext->get_Sym()->set_Next(ePrev);
    ePrev->get_Sym()->set_Next(eNext);

    pAllocator->Free(get_Sym());
    pAllocator->Free(this);
}


/* KillVertex( vDel ) destroys a vertex and removes it from the global
 * vertex list.  It updates the vertex loop to point to a given new vertex.
 */
void Vertex::KillVertex(Vertex *newOrg, VertexAllocator* pAllocator)
{
    HalfEdge *e, *eStart = get_Edge();
    Vertex *vPrev, *vNext;

    /* change the origin of all affected edges */
    e = eStart;
    do
    {
        e->set_Org(newOrg);
        e = e->get_ONext();
    } while ( e != eStart );

    /* delete from circular doubly-linked list */
    vPrev = get_Prev();
    vNext = get_Next();
    vNext->set_Prev(vPrev);
    vPrev->set_Next(vNext);

    pAllocator->Free(this);
}

/* KillFace( fDel ) destroys a face and removes it from the global face
 * list.  It updates the face loop to point to a given new face.
 */
void Face::KillFace(Face *newLface, FaceAllocator* pAllocator )
{
    HalfEdge *e, *eStart = get_Edge();
    Face *fPrev, *fNext;

    /* change the left face of all affected edges */
    e = eStart;
    do
    {
        e->set_LFace(newLface);
        e = e->get_LNext();
    } while ( e != eStart );

    /* delete from circular doubly-linked list */
    fPrev = get_Prev();
    fNext = get_Next();
    fNext->set_Prev(fPrev);
    fPrev->set_Next(fNext);

    pAllocator->Free(this);
}


/****************** Basic Edge Operations **********************/

/* MakeEdge creates one edge, two vertices, and a loop (face).
 * The loop consists of the two new half-edges.
 */
HalfEdge *Mesh::MakeEdge()
{
    Vertex *newVertex1= m_pTessellator->get_VertexAllocator()->Allocate();
    Vertex *newVertex2= m_pTessellator->get_VertexAllocator()->Allocate();
    Face *newFace= m_pTessellator->get_FaceAllocator()->Allocate();
    HalfEdge *e;

    e = m_edgeHead.MakeEdge(m_pTessellator->get_HalfEdgeAllocator());

    newVertex1->MakeVertex( e, &m_vertexHead );
    newVertex2->MakeVertex( e->get_Sym(), &m_vertexHead );
    newFace->MakeFace(e, &m_faceHead );

    return e;
}


/* __gl_meshSplice( eOrg, eDst ) is the basic operation for changing the
 * mesh connectivity and topology.  It changes the mesh so that
 *	eOrg->Onext <- OLD( eDst->Onext )
 *	eDst->Onext <- OLD( eOrg->Onext )
 * where OLD(...) means the value before the meshSplice operation.
 *
 * This can have two effects on the vertex structure:
 *  - if eOrg->Org != eDst->Org, the two vertices are merged together
 *  - if eOrg->Org == eDst->Org, the origin is split into two vertices
 * In both cases, eDst->Org is changed and eOrg->Org is untouched.
 *
 * Similarly (and independently) for the face structure,
 *  - if eOrg->Lface == eDst->Lface, one loop is split into two
 *  - if eOrg->Lface != eDst->Lface, two distinct loops are joined into one
 * In both cases, eDst->Lface is changed and eOrg->Lface is unaffected.
 *
 * Some special cases:
 * If eDst == eOrg, the operation has no effect.
 * If eDst == eOrg->Lnext, the new face will have a single edge.
 * If eDst == eOrg->Lprev, the old face will have a single edge.
 * If eDst == eOrg->Onext, the new vertex will have a single edge.
 * If eDst == eOrg->Oprev, the old vertex will have a single edge.
 */
int Mesh::MeshSplice(HalfEdge *eOrg, HalfEdge *eDst )
{
    bool isJoiningLoops = false;
    bool isJoiningVertices = false;

    if ( eOrg == eDst ) return 1;

    if ( eDst->get_Org() != eOrg->get_Org())
    {
        /* We are merging two disjoint vertices -- destroy eDst->Org */
        isJoiningVertices = true;
        eDst->get_Org()->KillVertex(eOrg->get_Org(), m_pTessellator->get_VertexAllocator());
    }
    if ( eDst->get_LFace() != eOrg->get_LFace())
    {
        /* We are connecting two disjoint loops -- destroy eDst->Lface */
        isJoiningLoops = true;
        eDst->get_LFace()->KillFace(eOrg->get_LFace(), m_pTessellator->get_FaceAllocator());
    }

    /* Change the edge structure */
    HalfEdge::Splice( eDst, eOrg );

    if (!isJoiningVertices)
    {
        Vertex *newVertex= m_pTessellator->get_VertexAllocator()->Allocate();

        /* We split one vertex into two -- the new vertex is eDst->Org.
         * Make sure the old vertex points to a valid half-edge.
         */
        newVertex->MakeVertex(eDst, eOrg->get_Org());
        eOrg->get_Org()->set_Edge(eOrg);
    }
    if (!isJoiningLoops)
    {
        Face *newFace= m_pTessellator->get_FaceAllocator()->Allocate();  

        /* We split one loop into two -- the new loop is eDst->Lface.
         * Make sure the old face points to a valid half-edge.
         */
        newFace->MakeFace(eDst, eOrg->get_LFace());
        eOrg->get_LFace()->set_Edge(eOrg);
    }

    return 1;
}


/* __gl_meshDelete( eDel ) removes the edge eDel.  There are several cases:
 * if (eDel->Lface != eDel->Rface), we join two loops into one; the loop
 * eDel->Lface is deleted.  Otherwise, we are splitting one loop into two;
 * the newly created loop will contain eDel->Dst.  If the deletion of eDel
 * would create isolated vertices, those are deleted as well.
 *
 * This function could be implemented as two calls to __gl_meshSplice
 * plus a few calls to memFree, but this would allocate and delete
 * unnecessary vertices and faces.
 */
int Mesh::Delete( HalfEdge *eDel )
{
    HalfEdge *eDelSym = eDel->get_Sym();
    bool isJoiningLoops = false;

    /* First step: disconnect the origin vertex eDel->Org.  We make all
     * changes to get a consistent mesh in this "intermediate" state.
     */
    if ( eDel->get_LFace() != eDel->get_RFace())
    {
        /* We are joining two loops into one -- remove the left face */
        isJoiningLoops = true;
        eDel->get_LFace()->KillFace(eDel->get_RFace(), m_pTessellator->get_FaceAllocator());
    }

    if ( eDel->get_ONext() == eDel )
    {
        eDel->get_Org()->KillVertex(NULL, m_pTessellator->get_VertexAllocator());
    }
    else
    {
        /* Make sure that eDel->Org and eDel->Rface point to valid half-edges */
        eDel->get_RFace()->set_Edge(eDel->get_OPrev());
        eDel->get_Org()->set_Edge(eDel->get_ONext());

        HalfEdge::Splice(eDel, eDel->get_OPrev());

        if (!isJoiningLoops)
        {
            Face *newFace= m_pTessellator->get_FaceAllocator()->Allocate();

            /* We are splitting one loop into two -- create a new loop for eDel. */
            newFace->MakeFace(eDel, eDel->get_LFace());
        }
    }

    /* Claim: the mesh is now in a consistent state, except that eDel->Org
     * may have been deleted.  Now we disconnect eDel->Dst.
     */
    if ( eDelSym->get_ONext() == eDelSym )
    {
        eDelSym->get_Org()->KillVertex(NULL, m_pTessellator->get_VertexAllocator());
        eDelSym->get_LFace()->KillFace(NULL, m_pTessellator->get_FaceAllocator());
    }
    else
    {
        /* Make sure that eDel->Dst and eDel->Lface point to valid half-edges */
        eDel->get_LFace()->set_Edge(eDelSym->get_OPrev());
        eDelSym->get_Org()->set_Edge(eDelSym->get_ONext());

        HalfEdge::Splice( eDelSym, eDelSym->get_OPrev());
    }

    /* Any isolated vertices or faces have already been freed. */
    eDel->KillEdge(m_pTessellator->get_HalfEdgeAllocator());

    return 1;
}


/******************** Other Edge Operations **********************/

/* All these routines can be implemented with the basic edge
 * operations above.  They are provided for convenience and efficiency.
 */


/* __gl_meshAddEdgeVertex( eOrg ) creates a new edge eNew such that
 * eNew == eOrg->Lnext, and eNew->Dst is a newly created vertex.
 * eOrg and eNew will have the same left face.
 */
HalfEdge *Mesh::AddEdgeVertex( HalfEdge *eOrg )
{
    HalfEdge *eNew = eOrg->MakeEdge(m_pTessellator->get_HalfEdgeAllocator());

    HalfEdge *eNewSym = eNew->get_Sym();

    /* Connect the new edge appropriately */
    HalfEdge::Splice( eNew, eOrg->get_LNext());

    /* Set the vertex and face information */
    eNew->set_Org(eOrg->get_Dst());

    Vertex *newVertex= m_pTessellator->get_VertexAllocator()->Allocate();
    if (newVertex == NULL) return NULL;

    newVertex->MakeVertex(eNewSym, eNew->get_Org());

    eNewSym->set_LFace(eOrg->get_LFace());
    eNew->set_LFace(eOrg->get_LFace());

    return eNew;
}


/* __gl_meshSplitEdge( eOrg ) splits eOrg into two edges eOrg and eNew,
 * such that eNew == eOrg->Lnext.  The new vertex is eOrg->Dst == eNew->Org.
 * eOrg and eNew will have the same left face.
 */
HalfEdge *Mesh::SplitEdge(HalfEdge *eOrg)
{
    HalfEdge *eNew;
    HalfEdge *tempHalfEdge= Mesh::AddEdgeVertex( eOrg );
    if (tempHalfEdge == NULL) return NULL;

    eNew = tempHalfEdge->get_Sym();

    /* Disconnect eOrg from eOrg->Dst and connect it to eNew->Org */
    HalfEdge::Splice( eOrg->get_Sym(), eOrg->get_Sym()->get_OPrev() );
    HalfEdge::Splice( eOrg->get_Sym(), eNew );

    /* Set the vertex and face information */
    eOrg->set_Dst(eNew->get_Org());
    eNew->get_Dst()->set_Edge(eNew->get_Sym());    /* may have pointed to eOrg->Sym */
    eNew->set_RFace(eOrg->get_RFace());
    eNew->set_Winding(eOrg->get_Winding());    /* copy old winding information */
    eNew->get_Sym()->set_Winding(eOrg->get_Sym()->get_Winding());

    return eNew;
}


/* __gl_meshConnect( eOrg, eDst ) creates a new edge from eOrg->Dst
 * to eDst->Org, and returns the corresponding half-edge eNew.
 * If eOrg->Lface == eDst->Lface, this splits one loop into two,
 * and the newly created loop is eNew->Lface.  Otherwise, two disjoint
 * loops are merged into one, and the loop eDst->Lface is destroyed.
 *
 * If (eOrg == eDst), the new face will have only two edges.
 * If (eOrg->Lnext == eDst), the old face is reduced to a single edge.
 * If (eOrg->Lnext->Lnext == eDst), the old face is reduced to two edges.
 */
HalfEdge *Mesh::Connect( HalfEdge *eOrg, HalfEdge *eDst )
{
    HalfEdge *eNewSym;
    bool isJoiningLoops = false;  
    HalfEdge *eNew = eOrg->MakeEdge(m_pTessellator->get_HalfEdgeAllocator());

    eNewSym = eNew->get_Sym();

    if ( eDst->get_LFace() != eOrg->get_LFace() )
    {
        /* We are connecting two disjoint loops -- destroy eDst->Lface */
        isJoiningLoops = true;
        eDst->get_LFace()->KillFace(eOrg->get_LFace(), m_pTessellator->get_FaceAllocator());
    }

    /* Connect the new edge appropriately */
    HalfEdge::Splice( eNew, eOrg->get_LNext());
    HalfEdge::Splice( eNewSym, eDst );

    /* Set the vertex and face information */
    eNew->set_Org(eOrg->get_Dst());
    eNewSym->set_Org(eDst->get_Org());
    eNew->set_LFace(eOrg->get_LFace());
    eNewSym->set_LFace(eOrg->get_LFace());

    /* Make sure the old face points to a valid half-edge */
    eOrg->get_LFace()->set_Edge(eNewSym);

    if (!isJoiningLoops)
    {
        Face *newFace= m_pTessellator->get_FaceAllocator()->Allocate();

        /* We split one loop into two -- the new loop is eNew->Lface */
        newFace->MakeFace(eNew, eOrg->get_LFace());
    }
    return eNew;
}


/******************** Other Operations **********************/

/* __gl_meshZapFace( fZap ) destroys a face and removes it from the
 * global face list.  All edges of fZap will have a NULL pointer as their
 * left face.  Any edges which also have a NULL pointer as their right face
 * are deleted entirely (along with any isolated vertices this produces).
 * An entire mesh can be deleted by zapping its faces, one at a time,
 * in any order.  Zapped faces cannot be used in further mesh operations!
 */
void Mesh::ZapFace(Face *fZap )
{
    HalfEdge *eStart = fZap->get_Edge();
    HalfEdge *e, *eNext, *eSym;
    Face *fPrev, *fNext;

    /* walk around face, deleting edges whose right face is also NULL */
    eNext = eStart->get_LNext();
    do
    {
        e = eNext;
        eNext = e->get_LNext();

        e->set_LFace(NULL);
        if ( e->get_RFace() == NULL )
        {
            /* delete the edge -- see __gl_MeshDelete above */

            if ( e->get_ONext() == e )
            {
                e->get_Org()->KillVertex(NULL, m_pTessellator->get_VertexAllocator());
            }
            else
            {
                /* Make sure that e->Org points to a valid half-edge */
                e->get_Org()->set_Edge(e->get_ONext());
                HalfEdge::Splice( e, e->get_OPrev());
            }
            eSym = e->get_Sym();
            if ( eSym->get_ONext() == eSym )
            {
                eSym->get_Org()->KillVertex(NULL, m_pTessellator->get_VertexAllocator());
            }
            else
            {
                /* Make sure that eSym->Org points to a valid half-edge */
                eSym->get_Org()->set_Edge(eSym->get_ONext());
                HalfEdge::Splice( eSym, eSym->get_OPrev());
            }

            e->KillEdge(m_pTessellator->get_HalfEdgeAllocator());
        }
    } while ( e != eStart );

    /* delete from circular doubly-linked list */
    fPrev = fZap->get_Prev();
    fNext = fZap->get_Next();
    fNext->set_Prev(fPrev);
    fPrev->set_Next(fNext);

    delete fZap;
}


/* __gl_meshNewMesh() creates a new mesh with no edges, no vertices,
 * and no loops (what we usually call a "face").
 */
Mesh::Mesh(Tessellator* pTessellator) : m_pTessellator(pTessellator)
{
    Vertex *v;
    Face *f;
    HalfEdge *e;
    HalfEdge *eSym;

    v = &m_vertexHead;
    f = &m_faceHead;
    e = &m_edgeHead;
    eSym = &m_edgeHeadSym;

    v->set_Next(v);
    v->set_Prev(v);
    v->set_Edge(NULL);

    f->set_Next(f);
    f->set_Prev(f);
    f->set_Edge(NULL);
    f->set_Trail(NULL);
    f->set_IsMarked(false);
    f->set_IsInside(false);

    e->set_Next(e);
    e->set_Sym(eSym);
    e->set_ONext(NULL);
    e->set_LNext(NULL);
    e->set_Org(NULL);
    e->set_LFace(NULL);
    e->set_Winding(0);
    e->set_ActiveRegion(NULL);
    e->set_IsMarked(false);

    eSym->set_Next(eSym);
    eSym->set_Sym(e);
    eSym->set_ONext(NULL);
    eSym->set_LNext(NULL);
    eSym->set_Org(NULL);
    eSym->set_LFace(NULL);
    eSym->set_Winding(0);
    eSym->set_ActiveRegion(NULL);
    eSym->set_IsMarked(false);

}


/* __gl_meshUnion( mesh1, mesh2 ) forms the union of all structures in
 * both meshes, and returns the new mesh (the old meshes are destroyed).
 */
void Mesh::Union( Mesh *mesh )
{
    Face *f1 = &m_faceHead;
    Vertex *v1 = &m_vertexHead;
    HalfEdge *e1 = &m_edgeHead;
    Face *f2 = &mesh->m_faceHead;
    Vertex *v2 = &mesh->m_vertexHead;
    HalfEdge *e2 = &mesh->m_edgeHead;

    /* Add the faces, vertices, and edges of mesh2 to those of mesh1 */
    if ( f2->get_Next() != f2 )
    {
        f1->get_Prev()->set_Next(f2->get_Next());
        f2->get_Next()->set_Prev(f1->get_Prev());
        f2->get_Prev()->set_Next(f1);
        f1->set_Prev(f2->get_Prev());
    }

    if ( v2->get_Next() != v2 )
    {
        v1->get_Prev()->set_Next(v2->get_Next());
        v2->get_Next()->set_Prev(v1->get_Prev());
        v2->get_Prev()->set_Next(v1);
        v1->set_Prev(v2->get_Prev());
    }

    if ( e2->get_Next() != e2 )
    {
        e1->get_Sym()->get_Next()->get_Sym()->set_Next(e2->get_Next());
        e2->get_Next()->get_Sym()->set_Next(e1->get_Sym()->get_Next());
        e2->get_Sym()->get_Next()->get_Sym()->set_Next(e1);
        e1->get_Sym()->set_Next(e2->get_Sym()->get_Next());
    }

    f2->set_Next(f2);
    f2->set_Prev(f2);

    v2->set_Next(v2);
    v2->set_Prev(v2);

    e2->set_Next(e2);

    delete mesh;
}


/* __gl_meshDeleteMesh( mesh ) will free all storage for any valid mesh.
 */
Mesh::~Mesh()
{
    Face *f, *fNext;
    Vertex *v, *vNext;
    HalfEdge *e, *eNext;

    for ( f = m_faceHead.get_Next(); f != &m_faceHead; f = fNext )
    {
        fNext = f->get_Next();
        m_pTessellator->get_FaceAllocator()->Free(f);
    }

    for ( v = m_vertexHead.get_Next(); v != &m_vertexHead; v = vNext )
    {
        vNext = v->get_Next();
        m_pTessellator->get_VertexAllocator()->Free(v);
    }

    for ( e = m_edgeHead.get_Next(); e != &m_edgeHead; e = eNext )
    {
        eNext = e->get_Next();

        m_pTessellator->get_HalfEdgeAllocator()->Free(e->get_Sym());
        m_pTessellator->get_HalfEdgeAllocator()->Free(e);
    }
}


/*
 * Delete any degenerate faces with only two edges.  WalkDirtyRegions()
 * will catch almost all of these, but it won't catch degenerate faces
 * produced by splice operations on already-processed edges.
 * The two places this can happen are in FinishLeftRegions(), when
 * we splice in a "temporary" edge produced by ConnectRightVertex(),
 * and in CheckForLeftSplice(), where we splice already-processed
 * edges to ensure that our dictionary invariants are not violated
 * by numerical errors.
 *
 * In both these cases it is *very* dangerous to delete the offending
 * edge at the time, since one of the routines further up the stack
 * will sometimes be keeping a pointer to that edge.
 */
int Mesh::RemoveDegenerateFaces()
{
    Face *f, *fNext;
    HalfEdge *e;

    for ( f = m_faceHead.get_Next(); f != &m_faceHead; f = fNext )
    {
        fNext = f->get_Next();
        e = f->get_Edge();

        assert( e->get_LNext() != e );

        if ( e->get_LNext()->get_LNext() == e )
        {
            /* A face with only two edges */

            e->get_ONext()->AddWinding(e);

            if (!Delete(e))
            {
                return 0;
            }
        }
    }
    return 1;
}

/* __gl_meshset_WindingNumber( mesh, value, keepOnlyBoundary ) resets the
 * winding numbers on all edges so that regions marked "inside" the
 * polygon have a winding number of "value", and regions outside
 * have a winding number of 0.
 *
 * If keepOnlyBoundary is TRUE, it also deletes all edges which do not
 * separate an interior region from an exterior one.
 */
int Mesh::set_WindingNumber(int value, bool keepOnlyBoundary )
{
    HalfEdge *e, *eNext;

    for ( e = m_edgeHead.get_Next(); e != &m_edgeHead; e = eNext )
    {
        eNext = e->get_Next();
        if (e->get_RFace()->get_IsInside() != e->get_LFace()->get_IsInside())
        {

            /* This is a boundary edge (one side is interior, one is exterior). */
            e->set_Winding((e->get_LFace()->get_IsInside()) ? value : -value);
        }
        else
        {

            /* Both regions are interior, or both are exterior. */
            if ( !keepOnlyBoundary )
            {
                e->set_Winding(0);
            }
            else
            {
                Delete(e);
            }
        }
    }
    return 1;
}

/* __gl_meshTessellateInterior( mesh ) tessellates each region of
 * the mesh which is marked "inside" the polygon.  Each such region
 * must be monotone.
 */
int Mesh::TessellateInterior()
{
    Face *f, *next;

    for ( f = m_faceHead.get_Next(); f != &m_faceHead; f = next )
    {
        /* Make sure we don''t try to tessellate the new triangles. */
        next = f->get_Next();
        if ( f->get_IsInside() )
        {
            f->TessellateMonoRegions(this);
        }
    }

  return 1;
}

void Face::TessellateMonoRegions(Mesh* pMesh)
{
    HalfEdge *up, *lo;

    /* All edges are oriented CCW around the boundary of the region.
     * First, find the half-edge whose origin vertex is rightmost.
     * Since the sweep goes from left to right, face->anEdge should
     * be close to the edge we want.
     */
    up = get_Edge();
    assert( up->get_LNext() != up && up->get_LNext()->get_LNext() != up );

    for ( ; Vertex::IsLessThanOrEqualTo(up->get_Dst(), up->get_Org() ); up = up->get_LPrev() )
        ;

    for ( ; Vertex::IsLessThanOrEqualTo( up->get_Org(), up->get_Dst() ); up = up->get_LNext() )
        ;

    lo = up->get_LPrev();

    while ( up->get_LNext() != lo )
    {
        if ( Vertex::IsLessThanOrEqualTo( up->get_Dst(), lo->get_Org() ))
        {
            /* up->Dst is on the left.  It is safe to form triangles from lo->Org.
             * The EdgeGoesLeft test guarantees progress even when some triangles
             * are CW, given that the upper and lower chains are truly monotone.
             */
            while ( lo->get_LNext() != up && (lo->get_LNext()->GoesLeft() || Vertex::EdgeSign( lo->get_Org(), lo->get_Dst(), lo->get_LNext()->get_Dst() ) <= 0 ))
            {
                HalfEdge *tempHalfEdge= pMesh->Connect(lo->get_LNext(), lo);
                lo = tempHalfEdge->get_Sym();
            }
            lo = lo->get_LPrev();
        }
        else
        {
            /* lo->Org is on the left.  We can make CCW triangles from up->Dst. */
            while (lo->get_LNext() != up && (up->get_LPrev()->GoesRight() || Vertex::EdgeSign( up->get_Dst(), up->get_Org(), up->get_LPrev()->get_Org() ) >= 0 ))
            {
                HalfEdge *tempHalfEdge= pMesh->Connect(up, up->get_LPrev());
                up = tempHalfEdge->get_Sym();
            }
            up = up->get_LNext();
        }
    }

    /* Now lo->Org == up->Dst == the leftmost vertex.  The remaining region
     * can be tessellated in a fan from this leftmost vertex.
     */
    assert( lo->get_LNext() != up );
    while ( lo->get_LNext()->get_LNext() != up )
    {
        HalfEdge *tempHalfEdge= pMesh->Connect(lo->get_LNext(), lo );
        lo = tempHalfEdge->get_Sym();
    }
}



/* __gl_meshCheckMesh( mesh ) checks a mesh for self-consistency.
 */
void Mesh::Check()
{
    Face *fHead = &m_faceHead;
    Vertex *vHead = &m_vertexHead;
    HalfEdge *eHead = &m_edgeHead;
    Face *f, *fPrev;
    Vertex *v, *vPrev;
    HalfEdge *e, *ePrev;

    fPrev = fHead;
    for ( fPrev = fHead ; (f = fPrev->get_Next()) != fHead; fPrev = f)
    {
        assert( f->get_Prev() == fPrev );
        e = f->get_Edge();
        do
        {
            assert( e->get_Sym() != e );
            assert( e->get_Sym()->get_Sym() == e );
            assert( e->get_LNext()->get_ONext()->get_Sym() == e );
            assert( e->get_ONext()->get_Sym()->get_LNext() == e );
            assert( e->get_LFace() == f );
            e = e->get_LNext();
        } while ( e != f->get_Edge() );
    }
    assert( f->get_Prev() == fPrev && f->get_Edge() == NULL );

    vPrev = vHead;
    for ( vPrev = vHead ; (v = vPrev->get_Next()) != vHead; vPrev = v)
    {
        assert( v->get_Prev() == vPrev );
        e = v->get_Edge();
        do
        {
            assert( e->get_Sym() != e );
            assert( e->get_Sym()->get_Sym() == e );
            assert( e->get_LNext()->get_ONext()->get_Sym() == e );
            assert( e->get_ONext()->get_Sym()->get_LNext() == e );
            assert( e->get_Org() == v );
            e = e->get_ONext();
        } while ( e != v->get_Edge());
    }
    assert( v->get_Prev() == vPrev && v->get_Edge() == NULL );

    ePrev = eHead;
    for ( ePrev = eHead ; (e = ePrev->get_Next()) != eHead; ePrev = e)
    {
        assert( e->get_Sym()->get_Next() == ePrev->get_Sym() );
        assert( e->get_Sym() != e );
        assert( e->get_Sym()->get_Sym() == e );
        assert( e->get_Org() != NULL );
        assert( e->get_Dst() != NULL );
        assert( e->get_LNext()->get_ONext()->get_Sym() == e );
        assert( e->get_ONext()->get_Sym()->get_LNext() == e );
    }
    assert( e->get_Sym()->get_Next() == ePrev->get_Sym()
            && e->get_Sym() == &m_edgeHeadSym
            && e->get_Sym()->get_Sym() == e
            && e->get_Org() == NULL && e->get_Dst() == NULL
            && e->get_LFace() == NULL && e->get_RFace() == NULL );
}



Float Vertex::EdgeEval(Vertex* u, Vertex* v, Vertex* w)
{
/* Given three vertices u,v,w such that VertLeq(u,v) && VertLeq(v,w),
 * evaluates the t-coord of the edge uw at the s-coord of the vertex v.
 * Returns v->t - (uw)(v->s), ie. the signed distance from uw to v.
 * If uw is vertical (and thus passes thru v), the result is zero.
 *
 * The calculation is extremely accurate and stable, even when v
 * is very close to u or w.  In particular if we set v->t = 0 and
 * let r be the negated result (this evaluates (uw)(v->s)), then
 * r is guaranteed to satisfy MIN(u->t,w->t) <= r <= MAX(u->t,w->t).
 */
    Float gapL, gapR;

    assert(Vertex::IsLessThanOrEqualTo(u, v) && Vertex::IsLessThanOrEqualTo(v, w));

    gapL = v->get_X() - u->get_X();
    gapR = w->get_X() - v->get_X();

    if ( gapL + gapR > 0 )
    {
        if ( gapL < gapR )
        {
            return(v->get_Y() - u->get_Y()) + (u->get_Y() - w->get_Y()) * (gapL / (gapL + gapR));
        }
        else
        {
            return(v->get_Y() - w->get_Y()) + (w->get_Y() - u->get_Y()) * (gapR / (gapL + gapR));
        }
    }
/* vertical line */
    return 0.0;
}

Float Vertex::EdgeSign(Vertex* u, Vertex* v, Vertex* w)
{
    /* Returns a number whose sign matches EdgeEval(u,v,w) but which
     * is cheaper to evaluate.  Returns > 0, == 0 , or < 0
     * as v is above, on, or below the edge uw.
     */
    Float gapL, gapR;
    
    assert(Vertex::IsLessThanOrEqualTo(u, v) && Vertex::IsLessThanOrEqualTo(v, w));
    
    gapL = v->get_X() - u->get_X();
    gapR = w->get_X() - v->get_X();
    
    if ( gapL + gapR > 0 )
    {
        return(v->get_Y() - w->get_Y()) * gapL + (v->get_Y() - u->get_Y()) * gapR;
    }
    /* vertical line */
    return 0;    
}

Float Vertex::EdgeTransEval(Vertex* u, Vertex* v, Vertex* w)
{
    Float gapL, gapR;

    gapL = v->get_Y() - u->get_Y();
    gapR = w->get_Y() - v->get_Y();

    if ( gapL + gapR > 0 )
    {
        if ( gapL < gapR )
        {
            return(v->get_X() - u->get_X()) + (u->get_X() - w->get_X()) * (gapL / (gapL + gapR));
        }
        else
        {
            return(v->get_X() - w->get_X()) + (w->get_X() - u->get_X()) * (gapR / (gapL + gapR));
        }
    }
/* vertical line */
    return 0.0;
}

Float Vertex::EdgeTransSign(Vertex* u, Vertex* v, Vertex* w)
{
    Float gapL, gapR;

    gapL = v->get_Y() - u->get_Y();
    gapR = w->get_Y() - v->get_Y();

    if ( gapL + gapR > 0 )
    {
        return(v->get_X() - w->get_X()) * gapL + (v->get_X() - u->get_X()) * gapR;
    }
    /* vertical line */
    return 0;    
}

bool Vertex::VertCCW(Vertex* u, Vertex* v, Vertex* w)
{
    /* For almost-degenerate situations, the results are not reliable.
     * Unless the floating-point arithmetic can be performed without
     * rounding errors, *any* implementation will give incorrect results
     * on some degenerate inputs, so the client must have some way to
     * handle this situation.
     */
    return (u->get_X()*(v->get_Y() - w->get_Y()) + v->get_X()*(w->get_Y() - u->get_Y()) + w->get_X()*(u->get_Y() - v->get_Y())) >= 0;
}

/* Given parameters a,x,b,y returns the value (b*x+a*y)/(a+b),
 * or (x+y)/2 if a==b==0.  It requires that a,b >= 0, and enforces
 * this in the rare case that one argument is slightly negative.
 * The implementation is extremely stable numerically.
 * In particular it guarantees that the result r satisfies
 * MIN(x,y) <= r <= MAX(x,y), and the results are very accurate
 * even when a and b differ greatly in magnitude.
 */
#define Interpolate(a,x,b,y)			\
  (a = (a < 0) ? 0 : a, b = (b < 0) ? 0 : b,		\
  ((a <= b) ? ((b == 0) ? ((x+y) / 2)			\
                        : (x + (y-x) * (a/(a+b))))	\
            : (y + (x-y) * (b/(a+b)))))

#define Swap(a,b) { Vertex *t = a; a = b; b = t; }


/* Given edges (o1,d1) and (o2,d2), compute their point of intersection.
 * The computed point is guaranteed to lie in the intersection of the
 * bounding rectangles defined by each edge.
 */
void Vertex::EdgeIntersect(Vertex* o1, Vertex* d1, Vertex* o2, Vertex* d2, Vertex* v)
{
    Float z1, z2;

    /* This is certainly not the most efficient way to find the intersection
     * of two line segments, but it is very numerically stable.
     *
     * Strategy: find the two middle vertices in the VertLeq ordering,
     * and interpolate the intersection s-value from these.  Then repeat
     * using the TransLeq ordering to find the intersection t-value.
     */

    if(!Vertex::IsLessThanOrEqualTo(o1, d1))
    {
        Swap( o1, d1 );
    }
    if(!Vertex::IsLessThanOrEqualTo(o2, d2))
    {
        Swap( o2, d2 );
    }
    if(!Vertex::IsLessThanOrEqualTo(o1, o2))
    {
        Swap( o1, o2 ); 
        Swap( d1, d2 );
    }

    if(!Vertex::IsLessThanOrEqualTo(o2, d1))
    {
        /* Technically, no intersection -- do our best */
        v->set_X((o2->get_X() + d1->get_X()) / Float(2.0));
    }
    else if ( Vertex::IsLessThanOrEqualTo( d1, d2 ))
    {
        /* Interpolate between o2 and d1 */
        z1 = EdgeEval( o1, o2, d1 );
        z2 = EdgeEval( o2, d1, d2 );
        if ( z1+z2 < 0 )
        {
            z1 = -z1; z2 = -z2;
        }
        v->set_X(Interpolate( z1, o2->get_X(), z2, d1->get_X()));
    }
    else
    {
        /* Interpolate between o2 and d2 */
        z1 = EdgeSign( o1, o2, d1 );
        z2 = -EdgeSign( o1, d2, d1 );
        if ( z1+z2 < 0 )
        {
            z1 = -z1; z2 = -z2;
        }
        v->set_X(Interpolate( z1, o2->get_X(), z2, d2->get_X()));
    }

    /* Now repeat the process for t */

    if (!Vertex::IsLessThanOrEqualToTrans( o1, d1 ))
    {
        Swap( o1, d1 );
    }
    if (!Vertex::IsLessThanOrEqualToTrans( o2, d2 ))
    {
        Swap( o2, d2 );
    }
    if (!Vertex::IsLessThanOrEqualToTrans( o1, o2 ))
    {
        Swap( o1, o2 ); 
        Swap( d1, d2 );
    }

    if (!Vertex::IsLessThanOrEqualToTrans( o2, d1 ))
    {
        /* Technically, no intersection -- do our best */
        v->set_Y((o2->get_Y() + d1->get_Y()) / 2);
    }
    else if (Vertex::IsLessThanOrEqualToTrans( d1, d2 ))
    {
        /* Interpolate between o2 and d1 */
        z1 = EdgeTransEval( o1, o2, d1 );
        z2 = EdgeTransEval( o2, d1, d2 );
        if ( z1+z2 < 0 )
        {
            z1 = -z1; z2 = -z2;
        }
        v->set_Y(Interpolate(z1, o2->get_Y(), z2, d1->get_Y()));
    }
    else
    {
        /* Interpolate between o2 and d2 */
        z1 = EdgeTransSign( o1, o2, d1 );
        z2 = -EdgeTransSign( o1, d2, d1 );
        if ( z1+z2 < 0 )
        {
            z1 = -z1; z2 = -z2;
        }
        v->set_Y(Interpolate( z1, o2->get_Y(), z2, d2->get_Y()));
    }

}


} // namespace tess
