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
** $Header: /cvs/mesa/Mesa/src/glu/sgi/libtess/mesh.h,v 1.1 2001/03/17 00:25:41 brianp Exp $
*/

#ifndef __mesh_h_
#define __mesh_h_

#include <tesscommon.h>

namespace Tess
{

/* The mesh structure is similar in spirit, notation, and operations
 * to the "quad-edge" structure (see L. Guibas and J. Stolfi, Primitives
 * for the manipulation of general subdivisions and the computation of
 * Voronoi diagrams, ACM Transactions on Graphics, 4(2):74-123, April 1985).
 * For a simplified description, see the course notes for CS348a,
 * "Mathematical Foundations of Computer Graphics", available at the
 * Stanford bookstore (and taught during the fall quarter).
 * The implementation also borrows a tiny subset of the graph-based approach
 * use in Mantyla's Geometric Work Bench (see M. Mantyla, An Introduction
 * to Sold Modeling, Computer Science Press, Rockville, Maryland, 1988).
 *
 * The fundamental data structure is the "half-edge".  Two half-edges
 * go together to make an edge, but they point in opposite directions.
 * Each half-edge has a pointer to its mate (the "symmetric" half-edge Sym),
 * its origin vertex (Org), the face on its left side (Lface), and the
 * adjacent half-edges in the CCW direction around the origin vertex
 * (Onext) and around the left face (Lnext).  There is also a "next"
 * pointer for the global edge list (see below).
 *
 * The notation used for mesh navigation:
 *	Sym   = the mate of a half-edge (same edge, but opposite direction)
 *	Onext = edge CCW around origin vertex (keep same origin)
 *	Dnext = edge CCW around destination vertex (keep same dest)
 *	Lnext = edge CCW around left face (dest becomes new origin)
 *	Rnext = edge CCW around right face (origin becomes new dest)
 *
 * "prev" means to substitute CW for CCW in the definitions above.
 *
 * The mesh keeps global lists of all vertices, faces, and edges,
 * stored as doubly-linked circular lists with a dummy header node.
 * The mesh stores pointers to these dummy headers (vHead, fHead, eHead).
 *
 * The circular edge list is special; since half-edges always occur
 * in pairs (e and e->Sym), each half-edge stores a pointer in only
 * one direction.  Starting at eHead and following the e->next pointers
 * will visit each *edge* once (ie. e or e->Sym, but not both).
 * e->Sym stores a pointer in the opposite direction, thus it is
 * always true that e->Sym->next->Sym->next == e.
 *
 * Each vertex has a pointer to next and previous vertices in the
 * circular list, and a pointer to a half-edge with this vertex as
 * the origin (NULL if this is the dummy header).  There is also a
 * field "data" for client data.
 *
 * Each face has a pointer to the next and previous faces in the
 * circular list, and a pointer to a half-edge with this face as
 * the left face (NULL if this is the dummy header).  There is also
 * a field "data" for client data.
 *
 * Note that what we call a "face" is really a loop; faces may consist
 * of more than one loop (ie. not simply connected), but there is no
 * record of this in the data structure.  The mesh may consist of
 * several disconnected regions, so it may not be possible to visit
 * the entire mesh by starting at a half-edge and traversing the edge
 * structure.
 *
 * The mesh does NOT support isolated vertices; a vertex is deleted along
 * with its last edge.  Similarly when two faces are merged, one of the
 * faces is deleted (see __gl_meshDelete below).  For mesh operations,
 * all face (loop) and vertex pointers must not be NULL.  However, once
 * mesh manipulation is finished, __gl_MeshZapFace can be used to delete
 * faces of the mesh, one at a time.  All external faces can be "zapped"
 * before the mesh is returned to the client; then a NULL face indicates
 * a region which is not part of the output polygon.
 */

class Mesh;
class HalfEdge;
class ActiveRegion;
class Face;
class DictNode;



class Vertex
{
public:

    Float get_X() { return m_x; }
    void set_X(Float newX) { m_x = newX; }

    Float get_Y() { return m_y; }
    void set_Y(Float newY) { m_y = newY; }

    Vertex* get_Next() { return m_pNext; }
    void set_Next(Vertex* pNext) { m_pNext = pNext; }

    Vertex* get_Prev() { return m_pPrev; }
    void set_Prev(Vertex* pPrev) { m_pPrev = pPrev; }

    HalfEdge* get_Edge() { return m_pEdge; }
    void  set_Edge(HalfEdge* pEdge) { m_pEdge = pEdge; }

