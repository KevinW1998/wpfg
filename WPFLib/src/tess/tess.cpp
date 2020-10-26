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
*/

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "tess.h"
#include "mesh.h"
#include "dict.h"
#include "PriorityQ.h"

namespace Tess
{

const Float Tessellator::c_maxCoord = Float(1.0e32);
const Float Tessellator::c_sentinelCoord = Float(1.0e32 * 4.0);

#undef	MAX
#undef	MIN
#define MAX(x,y)	((x) >= (y) ? (x) : (y))
#define MIN(x,y)	((x) <= (y) ? (x) : (y))

#pragma warning(disable:4100)
class ClientInterface : public IClientTessellatorInterface
{
public:
    virtual void EndVertexData(void) { }
    virtual void BeginVertexData(int vertexCount) { }
    virtual void BeginIndexData(int indexCount) { }
    virtual void IndexData(int index1, int index2, int index3) { }
    virtual void EdgeFlag(bool isBoundaryEdge) { }
    virtual void VertexData(Float x, Float y) { }
    virtual void Error() { }
    virtual void EndIndexData() { }
};

Tessellator::Tessellator(IClientTessellatorInterface* pClientTessellatorInterface) : m_pClientTessellatorInterface(pClientTessellatorInterface)
{
    m_state = Dormant;
    m_tolerance = 0.0;
    m_windingRule = Odd;
    m_pMesh = NULL;
}

void Tessellator::MakeDormant()
{
    if(m_pMesh != NULL)
    {
        delete m_pMesh;
        m_pMesh = NULL;
    }
    m_state = Dormant;
    m_pLastEdge = NULL;
}

void Tessellator::RequireState(TessState state) 
{ 
    if(m_state != state) 
    {
        GotoState(state);
    }
}

void Tessellator::GotoState(TessState newState)
{
    while(m_state != newState)
    {
        if(m_state < newState)
        {
            switch(m_state)
            {
            case Dormant:
                BeginPolygon();
                break;
            case InPolygon:
                BeginContour();
                break;
            default:
                break;
            }
        }
        else
        {
            switch(m_state)
            {
            case InContour:
                EndContour();
                break;
            case InPolygon:
                EndPolygon();
                break;
            default:
                break;
            }
        }
    }
}

Tessellator::~Tessellator()
{
    RequireState(Dormant);
}


int Tessellator::AddVertexData(Float x, Float y)
{
    HalfEdge *e;

    e = m_pLastEdge;
    if( e == NULL ) 
    {
      /* Make a self-loop (one vertex, one edge). */

      e = m_pMesh->MakeEdge();
      if (e == NULL) return 0;
      if (!m_pMesh->MeshSplice(e, e->get_Sym())) return 0;
    } 
    else 
    {
      /* Create a new vertex and edge which immediately follow e
       * in the ordering around the left face.
       */
      if (m_pMesh->SplitEdge(e) == NULL) return 0;
      e = e->get_LNext();
    }

    /* The new vertex is now e->Org. */
    e->get_Org()->set_X(x);
    e->get_Org()->set_Y(y);

    /* The winding of an edge says how the winding number changes as we
     * cross from the edge''s right face to its left face.  We add the
     * vertices in such an order that a CCW contour will add +1 to
     * the winding number of the region inside the contour.
     */
    e->set_Winding(1);
    e->get_Sym()->set_Winding(-1);

    m_pLastEdge = e;

    return 1;

}

void Tessellator::CacheVertex(Float x, Float y)
{
    CachedVertex *v = &m_cache[m_cacheCount++];
    v->x = x;
    v->y = y;
}

int Tessellator::EmptyCache()
{
    CachedVertex *v = m_cache;
    CachedVertex *vLast;

    m_pMesh = new Mesh(this);
    if (m_pMesh == NULL) return 0;

    for( vLast = v + m_cacheCount; v < vLast; ++v ) 
    {
        if (!AddVertexData(v->x, v->y)) 
        {
            return 0;
        }
    }
    m_cacheCount = 0;
    m_emptyCache = false;

    return 1;

}

void Tessellator::AddVertex( Float x, Float y )
{
  bool isTooLarge = false;
  
  RequireState(InContour);

  if(m_hasLastVertex)
  {
      if(fabsf(m_lastVertex.x - x) < 0.001f &&
         fabsf(m_lastVertex.y - y) < 0.001f)
      {
          return;
      }
  }
  else
  {
      m_hasLastVertex = true;
  }

  m_lastVertex.x = x;
  m_lastVertex.y = y;


  if(m_emptyCache)
  {
      if(!EmptyCache())
      {
          Error("Out of memory.");
          return;
      }
      m_pLastEdge = NULL;
  }

  if(x < -c_maxCoord)
  {
      x = -c_maxCoord;
      isTooLarge = true;
  }

  if(x > c_maxCoord)
  {
      x = c_maxCoord;
      isTooLarge = true;
  }

  if(y < -c_maxCoord)
  {
      y = -c_maxCoord;
      isTooLarge = true;
  }

  if(y > c_maxCoord)
  {
      y = c_maxCoord;
      isTooLarge = true;
  }

  if(isTooLarge) 
  {
      Error("Coordinate too large.");
  }

  if(m_pMesh == NULL)
  {
      if(m_cacheCount < c_tessMaxCache)
      {
          CacheVertex(x, y);
          return;
      }

      if(!EmptyCache())
      {
          Error("Out of memory.");
          return;
      }
  }

  if( m_pMesh == NULL ) 
  {
      if(m_cacheCount < c_tessMaxCache) 
      {
          CacheVertex(x, y);
          return;
      }
      if (!EmptyCache()) 
      {
          Error("Out of memory.");
          return;
      }
  }

  if(!AddVertexData(x, y))
  {
      Error("Out of memory.");
  }
}

void Tessellator::BeginPolygon()
{
    RequireState(Dormant);

    m_state = InPolygon;
    m_cacheCount = 0;
    m_emptyCache = false;
    m_pMesh = NULL;
}

void Tessellator::BeginContour()
{
    RequireState(InPolygon);

    m_state = InContour;
    m_pLastEdge = NULL;
    if(m_cacheCount > 0)
    {
        m_emptyCache = true;
    }
    m_hasLastVertex = false;
}

void Tessellator::EndContour()
{
    RequireState(InContour);
    m_state = InPolygon;
}

void Tessellator::EndPolygon()
{
    RequireState(InPolygon);
    m_state = Dormant;


    m_pClientTessellatorInterface->BeginData();

    if(m_pMesh == NULL)
    {
        if(RenderCache())
        {
            m_pClientTessellatorInterface->EndData();
            return;
        }

        EmptyCache();
    }

    if(!ComputeInterior())
    {
        Error("Error computing interior.");
    }

    if(!m_pMesh->TessellateInterior())
    {
        Error("Error tessellating interior.");
    }

    m_pMesh->Check();

    RenderMesh();

    RenderBoundary();

    m_pClientTessellatorInterface->EndData();

    delete m_pMesh;
    m_pMesh = NULL;
}


int Tessellator::InitPriorityQ()
{
    Vertex* v;
    Vertex* vHead;

    m_pPQ = new PriorityQ(Vertex::IsLessThanOrEqualToCallback);
    if(m_pPQ == NULL)
    {
        return 0;
    }

    vHead = m_pMesh->get_VHead();
    for(v = vHead->get_Next(); v != vHead; v = v->get_Next())
    {
        v->set_PQHandle(m_pPQ->Insert(v));
        if(v->get_PQHandle() == -1)
        {
            break;
        }
    }
    if(v != vHead)
    {
        delete m_pPQ;
        m_pPQ = NULL;
        return 0;
    }

    m_pPQ->Init();

    return 1;
}

void Tessellator::DonePriorityQ()
{
    delete m_pPQ;
    m_pPQ = NULL;
}


int Tessellator::ComputeInterior()
{
    Vertex *v, *vNext;

    m_hasFatalError = false;

    /* Each vertex defines an event for our sweep line.  Start by inserting
     * all the vertices in a priority queue.  Events are processed in
     * lexicographic order, ie.
     *
     *	e1 < e2  iff  e1.x < e2.x || (e1.x == e2.x && e1.y < e2.y)
     */
    RemoveDegenerateEdges();

    if (!InitPriorityQ())
    {
        return 0;
    }

    InitEdgeDict();

    while ( (v = (Vertex *)m_pPQ->ExtractMin()) != NULL)
    {
        for ( ;; )
        {
            vNext = (Vertex *)m_pPQ->Minimum(); /* __gl_pqSortMinimum */
            if ( vNext == NULL || !Vertex::Equals(vNext, v))
            {
                break;
            }

            /* Merge together all vertices at exactly the same location.
             * This is more efficient than processing them one at a time,
             * simplifies the code (see ConnectLeftDegenerate), and is also
             * important for correct handling of certain degenerate cases.
             * For example, suppose there are two identical edges A and B
             * that belong to different contours (so without this code they would
             * be processed by separate sweep events).  Suppose another edge C
             * crosses A and B from above.  When A is processed, we split it
             * at its intersection point with C.  However this also splits C,
             * so when we insert B we may compute a slightly different
             * intersection point.  This might leave two edges with a small
             * gap between them.  This kind of error is especially obvious
             * when using boundary extraction (GLU_TESS_BOUNDARY_ONLY).
             */
            vNext = (Vertex *)m_pPQ->ExtractMin(); /* __gl_pqSortExtractMin*/
            SpliceMergeVertices(v->get_Edge(), vNext->get_Edge());
        }
        SweepEvent(v);
    }

    /* Set tess->event for debugging purposes */
    /* __GL_DICTLISTKEY */ /* __GL_DICTLISTMIN */
    m_pEvent = ((ActiveRegion *) (m_pDict->get_Min()->get_Key()))->get_EUp()->get_Org();

    DoneEdgeDict();
    DonePriorityQ();

    if (!m_pMesh->RemoveDegenerateFaces())
    {
        return 0;
    }
    m_pMesh->Check();

    return 1;

}

void Tessellator::InitEdgeDict()
{
    m_pDict = new Dict(this, IsActiveRegionLessThanOrEqualToCallback);

    AddSentinel(-c_sentinelCoord);
    AddSentinel(c_sentinelCoord);
}

void Tessellator::DoneEdgeDict()
{
    ActiveRegion* reg;
    int fixedEdges = 0;

    while ((reg = (ActiveRegion *)(m_pDict->get_Min()->get_Key())) != NULL)
    {
        /*
         * At the end of all processing, the dictionary should contain
         * only the two sentinel edges, plus at most one "fixable" edge
         * created by ConnectRightVertex().
         */
        if ( !reg->get_IsSentinel())
        {
            assert( reg->get_FixUpperEdge() );
            assert( ++fixedEdges == 1 );
        }
        assert( reg->get_WindingNumber() == 0 );
        DeleteRegion(reg);
    }

    delete m_pDict; /* __gl_dictListDeleteDict */
    m_pDict = NULL;
}

/*
 * Both edges must be directed from right to left (this is the canonical
 * direction for the upper edge of each region).
 *
 * The strategy is to evaluate a "t" value for each edge at the
 * current sweep line position, given by tess->event.  The calculations
 * are designed to be very stable, but of course they are not perfect.
 *
 * Special case: if both edge destinations are at the sweep event,
 * we sort the edges by slope (they would otherwise compare equally).
 */
bool Tessellator::EdgeLeq(ActiveRegion *reg1, ActiveRegion *reg2)
{
    Vertex *event = m_pEvent;
    HalfEdge *e1, *e2;
    Float t1, t2;

    e1 = reg1->get_EUp();
    e2 = reg2->get_EUp();

    if ( e1->get_Dst() == event )
    {
        if ( e2->get_Dst() == event )
        {
            /* Two edges right of the sweep line which meet at the sweep event.
             * Sort them by slope.
             */
            if (Vertex::IsLessThanOrEqualTo(e1->get_Org(), e2->get_Org()))
            {
                return Vertex::EdgeSign(e2->get_Dst(), e1->get_Org(), e2->get_Org()) <= 0;
            }
            return Vertex::EdgeSign(e1->get_Dst(), e2->get_Org(), e1->get_Org()) >= 0;
        }
        return Vertex::EdgeSign( e2->get_Dst(), event, e2->get_Org()) <= 0;
    }
    if ( e2->get_Dst() == event )
    {
        return Vertex::EdgeSign( e1->get_Dst(), event, e1->get_Org()) >= 0;
    }

    /* General case - compute signed distance *from* e1, e2 to event */
    t1 = Vertex::EdgeEval( e1->get_Dst(), event, e1->get_Org());
    t2 = Vertex::EdgeEval( e2->get_Dst(), event, e2->get_Org());
    return(t1 >= t2);
}

/*
 * We add two sentinel edges above and below all other edges,
 * to avoid special cases at the top and bottom.
 */
void Tessellator::AddSentinel(Float yValue)
{
  HalfEdge *e;
  ActiveRegion *reg = get_ActiveRegionAllocator()->Allocate();

  e = m_pMesh->MakeEdge();

  e->get_Org()->set_X(c_sentinelCoord);
  e->get_Org()->set_Y(yValue);
  e->get_Dst()->set_X(-c_sentinelCoord);
  e->get_Dst()->set_Y(yValue);
  m_pEvent = e->get_Dst(); 	/* initialize it */

  reg->set_EUp(e);
  reg->set_WindingNumber(0);
  reg->set_IsInside(false);
  reg->set_FixUpperEdge(false);
  reg->set_IsSentinel(true);
  reg->set_IsDirty(false);
  reg->set_NodeUp(m_pDict->Insert(reg)); /* __gl_dictListInsertBefore */
}


void Tessellator::DeleteRegion(ActiveRegion* pRegion)
{
    if(pRegion->get_FixUpperEdge()) 
    {
        assert( pRegion->get_EUp()->get_Winding() == 0 );
    }

    pRegion->get_EUp()->set_ActiveRegion(NULL);
    m_pDict->Delete(pRegion->get_NodeUp());

    get_ActiveRegionAllocator()->Free(pRegion);
}

/*
 * Remove zero-length edges, and contours with fewer than 3 vertices.
 */
void Tessellator::RemoveDegenerateEdges()
{
    HalfEdge *e, *eNext, *eLnext;
    HalfEdge *eHead = m_pMesh->get_EHead();

    /*LINTED*/
    for ( e = eHead->get_Next(); e != eHead; e = eNext )
    {
        eNext = e->get_Next();
        eLnext = e->get_LNext();

        if ( Vertex::Equals(e->get_Org(), e->get_Dst()) && e->get_LNext()->get_LNext() != e)
        {
            /* Zero-length edge, contour has at least 3 edges */

            SpliceMergeVertices(eLnext, e); /* deletes e->Org */
            m_pMesh->Delete(e);

            e = eLnext;
            eLnext = e->get_LNext();
        }
        if ( eLnext->get_LNext() == e )
        {
            /* Degenerate contour (one or two edges) */

            if ( eLnext != e )
            {
                if(eLnext == eNext || eLnext == eNext->get_Sym())
                {
                    eNext = eNext->get_Next();
                }
                m_pMesh->Delete(eLnext);
            }
            if ( e == eNext || e == eNext->get_Sym())
            {
                eNext = eNext->get_Next();
            }
            m_pMesh->Delete(e);
        }
    }

}

/*
 * Two vertices with idential coordinates are combined into one.
 * e1->Org is kept, while e2->Org is discarded.
 */
void Tessellator::SpliceMergeVertices(HalfEdge *e1, HalfEdge *e2 )
{
    m_pMesh->MeshSplice(e1, e2);
}

/*
 * Does everything necessary when the sweep line crosses a vertex.
 * Updates the mesh and the edge dictionary.
 */
void Tessellator::SweepEvent(Vertex* vEvent )
{
    ActiveRegion *regUp, *reg;
    HalfEdge *e, *eTopLeft, *eBottomLeft;

    m_pEvent = vEvent;    /* for access in EdgeLeq() */

    /* Check if this vertex is the right endpoint of an edge that is
     * already in the dictionary.  In this case we don't need to waste
     * time searching for the location to insert new edges.
     */
    e = vEvent->get_Edge();
    while ( e->get_ActiveRegion() == NULL )
    {
        e = e->get_ONext();
        if ( e == vEvent->get_Edge() )
        {
            /* All edges go right -- not incident to any processed edges */
            ConnectLeftVertex(vEvent);
            return;
        }
    }

    /* Processing consists of two phases: first we "finish" all the
     * active regions where both the upper and lower edges terminate
     * at vEvent (ie. vEvent is closing off these regions).
     * We mark these faces "inside" or "outside" the polygon according
     * to their winding number, and delete the edges from the dictionary.
     * This takes care of all the left-going edges from vEvent.
     */
    regUp = e->get_ActiveRegion()->get_TopLeftRegion(m_pMesh);
    assert(regUp != NULL);

    reg = regUp->get_RegionBelow();

    eTopLeft = reg->get_EUp();
    eBottomLeft = FinishLeftRegions(reg, NULL );

    /* Next we process all the right-going edges from vEvent.  This
     * involves adding the edges to the dictionary, and creating the
     * associated "active regions" which record information about the
     * regions between adjacent dictionary edges.
     */
    if (eBottomLeft->get_ONext() == eTopLeft)
    {
        /* No right-going edges -- add a temporary "fixable" edge */
        ConnectRightVertex(regUp, eBottomLeft);
    }
    else
    {
        AddRightEdges(regUp, eBottomLeft->get_ONext(), eTopLeft, eTopLeft, true );
    }
}

/*
 * Purpose: insert right-going edges into the edge dictionary, and update
 * winding numbers and mesh connectivity appropriately.  All right-going
 * edges share a common origin vOrg.  Edges are inserted CCW starting at
 * eFirst; the last edge inserted is eLast->Oprev.  If vOrg has any
 * left-going edges already processed, then eTopLeft must be the edge
 * such that an imaginary upward vertical segment from vOrg would be
 * contained between eTopLeft->Oprev and eTopLeft; otherwise eTopLeft
 * should be NULL.
 */
void Tessellator::AddRightEdges(ActiveRegion *regUp, HalfEdge *eFirst, HalfEdge *eLast, HalfEdge *eTopLeft, bool cleanUp)
{
    ActiveRegion *reg, *regPrev;
    HalfEdge *e, *ePrev;
    bool isFirstTime = true;

    /* Insert the new right-going edges in the dictionary */
    e = eFirst;
    do
    {
        assert(Vertex::IsLessThanOrEqualTo(e->get_Org(), e->get_Dst()));

        AddRegionBelow(regUp, e->get_Sym());
        e = e->get_ONext();

    } while ( e != eLast );

    /* Walk *all* right-going edges from e->Org, in the dictionary order,
     * updating the winding numbers of each region, and re-linking the mesh
     * edges to match the dictionary ordering (if necessary).
     */
    if ( eTopLeft == NULL )
    {
        eTopLeft = regUp->get_RegionBelow()->get_EUp()->get_RPrev();
    }

    regPrev = regUp;
    ePrev = eTopLeft;
    for ( ;; )
    {
        reg = regPrev->get_RegionBelow();
        e = reg->get_EUp()->get_Sym();
        if (e->get_Org() != ePrev->get_Org()) break;

        if ( e->get_ONext() != ePrev )
        {
            /* Unlink e from its current position, and relink below ePrev */
            m_pMesh->MeshSplice(e->get_OPrev(), e);
            m_pMesh->MeshSplice(ePrev->get_OPrev(), e);
        }
        /* Compute the winding number and "inside" flag for the new regions */
        reg->set_WindingNumber(regPrev->get_WindingNumber() - e->get_Winding());
        reg->set_IsInside(IsWindingInside(reg->get_WindingNumber()));

        /* Check for two outgoing edges with same slope -- process these
         * before any intersection tests (see example in __gl_computeInterior).
         */
        regPrev->set_IsDirty(true);
        if ( !isFirstTime && CheckForRightSplice(regPrev))
        {
            e->AddWinding(ePrev);
            DeleteRegion(regPrev );
            m_pMesh->Delete(ePrev);
        }
        isFirstTime = false;
        regPrev = reg;
        ePrev = e;
    }
    regPrev->set_IsDirty(true);
    assert( regPrev->get_WindingNumber() - e->get_Winding() == reg->get_WindingNumber());

    if (cleanUp)
    {
        /* Check for intersections between newly adjacent edges. */
        WalkDirtyRegions(regPrev);
    }
}

bool Tessellator::IsWindingInside(int n)
{
    switch(m_windingRule)
    {
    case Odd:
        return n & 1;
    case NonZero:
        return n != 0;
    case Positive:
        return n > 0;
    case Negative:
        return n < 0;
    case AbsGeqTwo:
        return n >= 2 || n <= -2;
    default:
        assert(false);
    }

    return false;
}


/*
 * When the upper or lower edge of any region changes, the region is
 * marked "dirty".  This routine walks through all the dirty regions
 * and makes sure that the dictionary invariants are satisfied
 * (see the comments at the beginning of this file).  Of course
 * new dirty regions can be created as we make changes to restore
 * the invariants.
 */
void Tessellator::WalkDirtyRegions(ActiveRegion *regUp)
{
    ActiveRegion *regLo = regUp->get_RegionBelow();
    HalfEdge *eUp, *eLo;

    for ( ;; )
    {
        /* Find the lowest dirty region (we walk from the bottom up). */
        while (regLo->get_IsDirty())
        {
            regUp = regLo;
            regLo = regLo->get_RegionBelow();
        }
        if (!regUp->get_IsDirty())
        {
            regLo = regUp;
            regUp = regUp->get_RegionAbove();
            if ( regUp == NULL || ! regUp->get_IsDirty())
            {
                /* We've walked all the dirty regions */
                return;
            }
        }
        regUp->set_IsDirty(false);
        eUp = regUp->get_EUp();
        eLo = regLo->get_EUp();

        if ( eUp->get_Dst() != eLo->get_Dst())
        {
            /* Check that the edge ordering is obeyed at the Dst vertices. */
            if (CheckForLeftSplice(regUp))
            {

                /* If the upper or lower edge was marked fixUpperEdge, then
                 * we no longer need it (since these edges are needed only for
                 * vertices which otherwise have no right-going edges).
                 */
                if ( regLo->get_FixUpperEdge())
                {
                    DeleteRegion(regLo);

                    m_pMesh->Delete(eLo);

                    regLo = regUp->get_RegionBelow();
                    eLo = regLo->get_EUp();
                }
                else if ( regUp->get_FixUpperEdge())
                {
                    DeleteRegion(regUp);
                    m_pMesh->Delete(eUp);
                    regUp = regLo->get_RegionAbove();
                    eUp = regUp->get_EUp();
                }
            }
        }

        if(eUp->get_Org() != eLo->get_Org())
        {
            if (    eUp->get_Dst() != eLo->get_Dst()
                    && ! regUp->get_FixUpperEdge() && ! regLo->get_FixUpperEdge()
                    && (eUp->get_Dst() == m_pEvent || eLo->get_Dst() == m_pEvent) )
            {
                /* When all else fails in CheckForIntersect(), it uses tess->event
                 * as the intersection location.  To make this possible, it requires
                 * that tess->event lie between the upper and lower edges, and also
                 * that neither of these is marked fixUpperEdge (since in the worst
                 * case it might splice one of these edges into tess->event, and
                 * violate the invariant that fixable edges are the only right-going
                 * edge from their associated vertex).
                 */
                if (CheckForIntersect(regUp))
                {
                    /* WalkDirtyRegions() was called recursively; we're done */
                    return;
                }
            }
            else
            {
                /* Even though we can't use CheckForIntersect(), the Org vertices
                 * may violate the dictionary edge ordering.  Check and correct this.
                 */
                (void) CheckForRightSplice(regUp);
            }
        }
        if ( eUp->get_Org() == eLo->get_Org() && eUp->get_Dst() == eLo->get_Dst())
        {
            /* A degenerate loop consisting of only two edges -- delete it. */
            eLo->AddWinding(eUp);
            DeleteRegion(regUp);

            m_pMesh->Delete(eUp);
            regUp = regLo->get_RegionAbove();
        }
    }
}

/*
 * Add a new active region to the sweep line, *somewhere* below "regAbove"
 * (according to where the new edge belongs in the sweep-line dictionary).
 * The upper edge of the new region will be "eNewUp".
 * Winding number and "inside" flag are not updated.
 */
ActiveRegion *Tessellator::AddRegionBelow(ActiveRegion *regAbove, HalfEdge *eNewUp )
{
    ActiveRegion *regNew = get_ActiveRegionAllocator()->Allocate();
    
    regNew->set_EUp(eNewUp);
    
    regNew->set_NodeUp(m_pDict->InsertBefore(regAbove->get_NodeUp(), regNew));
    
    regNew->set_FixUpperEdge(false);
    regNew->set_IsSentinel(false);
    regNew->set_IsDirty(false);
    
    eNewUp->set_ActiveRegion(regNew);
    
    return regNew;
}

/*
 * Check the upper and lower edge of "regUp", to make sure that the
 * eUp->Org is above eLo, or eLo->Org is below eUp (depending on which
 * origin is leftmost).
 *
 * The main purpose is to splice right-going edges with the same
 * dest vertex and nearly identical slopes (ie. we can't distinguish
 * the slopes numerically).  However the splicing can also help us
 * to recover from numerical errors.  For example, suppose at one
 * point we checked eUp and eLo, and decided that eUp->Org is barely
 * above eLo.  Then later, we split eLo into two edges (eg. from
 * a splice operation like this one).  This can change the result of
 * our test so that now eUp->Org is incident to eLo, or barely below it.
 * We must correct this condition to maintain the dictionary invariants.
 *
 * One possibility is to check these edges for intersection again
 * (ie. CheckForIntersect).  This is what we do if possible.  However
 * CheckForIntersect requires that tess->event lies between eUp and eLo,
 * so that it has something to fall back on when the intersection
 * calculation gives us an unusable answer.  So, for those cases where
 * we can't check for intersection, this routine fixes the problem
 * by just splicing the offending vertex into the other edge.
 * This is a guaranteed solution, no matter how degenerate things get.
 * Basically this is a combinatorial solution to a numerical problem.
 */
bool Tessellator::CheckForRightSplice(ActiveRegion *regUp)
{
    ActiveRegion *regLo = regUp->get_RegionBelow();
    HalfEdge *eUp = regUp->get_EUp();
    HalfEdge *eLo = regLo->get_EUp();

    if ( Vertex::IsLessThanOrEqualTo(eUp->get_Org(), eLo->get_Org()))
    {
        if (Vertex::EdgeSign(eLo->get_Dst(), eUp->get_Org(), eLo->get_Org()) > 0 )
             return false;

        /* eUp->Org appears to be below eLo */
        if (!Vertex::Equals(eUp->get_Org(), eLo->get_Org()))
        {
            /* Splice eUp->Org into eLo */
            m_pMesh->SplitEdge(eLo->get_Sym());
            m_pMesh->MeshSplice(eUp, eLo->get_OPrev());
            regUp->set_IsDirty(true);
            regLo->set_IsDirty(true);

        }
        else if (eUp->get_Org() != eLo->get_Org())
        {
            /* merge the two vertices, discarding eUp->Org */
            m_pPQ->Delete(eUp->get_Org()->get_PQHandle()); 

            SpliceMergeVertices( eLo->get_OPrev(), eUp );
        }
    }
    else
    {
        if (Vertex::EdgeSign(eUp->get_Dst(), eLo->get_Org(), eUp->get_Org()) < 0 ) return false;

        /* eLo->Org appears to be above eUp, so splice eLo->Org into eUp */
        regUp->get_RegionAbove()->set_IsDirty(true);
        regUp->set_IsDirty(true);

        m_pMesh->SplitEdge(eUp->get_Sym());
        m_pMesh->MeshSplice(eLo->get_OPrev(), eUp);
    }
    return true;
}

/*
 * Check the upper and lower edge of "regUp", to make sure that the
 * eUp->Dst is above eLo, or eLo->Dst is below eUp (depending on which
 * destination is rightmost).
 *
 * Theoretically, this should always be true.  However, splitting an edge
 * into two pieces can change the results of previous tests.  For example,
 * suppose at one point we checked eUp and eLo, and decided that eUp->Dst
 * is barely above eLo.  Then later, we split eLo into two edges (eg. from
 * a splice operation like this one).  This can change the result of
 * the test so that now eUp->Dst is incident to eLo, or barely below it.
 * We must correct this condition to maintain the dictionary invariants
 * (otherwise new edges might get inserted in the wrong place in the
 * dictionary, and bad stuff will happen).
 *
 * We fix the problem by just splicing the offending vertex into the
 * other edge.
 */
bool Tessellator::CheckForLeftSplice(ActiveRegion *regUp)
{
  ActiveRegion *regLo = regUp->get_RegionBelow();
  HalfEdge *eUp = regUp->get_EUp();
  HalfEdge *eLo = regLo->get_EUp();
  HalfEdge *e;

  assert(!Vertex::Equals( eUp->get_Dst(), eLo->get_Dst()));

    if ( Vertex::IsLessThanOrEqualTo(eUp->get_Dst(), eLo->get_Dst()))
    {
        if (Vertex::EdgeSign( eUp->get_Dst(), eLo->get_Dst(), eUp->get_Org()) < 0 ) return false;

        /* eLo->Dst is above eUp, so splice eLo->Dst into eUp */
        regUp->get_RegionAbove()->set_IsDirty(true);
        regUp->set_IsDirty(true);

        e = m_pMesh->SplitEdge(eUp);
        m_pMesh->MeshSplice(eLo->get_Sym(), e);

        e->get_LFace()->set_IsInside(regUp->get_IsInside());
    }
    else
    {
        if (Vertex::EdgeSign( eLo->get_Dst(), eUp->get_Dst(), eLo->get_Org()) > 0 ) return false;

        /* eUp->Dst is below eLo, so splice eUp->Dst into eLo */
        regUp->set_IsDirty(true);
        regLo->set_IsDirty(true);

        e = m_pMesh->SplitEdge(eLo);

        m_pMesh->MeshSplice(eUp->get_LNext(), eLo->get_Sym());
        e->get_RFace()->set_IsInside(regUp->get_IsInside());
    }
    return true;
}

/*
 * Check the upper and lower edges of the given region to see if
 * they intersect.  If so, create the intersection and add it
 * to the data structures.
 *
 * Returns TRUE if adding the new intersection resulted in a recursive
 * call to AddRightEdges(); in this case all "dirty" regions have been
 * checked for intersections, and possibly regUp has been deleted.
 */

//[s, t]
bool Tessellator::CheckForIntersect( ActiveRegion *regUp )
{
  ActiveRegion *regLo = regUp->get_RegionBelow();
  HalfEdge *eUp = regUp->get_EUp();
  HalfEdge *eLo = regLo->get_EUp();
  Vertex *orgUp = eUp->get_Org();
  Vertex *orgLo = eLo->get_Org();
  Vertex *dstUp = eUp->get_Dst();
  Vertex *dstLo = eLo->get_Dst();
  Float yMinUp, yMaxLo;
  Vertex isect, *orgMin;
  HalfEdge *e;

    assert(!Vertex::Equals(dstLo, dstUp));
    assert(Vertex::EdgeSign(dstUp, m_pEvent, orgUp) <= 0 );
    assert(Vertex::EdgeSign(dstLo, m_pEvent, orgLo) >= 0 );
    assert(orgUp != m_pEvent && orgLo != m_pEvent);
    assert(!regUp->get_FixUpperEdge() && !regLo->get_FixUpperEdge());

    if ( orgUp == orgLo ) return false;    /* right endpoints are the same */

    yMinUp = MIN(orgUp->get_Y(), dstUp->get_Y());
    yMaxLo = MAX(orgLo->get_Y(), dstLo->get_Y());

    if ( yMinUp > yMaxLo ) return false;   /* t ranges do not overlap */

    if ( Vertex::IsLessThanOrEqualTo(orgUp, orgLo ))
    {
        if ( Vertex::EdgeSign( dstLo, orgUp, orgLo ) > 0 ) return false;
    }
    else
    {
        if ( Vertex::EdgeSign( dstUp, orgLo, orgUp ) < 0 ) return false;
    }

    Vertex::EdgeIntersect(dstUp, orgUp, dstLo, orgLo, &isect);

    /* The following properties are guaranteed: */
    assert(MIN(orgUp->get_Y(), dstUp->get_Y()) <= isect.get_Y());
    assert(isect.get_Y() <= MAX(orgLo->get_Y(), dstLo->get_Y()));
    assert(MIN(dstLo->get_X(), dstUp->get_X()) <= isect.get_X());
    assert(isect.get_X() <= MAX(orgLo->get_X(), orgUp->get_X()));

    if (Vertex::IsLessThanOrEqualTo(&isect, m_pEvent))
    {
        /* The intersection point lies slightly to the left of the sweep line,
         * so move it until it''s slightly to the right of the sweep line.
         * (If we had perfect numerical precision, this would never happen
         * in the first place).  The easiest and safest thing to do is
         * replace the intersection by tess->event.
         */
        isect.set_X(m_pEvent->get_X());
        isect.set_Y(m_pEvent->get_Y());
    }
    /* Similarly, if the computed intersection lies to the right of the
     * rightmost origin (which should rarely happen), it can cause
     * unbelievable inefficiency on sufficiently degenerate inputs.
     * (If you have the test program, try running test54.d with the
     * "X zoom" option turned on).
     */
    orgMin = Vertex::IsLessThanOrEqualTo( orgUp, orgLo ) ? orgUp : orgLo;
    if ( Vertex::IsLessThanOrEqualTo( orgMin, &isect ))
    {
        isect.set_X(orgMin->get_X());
        isect.set_Y(orgMin->get_Y());
    }

    if ( Vertex::Equals( &isect, orgUp ) || Vertex::Equals( &isect, orgLo ))
    {
        /* Easy case -- intersection at one of the right endpoints */
        (void) CheckForRightSplice(regUp);
        return false;
    }

    if ((!Vertex::Equals(dstUp, m_pEvent) && Vertex::EdgeSign(dstUp, m_pEvent, &isect) >= 0) ||
        (!Vertex::Equals(dstLo, m_pEvent) && Vertex::EdgeSign(dstLo, m_pEvent, &isect) <= 0))
    {
        /* Very unusual -- the new upper or lower edge would pass on the
         * wrong side of the sweep event, or through it.  This can happen
         * due to very small numerical errors in the intersection calculation.
         */
        if ( dstLo == m_pEvent )
        {
            /* Splice dstLo into eUp, and process the new region(s) */
            m_pMesh->SplitEdge(eUp->get_Sym());
            m_pMesh->MeshSplice(eLo->get_Sym(), eUp);

            regUp = regUp->get_TopLeftRegion(m_pMesh);

            eUp = regUp->get_RegionBelow()->get_EUp();

            FinishLeftRegions(regUp->get_RegionBelow(), regLo);

            AddRightEdges(regUp, eUp->get_OPrev(), eUp, eUp, true);
            return true;
        }

        if ( dstUp == m_pEvent )
        {
            /* Splice dstUp into eLo, and process the new region(s) */
            m_pMesh->SplitEdge(eLo->get_Sym());
            m_pMesh->MeshSplice(eUp->get_LNext(), eLo->get_OPrev());
            regLo = regUp;
            regUp = regUp->get_TopRightRegion();

            e = regUp->get_RegionBelow()->get_EUp()->get_RPrev();

            regLo->set_EUp(eLo->get_OPrev());

            eLo = FinishLeftRegions(regLo, NULL );

            AddRightEdges(regUp, eLo->get_ONext(), eUp->get_RPrev(), e, true);

            return true;
        }
        /* Special case: called from ConnectRightVertex.  If either
         * edge passes on the wrong side of tess->event, split it
         * (and wait for ConnectRightVertex to splice it appropriately).
         */
        if (Vertex::EdgeSign( dstUp, m_pEvent, &isect ) >= 0 )
        {
            regUp->get_RegionAbove()->set_IsDirty(true);
            regUp->set_IsDirty(true);

            m_pMesh->SplitEdge(eUp->get_Sym());

            eUp->get_Org()->set_X(m_pEvent->get_X());
            eUp->get_Org()->set_Y(m_pEvent->get_Y());
        }
        if (Vertex::EdgeSign( dstLo, m_pEvent, &isect ) <= 0 )
        {
            regUp->set_IsDirty(true);
            regLo->set_IsDirty(true);

            m_pMesh->SplitEdge(eLo->get_Sym());

            eLo->get_Org()->set_X(m_pEvent->get_X());
            eLo->get_Org()->set_Y(m_pEvent->get_Y());
        }
        /* leave the rest for ConnectRightVertex */
        return false;
    }

    /* General case -- split both edges, splice into new vertex.
     * When we do the splice operation, the order of the arguments is
     * arbitrary as far as correctness goes.  However, when the operation
     * creates a new face, the work done is proportional to the size of
     * the new face.  We expect the faces in the processed part of
     * the mesh (ie. eUp->Lface) to be smaller than the faces in the
     * unprocessed original contours (which will be eLo->Oprev->Lface).
     */
    m_pMesh->SplitEdge(eUp->get_Sym());
    m_pMesh->SplitEdge(eLo->get_Sym());
    m_pMesh->MeshSplice(eLo->get_OPrev(), eUp);

    eUp->get_Org()->set_X(isect.get_X());
    eUp->get_Org()->set_Y(isect.get_Y());
    eUp->get_Org()->set_PQHandle(m_pPQ->Insert(eUp->get_Org())); 

    regUp->get_RegionAbove()->set_IsDirty(true);
    regUp->set_IsDirty(true);
    regLo->set_IsDirty(true);;
    return false;
}


/*
 * We are given a vertex with one or more left-going edges.  All affected
 * edges should be in the edge dictionary.  Starting at regFirst->eUp,
 * we walk down deleting all regions where both edges have the same
 * origin vOrg.  At the same time we copy the "inside" flag from the
 * active region to the face, since at this point each face will belong
 * to at most one region (this was not necessarily true until this point
 * in the sweep).  The walk stops at the region above regLast; if regLast
 * is NULL we walk as far as possible.	At the same time we relink the
 * mesh if necessary, so that the ordering of edges around vOrg is the
 * same as in the dictionary.
 */
HalfEdge *Tessellator::FinishLeftRegions(ActiveRegion *regFirst, ActiveRegion *regLast)
{
    ActiveRegion *reg, *regPrev;
    HalfEdge *e, *ePrev;

    regPrev = regFirst;
    ePrev = regFirst->get_EUp();
    while ( regPrev != regLast )
    {
        regPrev->set_FixUpperEdge(false);  /* placement was OK */
        reg = regPrev->get_RegionBelow();
        e = reg->get_EUp();
        if ( e->get_Org() != ePrev->get_Org() )
        {
            if (!reg->get_FixUpperEdge())
            {
                /* Remove the last left-going edge.  Even though there are no further
                 * edges in the dictionary with this origin, there may be further
                 * such edges in the mesh (if we are adding left edges to a vertex
                 * that has already been processed).  Thus it is important to call
                 * FinishRegion rather than just DeleteRegion.
                 */
                FinishRegion(regPrev );
                break;
            }
            /* If the edge below was a temporary edge introduced by
             * ConnectRightVertex, now is the time to fix it.
             */

            e = m_pMesh->Connect(ePrev->get_LPrev(), e->get_Sym());

            reg->FixUpperEdge(e, m_pMesh);
        }

        /* Relink edges so that ePrev->Onext == e */
        if ( ePrev->get_ONext() != e )
        {
            m_pMesh->MeshSplice(e->get_OPrev(), e);
            m_pMesh->MeshSplice(ePrev, e);
        }
        FinishRegion(regPrev);  /* may change reg->eUp */
        ePrev = reg->get_EUp();
        regPrev = reg;
    }
    return ePrev;
}


/*
 * Delete a region from the sweep line.  This happens when the upper
 * and lower chains of a region meet (at a vertex on the sweep line).
 * The "inside" flag is copied to the appropriate mesh face (we could
 * not do this before -- since the structure of the mesh is always
 * changing, this face may not have even existed until now).
 */
void Tessellator::FinishRegion(ActiveRegion *reg)
{
    HalfEdge *e = reg->get_EUp();
    Face *f = e->get_LFace();
    
    f->set_IsInside(reg->get_IsInside());
    f->set_Edge(e);   /* optimization for __gl_meshTessellateMonoRegion() */
    
    DeleteRegion(reg);
}

void Tessellator::ConnectLeftVertex(Vertex *vEvent)
{
    ActiveRegion *regUp, *regLo, *reg;
    HalfEdge *eUp, *eLo, *eNew;
    ActiveRegion tmp;

    /* Get a pointer to the active region containing vEvent */
    tmp.set_EUp(vEvent->get_Edge()->get_Sym());
    
    regUp = (ActiveRegion *)(m_pDict->Search(&tmp)->get_Key());
    regLo = regUp->get_RegionBelow();
    eUp = regUp->get_EUp();
    eLo = regLo->get_EUp();
    
    /* Try merging with U or L first */
    if (Vertex::EdgeSign( eUp->get_Dst(), vEvent, eUp->get_Org() ) == 0 )
    {
        ConnectLeftDegenerate(regUp, vEvent);
        return;
    }

    /* Connect vEvent to rightmost processed vertex of either chain.
     * e->Dst is the vertex that we will connect to vEvent.
     */
    reg = Vertex::IsLessThanOrEqualTo(eLo->get_Dst(), eUp->get_Dst() ) ? regUp : regLo;

    if ( regUp->get_IsInside() || reg->get_FixUpperEdge())
    {
        if ( reg == regUp )
        {
            eNew = m_pMesh->Connect(vEvent->get_Edge()->get_Sym(), eUp->get_LNext());
        }
        else
        {
            HalfEdge *tempHalfEdge= m_pMesh->Connect(eLo->get_DNext(), vEvent->get_Edge());
            eNew = tempHalfEdge->get_Sym();
        }

        if (reg->get_FixUpperEdge())
        {
            reg->FixUpperEdge(eNew, m_pMesh);
        }
        else
        {
            ComputeWinding(AddRegionBelow(regUp, eNew));
        }

        SweepEvent(vEvent );
    }
    else
    {
        /* The new vertex is in a region which does not belong to the polygon.
         * We don''t need to connect this vertex to the rest of the mesh.
         */
        AddRightEdges(regUp, vEvent->get_Edge(), vEvent->get_Edge(), NULL, true);
    }
}

void Tessellator::ComputeWinding(ActiveRegion *reg)
{
    reg->set_WindingNumber(reg->get_RegionAbove()->get_WindingNumber() + reg->get_EUp()->get_Winding());
    reg->set_IsInside(IsWindingInside(reg->get_WindingNumber()));
}

/*
 * The event vertex lies exacty on an already-processed edge or vertex.
 * Adding the new vertex involves splicing it into the already-processed
 * part of the mesh.
 */
void Tessellator::ConnectLeftDegenerate(ActiveRegion *regUp, Vertex *vEvent )
{
    HalfEdge *e, *eTopLeft, *eTopRight, *eLast;
    ActiveRegion *reg;

    e = regUp->get_EUp();
    if ( Vertex::Equals(e->get_Org(), vEvent))
    {
        /* e->Org is an unprocessed vertex - just combine them, and wait
         * for e->Org to be pulled from the queue
         */
        SpliceMergeVertices(e, vEvent->get_Edge());
        return;
    }

    if (!Vertex::Equals(e->get_Dst(), vEvent ))
    {
        /* General case -- splice vEvent into edge e which passes through it */
        m_pMesh->SplitEdge(e->get_Sym());
        if ( regUp->get_FixUpperEdge())
        {
            /* This edge was fixable -- delete unused portion of original edge */
            m_pMesh->Delete(e->get_ONext());
            regUp->set_FixUpperEdge(false);
        }
        m_pMesh->MeshSplice(vEvent->get_Edge(), e);

        SweepEvent(vEvent); /* recurse */

        return;
    }

    /* vEvent coincides with e->Dst, which has already been processed.
     * Splice in the additional right-going edges.
     */
    regUp = regUp->get_TopRightRegion();
    reg = regUp->get_RegionBelow();
    eTopRight = reg->get_EUp()->get_Sym();
    eTopLeft = eLast = eTopRight->get_ONext();

    if (reg->get_FixUpperEdge())
    {
        /* Here e->Dst has only a single fixable edge going right.
         * We can delete it since now we have some real right-going edges.
         */
        assert( eTopLeft != eTopRight );   /* there are some left edges too */
        DeleteRegion(reg);

        m_pMesh->Delete(eTopRight);

        eTopRight = eTopLeft->get_OPrev();
    }

    m_pMesh->MeshSplice(vEvent->get_Edge(), eTopRight);

    if (!eTopLeft->GoesLeft())
    {
        /* e->Dst had no left-going edges -- indicate this to AddRightEdges() */
        eTopLeft = NULL;
    }

    AddRightEdges( regUp, eTopRight->get_ONext(), eLast, eTopLeft, true );
}



/*
 * Purpose: connect a "right" vertex vEvent (one where all edges go left)
 * to the unprocessed portion of the mesh.  Since there are no right-going
 * edges, two regions (one above vEvent and one below) are being merged
 * into one.  "regUp" is the upper of these two regions.
 *
 * There are two reasons for doing this (adding a right-going edge):
 *  - if the two regions being merged are "inside", we must add an edge
 *    to keep them separated (the combined region would not be monotone).
 *  - in any case, we must leave some record of vEvent in the dictionary,
 *    so that we can merge vEvent with features that we have not seen yet.
 *    For example, maybe there is a vertical edge which passes just to
 *    the right of vEvent; we would like to splice vEvent into this edge.
 *
 * However, we don't want to connect vEvent to just any vertex.  We don''t
 * want the new edge to cross any other edges; otherwise we will create
 * intersection vertices even when the input data had no self-intersections.
 * (This is a bad thing; if the user's input data has no intersections,
 * we don't want to generate any false intersections ourselves.)
 *
 * Our eventual goal is to connect vEvent to the leftmost unprocessed
 * vertex of the combined region (the union of regUp and regLo).
 * But because of unseen vertices with all right-going edges, and also
 * new vertices which may be created by edge intersections, we don''t
 * know where that leftmost unprocessed vertex is.  In the meantime, we
 * connect vEvent to the closest vertex of either chain, and mark the region
 * as "fixUpperEdge".  This flag says to delete and reconnect this edge
 * to the next processed vertex on the boundary of the combined region.
 * Quite possibly the vertex we connected to will turn out to be the
 * closest one, in which case we won''t need to make any changes.
 */
void Tessellator::ConnectRightVertex(ActiveRegion *regUp, HalfEdge *eBottomLeft )
{
    HalfEdge *eNew;
    HalfEdge *eTopLeft = eBottomLeft->get_ONext();
    ActiveRegion *regLo = regUp->get_RegionBelow();
    HalfEdge *eUp = regUp->get_EUp();
    HalfEdge *eLo = regLo->get_EUp();
    bool isDegenerate = false;

    if ( eUp->get_Dst() != eLo->get_Dst() )
    {
        (void) CheckForIntersect(regUp);
    }

    /* Possible new degeneracies: upper or lower edge of regUp may pass
     * through vEvent, or may coincide with new intersection vertex
     */
    if ( Vertex::Equals( eUp->get_Org(), m_pEvent ))
    {
        m_pMesh->MeshSplice(eTopLeft->get_OPrev(), eUp);

        regUp = regUp->get_TopLeftRegion(m_pMesh);

        eTopLeft = regUp->get_RegionBelow()->get_EUp();

        FinishLeftRegions(regUp->get_RegionBelow(), regLo);
        isDegenerate = true;
    }
    if ( Vertex::Equals( eLo->get_Org(), m_pEvent))
    {
        m_pMesh->MeshSplice(eBottomLeft, eLo->get_OPrev());

        eBottomLeft = FinishLeftRegions(regLo, NULL );
        isDegenerate = true;
    }
    if ( isDegenerate )
    {
        AddRightEdges( regUp, eBottomLeft->get_ONext(), eTopLeft, eTopLeft, true );
        return;
    }

    /* Non-degenerate situation -- need to add a temporary, fixable edge.
     * Connect to the closer of eLo->Org, eUp->Org.
     */
    if ( Vertex::IsLessThanOrEqualTo(eLo->get_Org(), eUp->get_Org()))
    {
        eNew = eLo->get_OPrev();
    }
    else
    {
        eNew = eUp;
    }

    eNew = m_pMesh->Connect(eBottomLeft->get_LPrev(), eNew);

    /* Prevent cleanup, otherwise eNew might disappear before we've even
     * had a chance to mark it as a temporary edge.
     */
    AddRightEdges( regUp, eNew, eNew->get_ONext(), eNew->get_ONext(), false );

    eNew->get_Sym()->get_ActiveRegion()->set_FixUpperEdge(true);
    WalkDirtyRegions(regUp);
}

/*
 * If check==FALSE, we compute the polygon normal and place it in norm[].
 * If check==TRUE, we check that each triangle in the fan from v0 has a
 * consistent orientation with respect to norm[].  If triangles are
 * consistently oriented CCW, return 1; if CW, return -1; if all triangles
 * are degenerate return 0; otherwise (no consistent orientation) return
 * SIGN_INCONSISTENT.
 */
Tessellator::NormalSign Tessellator::ComputeNormal(Float norm[3], bool check)
{
    CachedVertex *v0 = m_cache;
    CachedVertex *vn = v0 + m_cacheCount;
    CachedVertex *vc;
    Float dot, xc, yc, xp, yp, n[3];
    NormalSign sign = SignDegenerate;

    /* Find the polygon normal.  It is important to get a reasonable
     * normal even when the polygon is self-intersecting (eg. a bowtie).
     * Otherwise, the computed normal could be very tiny, but perpendicular
     * to the true plane of the polygon due to numerical noise.  Then all
     * the triangles would appear to be degenerate and we would incorrectly
     * decompose the polygon as a fan (or simply not render it at all).
     *
     * We use a sum-of-triangles normal algorithm rather than the more
     * efficient sum-of-trapezoids method (used in CheckOrientation()
     * in normal.c).  This lets us explicitly reverse the signed area
     * of some triangles to get a reasonable normal in the self-intersecting
     * case.
     */

    vc = v0 + 1;
    xc = vc->x - v0->x;
    yc = vc->y - v0->y;
    while ( ++vc < vn )
    {
        xp = xc; yp = yc;
        xc = vc->x - v0->x;
        yc = vc->y - v0->y;

        /* Compute (vp - v0) cross (vc - v0) */
        n[0] = 0.0;
        n[1] = 0.0;
        n[2] = xp*yc - yp*xc;

        dot = n[0]*norm[0] + n[1]*norm[1] + n[2]*norm[2];
        if ( ! check )
        {
            /* Reverse the contribution of back-facing triangles to get
             * a reasonable normal for self-intersecting polygons (see above)
             */
            if ( dot >= 0 )
            {
                norm[0] += n[0]; norm[1] += n[1]; norm[2] += n[2];
            }
            else
            {
                norm[0] -= n[0]; norm[1] -= n[1]; norm[2] -= n[2];
            }
        }
        else if ( dot != 0 )
        {
            /* Check the new orientation for consistency with previous triangles */
            if ( dot > 0 )
            {
                if ( sign == SignCW ) return SignInconsistent;
                sign = SignCCW;
            }
            else
            {
                if ( sign == SignCCW ) return SignInconsistent;
                sign = SignCW;
            }
        }
    }
    return sign;
}

bool Tessellator::RenderCache()
{
    CachedVertex *v0 = m_cache;
    CachedVertex *vn = v0 + m_cacheCount;
    CachedVertex *vc;
    Float norm[3];

    if ( m_cacheCount < 3 )
    {
        /* Degenerate contour -- no output */
        return true;
    }

    norm[0] = 0.0;
    norm[1] = 0.0;
    norm[2] = 1.0;

    NormalSign sign = ComputeNormal(norm, true);
    if ( sign == SignInconsistent )
    {
        /* Fan triangles did not have a consistent orientation */
        return false;
    }
    if ( sign == SignDegenerate )
    {
        /* All triangles were degenerate */
        return true;
    }

    /* Make sure we do the right thing for each winding rule */
    switch ( m_windingRule )
    {
    case Odd:
    case NonZero:
        break;
    case Positive:
        if ( sign == SignCW ) return true;
        break;
    case Negative:
        if ( sign == SignCCW ) return true;
        break;
    case AbsGeqTwo:
        return true;
        break;
    default:
        assert(false);
        return true;
    }

    // Eliminate degeneracy in the case when first vertex = last vertex
    if(fabsf(v0->x - (vn - 1)->x) < 0.001f &&
       fabsf(v0->y - (vn - 1)->y) < 0.001f)
    {
        vn -= 1;
        m_cacheCount -= 1;
    }


    m_pClientTessellatorInterface->BeginVertexData();

    for ( vc = v0; vc < vn; ++vc )
    {
        m_pClientTessellatorInterface->VertexData(vc->x, vc->y);        
    }

    m_pClientTessellatorInterface->EndVertexData();

    m_pClientTessellatorInterface->BeginIndexData();

    // polygon is convex, just triangle fan it
    for (int i = 1; i < m_cacheCount - 1; i++)
    {
        m_pClientTessellatorInterface->IndexData(0, i, i + 1);
    }

    m_pClientTessellatorInterface->EndIndexData();

    m_pClientTessellatorInterface->BeginContourData();

    m_pClientTessellatorInterface->BeginContour(sign == SignCCW);

    for ( vc = v0; vc < vn; ++vc )
    {
        m_pClientTessellatorInterface->ContourData((int) (vc - v0));
    }    

    m_pClientTessellatorInterface->EndContour();

    m_pClientTessellatorInterface->EndContourData();

    return true;
}

/************************ Strips and Fans decomposition ******************/

/* __gl_renderMesh( tess, mesh ) takes a mesh and breaks it into triangle
 * fans, strips, and separate triangles.  A substantial effort is made
 * to use as few rendering primitives as possible (ie. to make the fans
 * and strips as large as possible).
 *
 * The rendering output is provided as callbacks (see the api).
 */
void Tessellator::RenderMesh( )
{
    Face *f;
    int vIndex = 0;

    m_pClientTessellatorInterface->BeginVertexData();
        
    for( Vertex* v = m_pMesh->get_VHead()->get_Next(); v != m_pMesh->get_VHead(); v = v->get_Next() )
    {
        if(v->get_X() > c_maxCoord || v->get_X() < -c_maxCoord ||
           v->get_Y() > c_maxCoord || v->get_Y() < -c_maxCoord)
        {
            continue;

        }
        v->set_Index(vIndex++);
        m_pClientTessellatorInterface->VertexData(v->get_X(), v->get_Y());
    }

    m_pClientTessellatorInterface->EndVertexData();


    m_pClientTessellatorInterface->BeginIndexData();

    for( f = m_pMesh->get_FHead()->get_Next(); f != m_pMesh->get_FHead(); f = f->get_Next() ) 
    {
        if(f->get_IsInside())
        {
            HalfEdge* pEdge = f->get_Edge();
            HalfEdge* pEdgeNext = pEdge->get_LNext();
            HalfEdge* pEdgeNextNext = pEdgeNext->get_LNext();

            assert(pEdgeNextNext->get_LNext() == pEdge);

            m_pClientTessellatorInterface->IndexData(pEdge->get_Org()->get_Index(),
                                                     pEdgeNext->get_Org()->get_Index(),
                                                     pEdgeNextNext->get_Org()->get_Index());
        }
    }

    m_pClientTessellatorInterface->EndIndexData();

}

HalfEdge* Tessellator::FindNextInContour(HalfEdge* eCur)
{
    Vertex* pDst = eCur->get_Dst();
    HalfEdge* eStart = pDst->get_Edge();
    HalfEdge* eNext = eStart;

    do
    {
        if(!eNext->get_IsMarked() && eNext->get_RFace()->get_IsInside() == eCur->get_RFace()->get_IsInside() &&
           eNext->get_LFace()->get_IsInside() == eCur->get_LFace()->get_IsInside())
        {
            return eNext;
        }
        eNext = eNext->get_ONext();
    } while(eNext != eStart);

    return NULL;
}


void Tessellator::RenderContourData(HalfEdge* eStart)
{
    HalfEdge* eCur = eStart;

    while(eCur != NULL)
    {
        m_pClientTessellatorInterface->ContourData(eCur->get_Org()->get_Index());
        eCur->set_IsMarked(true);
        eCur->get_Sym()->set_IsMarked(true);

        eCur = FindNextInContour(eCur);
    } 
}

void Tessellator::RenderBoundary()
{
    m_pClientTessellatorInterface->BeginContourData();

    HalfEdge* e;
    HalfEdge* eNext;
                       
    for ( e = m_pMesh->get_EHead()->get_Next(); e != m_pMesh->get_EHead(); e = eNext )
    {
        eNext = e->get_Next();

        if (!e->get_IsMarked() && e->get_RFace()->get_IsInside() != e->get_LFace()->get_IsInside())
        {
            m_pClientTessellatorInterface->BeginContour(e->get_LFace()->get_IsInside());

            RenderContourData(e);

            m_pClientTessellatorInterface->EndContour();

        }

    }

    m_pClientTessellatorInterface->EndContourData();
}


}// namespace Tess