    void MakeVertex(HalfEdge *eOrig, Vertex *vNext);
    void KillVertex(Vertex *newOrg, VertexAllocator* pAllocator);

    int get_PQHandle() { return m_pqHandle; }
    void set_PQHandle(int pqHandle) { m_pqHandle = pqHandle; }

    int get_Index() { return m_index; }
    void set_Index(int index) { m_index = index; }

    static bool Equals(Vertex* pV1, Vertex* pV2) { return pV1->get_X() == pV2->get_X() && pV1->get_Y() == pV2->get_Y(); }

    static bool IsLessThanOrEqualTo(Vertex* pV1, Vertex* pV2) { return ((pV1->get_X() < pV2->get_X()) || (pV1->get_X() == pV2->get_X() &&  pV1->get_Y() <= pV2->get_Y())); };
    static bool IsLessThanOrEqualToTrans(Vertex* pV1, Vertex* pV2) { return ((pV1->get_Y() < pV2->get_Y()) || (pV1->get_Y() == pV2->get_Y() &&  pV1->get_X() <= pV2->get_X())); };
    static bool IsLessThanOrEqualToCallback(void* pV1, void* pV2) { return IsLessThanOrEqualTo((Vertex*) pV1, (Vertex*) pV2); }

    static Float EdgeEval(Vertex* u, Vertex* v, Vertex* w);
    static Float EdgeSign(Vertex* u, Vertex* v, Vertex* w);

    static Float EdgeTransEval(Vertex* u, Vertex* v, Vertex* w);
    static Float EdgeTransSign(Vertex* u, Vertex* v, Vertex* w);

    static bool VertCCW(Vertex* u, Vertex* v, Vertex* w);
    static void EdgeIntersect(Vertex* o1, Vertex* d1, Vertex* o2, Vertex* d2, Vertex* v);

private:
    Vertex* m_pNext;
    Vertex* m_pPrev;
    HalfEdge* m_pEdge;

    Float m_x, m_y;
    int m_pqHandle;
    int m_index;
};

class Face
{
public:

    Face* get_Next() { return m_pNext; }
    void set_Next(Face* pNext) { m_pNext = pNext; }

    Face* get_Prev() { return m_pPrev; }
    void set_Prev(Face* pPrev) { m_pPrev = pPrev; }

    HalfEdge* get_Edge() { return m_pEdge; }
    void set_Edge(HalfEdge* pEdge) { m_pEdge = pEdge; }

    void MakeFace(HalfEdge *eOrig, Face *fNext);
    void KillFace( Face *newLface, FaceAllocator* pAllocator);


    void set_Trail(Face* pTrail) { m_pTrail = pTrail; }

    void set_IsMarked(bool isMarked) { m_isMarked = isMarked; }


    bool get_IsInside() { return m_isInside; }
    void set_IsInside(bool isInside) { m_isInside = isInside; }

    void TessellateMonoRegions(Mesh* pMesh);

private:

    Face* m_pNext;       /* next face (never NULL) */
    Face* m_pPrev;       /* previous face (never NULL) */
    HalfEdge* m_pEdge;   /* a half edge with this left face */

    /* Internal data (keep hidden) */
    Face* m_pTrail; /* "stack" for conversion to strips */
    bool m_isMarked; /* flag for conversion to strips */
    bool m_isInside; /* this face is in the polygon interior */
};


class HalfEdge
{
public:
    HalfEdge* get_Next() { return m_pNext; }
    void set_Next(HalfEdge* pNext) { m_pNext = pNext; }

    HalfEdge* get_Sym() { return m_pSym; }
    void set_Sym(HalfEdge* pSym) { m_pSym = pSym; }

    HalfEdge* get_ONext() { return m_pONext; }
    void set_ONext(HalfEdge* pONext) { m_pONext = pONext; }

    HalfEdge* get_OPrev() { return get_Sym()->get_LNext(); }

    HalfEdge* get_LNext() { return m_pLNext; }
    void set_LNext(HalfEdge* pLNext) { m_pLNext = pLNext; }

    HalfEdge* get_LPrev() { return get_ONext()->get_Sym(); }

    HalfEdge* get_RNext() { return get_OPrev()->get_Sym(); }
    HalfEdge* get_RPrev() { return get_Sym()->get_ONext(); }

    HalfEdge* get_DNext() { return get_RPrev()->get_Sym(); }
    HalfEdge* get_DPrev() { return get_LNext()->get_Sym(); }

    Vertex* get_Org() { return m_pOrg; }
    void set_Org(Vertex* pOrg) { m_pOrg = pOrg; }

    Face* get_LFace() { return m_pLFace; }
    void set_LFace(Face* pLFace) { m_pLFace = pLFace; }

    Face* get_RFace() { return get_Sym()->get_LFace(); }
    void set_RFace(Face* pRFace) { get_Sym()->m_pLFace = pRFace; }

    Vertex* get_Dst() { return get_Sym()->get_Org(); }
    void set_Dst(Vertex* pDst) { get_Sym()->m_pOrg = pDst; }

    ActiveRegion* get_ActiveRegion() { return m_pActiveRegion; }
    void set_ActiveRegion(ActiveRegion* pActiveRegion) { m_pActiveRegion = pActiveRegion; }

    int get_Winding() { return m_winding; }
    void set_Winding(int winding) { m_winding = winding; }

    bool get_IsMarked() { return m_isMarked; }
    bool set_IsMarked(bool isMarked) { return m_isMarked = isMarked; }

    void AddWinding(HalfEdge* eSrc)
    {
        m_winding += eSrc->m_winding;
        get_Sym()->m_winding += eSrc->get_Sym()->get_Winding();
    }

    bool GoesLeft() { return Vertex::IsLessThanOrEqualTo(get_Dst(), get_Org()); }
    bool GoesRight() { return Vertex::IsLessThanOrEqualTo(get_Org(), get_Dst()); }

    HalfEdge *MakeEdge(HalfEdgeAllocator* pAllocator);
    void KillEdge(HalfEdgeAllocator* pAllocator);

    static void Splice(HalfEdge* pOrg, HalfEdge* pDst);

private:

    HalfEdge* m_pNext; /* doubly-linked list (prev==Sym->next) */
    HalfEdge* m_pSym; /* same edge, opposite direction */
    HalfEdge* m_pONext; /* next edge CCW around origin */
    HalfEdge* m_pLNext; /* next edge CCW around left face */
    Vertex* m_pOrg; /* origin vertex (Overtex too long) */
    Face* m_pLFace; /* left face */


    ActiveRegion	*m_pActiveRegion;	/* a region with this upper edge (sweep.c) */
    int m_winding;	/* change in winding number when crossing
                                     from the right face to the left face */
    bool m_isMarked;
};


/* For each pair of adjacent edges crossing the sweep line, there is
 * an ActiveRegion to represent the region between them.  The active
 * regions are kept in sorted order in a dynamic dictionary.  As the
 * sweep line crosses each vertex, we update the affected regions.
 */

class ActiveRegion 
{
public:
    ActiveRegion() { };
    ~ActiveRegion() { };

    ActiveRegion* get_RegionBelow();
    ActiveRegion* get_RegionAbove();
    ActiveRegion* get_TopLeftRegion(Mesh* pMesh);
    ActiveRegion* get_TopRightRegion();

    HalfEdge* get_EUp() { return m_eUp;}
    void set_EUp(HalfEdge* pEdge) { m_eUp = pEdge; }

    DictNode* get_NodeUp() { return m_nodeUp;}
    void set_NodeUp(DictNode* pNode) { m_nodeUp = pNode; }

    int get_WindingNumber() { return m_windingNumber;}
    void set_WindingNumber(int windingNumber) { m_windingNumber = windingNumber; }

    bool get_IsInside() { return m_isInside;}
    void set_IsInside(bool isInside) { m_isInside = isInside; }

    bool get_IsSentinel() { return m_isSentinel;}
    void set_IsSentinel(bool isSentinel) { m_isSentinel = isSentinel; }

    bool get_IsDirty() { return m_isDirty;}
    void set_IsDirty(bool isDirty) { m_isDirty = isDirty; }

    bool get_FixUpperEdge() { return m_fixUpperEdge;}
    void set_FixUpperEdge(bool fixUpperEdge) { m_fixUpperEdge = fixUpperEdge; }

    int FixUpperEdge(HalfEdge* pNewEdge, Mesh* pMesh);

private:

    HalfEdge  *m_eUp;     /* upper edge, directed right to left */
    DictNode  *m_nodeUp;  /* dictionary node corresponding to eUp */
    int       m_windingNumber;    /* used to determine which regions are
                                   * inside the polygon */
    bool m_isInside;      /* is this region inside the polygon? */
    bool m_isSentinel;    /* marks fake edges at t = +/-infinity */
    bool m_isDirty;       /* marks regions where the upper or lower
                                   * edge has changed, but we haven't checked
                                   * whether they intersect yet */
    bool m_fixUpperEdge;  /* marks temporary edges introduced when
                                   * we process a "right vertex" (one without
                                   * any edges leaving to the right) */
};



/* The mesh operations below have three motivations: completeness,
 * convenience, and efficiency.  The basic mesh operations are MakeEdge,
 * Splice, and Delete.  All the other edge operations can be implemented
 * in terms of these.  The other operations are provided for convenience
 * and/or efficiency.
 *
 * When a face is split or a vertex is added, they are inserted into the
 * global list *before* the existing vertex or face (ie. e->Org or e->Lface).
 * This makes it easier to process all vertices or faces in the global lists
 * without worrying about processing the same data twice.  As a convenience,
 * when a face is split, the "inside" flag is copied from the old face.
 * Other internal data (v->data, v->activeRegion, f->data, f->marked,
 * f->trail, e->winding) is set to zero.
 *
 * ********************** Basic Edge Operations **************************
 *
 * __gl_meshMakeEdge( mesh ) creates one edge, two vertices, and a loop.
 * The loop (face) consists of the two new half-edges.
 *
 * __gl_meshSplice( eOrg, eDst ) is the basic operation for changing the
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
 * __gl_meshDelete( eDel ) removes the edge eDel.  There are several cases:
 * if (eDel->Lface != eDel->Rface), we join two loops into one; the loop
 * eDel->Lface is deleted.  Otherwise, we are splitting one loop into two;
 * the newly created loop will contain eDel->Dst.  If the deletion of eDel
 * would create isolated vertices, those are deleted as well.
 *
 * ********************** Other Edge Operations **************************
 *
 * __gl_meshAddEdgeVertex( eOrg ) creates a new edge eNew such that
 * eNew == eOrg->Lnext, and eNew->Dst is a newly created vertex.
 * eOrg and eNew will have the same left face.
 *
 * __gl_meshSplitEdge( eOrg ) splits eOrg into two edges eOrg and eNew,
 * such that eNew == eOrg->Lnext.  The new vertex is eOrg->Dst == eNew->Org.
 * eOrg and eNew will have the same left face.
 *
 * __gl_meshConnect( eOrg, eDst ) creates a new edge from eOrg->Dst
 * to eDst->Org, and returns the corresponding half-edge eNew.
 * If eOrg->Lface == eDst->Lface, this splits one loop into two,
 * and the newly created loop is eNew->Lface.  Otherwise, two disjoint
 * loops are merged into one, and the loop eDst->Lface is destroyed.
 *
 * ************************ Other Operations *****************************
 *
 * __gl_meshNewMesh() creates a new mesh with no edges, no vertices,
 * and no loops (what we usually call a "face").
 *
 * __gl_meshUnion( mesh1, mesh2 ) forms the union of all structures in
 * both meshes, and returns the new mesh (the old meshes are destroyed).
 *
 * __gl_meshDeleteMesh( mesh ) will free all storage for any valid mesh.
 *
 * __gl_meshZapFace( fZap ) destroys a face and removes it from the
 * global face list.  All edges of fZap will have a NULL pointer as their
 * left face.  Any edges which also have a NULL pointer as their right face
 * are deleted entirely (along with any isolated vertices this produces).
 * An entire mesh can be deleted by zapping its faces, one at a time,
 * in any order.  Zapped faces cannot be used in further mesh operations!
 *
 * __gl_meshCheckMesh( mesh ) checks a mesh for self-consistency.
 */

class Mesh
{
public:
    Mesh(Tessellator* pTessellator);
    ~Mesh();

    HalfEdge* MakeEdge();
    void Check();

    void Union(Mesh* mesh);

    int set_WindingNumber(int windingNumber, bool keepOnlyBoundary);
    int TessellateInterior();
    int RemoveDegenerateFaces();

    int MeshSplice(HalfEdge* pOrg, HalfEdge* pDst);
    int Delete(HalfEdge* pDel);

    HalfEdge* AddEdgeVertex(HalfEdge* pOrg);
    HalfEdge* SplitEdge(HalfEdge* pOrg);
    HalfEdge* Connect(HalfEdge* pOrg, HalfEdge* pDst);
    void ZapFace(Face* pZap);

    Vertex* get_VHead() { return &m_vertexHead; }
    HalfEdge* get_EHead() { return &m_edgeHead; }
    Face* get_FHead() { return &m_faceHead; }

private:


    Tessellator* m_pTessellator; // Tessellator back pointer

    Vertex m_vertexHead;    /* dummy header for vertex list */
    Face m_faceHead;        /* dummy header for face list */
    HalfEdge m_edgeHead;    /* dummy header for edge list */
    HalfEdge m_edgeHeadSym; /* and its symmetric counterpart */

};

} // Namespace Tess

#endif
