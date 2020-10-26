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
** $Header: /cvs/mesa/Mesa/src/glu/sgi/libtess/tess.h,v 1.1 2001/03/17 00:25:41 brianp Exp $
*/

#ifndef __tess_h_
#define __tess_h_

#include "tesscommon.h"
#include "PoolAllocator.h"

namespace Tess
{

/* The begin/end calls must be properly nested.  We keep track of
 * the current state to enforce the ordering.
 */

/* We cache vertex data for single-contour polygons so that we can
 * try a quick-and-dirty decomposition first.
 */

class HalfEdge;
class Mesh;
class Dict;
class PriorityQ;
class Vertex;
class Face;
class ActiveRegion;

typedef PoolAllocator<Vertex, 64> VertexAllocator;
typedef PoolAllocator<HalfEdge, 64> HalfEdgeAllocator;
typedef PoolAllocator<Face, 64> FaceAllocator;
typedef PoolAllocator<ActiveRegion, 32> ActiveRegionAllocator;

class IClientTessellatorInterface
{
public:

    virtual void BeginData() = 0;

    virtual void BeginVertexData() = 0;
    virtual void VertexData(Float x, Float y) = 0;
    virtual void EndVertexData() = 0;

    virtual void BeginIndexData() = 0;
    virtual void IndexData(int index1, int index2, int index3) = 0;
    virtual void EndIndexData() = 0;

    virtual void BeginContourData() = 0;
    virtual void BeginContour(bool isLeftFilled) = 0;
    virtual void ContourData(int index) = 0;
    virtual void EndContour() = 0;
    virtual void EndContourData() = 0;

    virtual void EndData() = 0;
}; 

class Tessellator 
{
    friend class Mesh;
public:

    Tessellator(IClientTessellatorInterface* pClientTessellatorInterface);
    ~Tessellator();

    void BeginPolygon();
    void BeginContour();

    void AddVertex(Float x, Float y);

    void EndContour();
    void EndPolygon();

    enum WindingRule
    {
        Odd,
        NonZero,
        Positive,
        Negative,
        AbsGeqTwo
    };

private:

    enum TessState
    {
        Dormant,
        InPolygon,
        InContour
    };

    enum NormalSign
    {
        SignCCW,
        SignCW,
        SignInconsistent,
        SignDegenerate
    };

    VertexAllocator* get_VertexAllocator() { return &m_vertexAllocator; }
    HalfEdgeAllocator* get_HalfEdgeAllocator() { return &m_halfEdgeAllocator; }
    FaceAllocator* get_FaceAllocator() { return &m_faceAllocator; }
    ActiveRegionAllocator* get_ActiveRegionAllocator() { return &m_activeRegionAllocator; }

    void MakeDormant();
    void RequireState(TessState state);
    void GotoState(TessState newState);
    void CacheVertex(Float x, Float y);
    int EmptyCache();
    int AddVertexData(Float x, Float y);

    int ComputeInterior();

    //void RenderBoundary();
    void RenderMesh();
    bool RenderCache();

    int InitPriorityQ();
    void DonePriorityQ();

    void InitEdgeDict();
    void DoneEdgeDict();

    void Error(const char*) { }
    void AddSentinel(Float yValue);
    void DeleteRegion(ActiveRegion* pRegion);
    void RemoveDegenerateEdges();
    void SpliceMergeVertices(HalfEdge *e1, HalfEdge *e2 );
    void AddRightEdges(ActiveRegion *regUp, HalfEdge *eFirst, HalfEdge *eLast, HalfEdge *eTopLeft, bool cleanUp);
    ActiveRegion *AddRegionBelow(ActiveRegion *regAbove, HalfEdge *eNewUp);
    bool CheckForRightSplice(ActiveRegion *regUp);
    bool CheckForLeftSplice(ActiveRegion *regUp);
    bool CheckForIntersect( ActiveRegion *regUp );
    HalfEdge *FinishLeftRegions(ActiveRegion *regFirst, ActiveRegion *regLast);
    void ConnectLeftVertex(Vertex *vEvent);
    void ConnectLeftDegenerate(ActiveRegion *regUp, Vertex *vEvent );
    void ConnectRightVertex(ActiveRegion *regUp, HalfEdge *eBottomLeft);

    void ComputeWinding(ActiveRegion *reg);
    NormalSign ComputeNormal(Float norm[3], bool check);

    void RenderBoundary();
    void RenderContourData(HalfEdge* eStart);
    HalfEdge* FindNextInContour(HalfEdge* eCur);


    void FinishRegion(ActiveRegion *reg);

    void SweepEvent(Vertex* vEvent);
    bool IsWindingInside(int n);
    void WalkDirtyRegions(ActiveRegion *regUp);


    bool EdgeLeq(ActiveRegion *reg1, ActiveRegion *reg2);
    static bool IsActiveRegionLessThanOrEqualToCallback(void* pTess, void* pAR1, void* pAR2) 
    { 
        return ((Tessellator*)pTess)->EdgeLeq((ActiveRegion*) pAR1, (ActiveRegion*) pAR2);
    }


    struct CachedVertex
    {
        Float x;
        Float y;
    };

    /*** state needed for collecting the input data ***/


    IClientTessellatorInterface* m_pClientTessellatorInterface;

    TessState m_state;        /* what begin/end calls have we seen? */
    HalfEdge   *m_pLastEdge;  /* lastEdge->Org is the most recent vertex */
    Mesh   *m_pMesh;          /* stores the input contours, and eventually
                                 the tessellation itself */

    /*** state needed for the line sweep ***/

    Float m_tolerance;   /* tolerance for merging features */
    WindingRule m_windingRule;    /* rule for determining polygon interior */

    Dict      *m_pDict;      /* edge dictionary for sweep line */
    PriorityQ *m_pPQ;        /* priority queue of vertex events */
    Vertex *m_pEvent;     /* current sweep event being processed */

    /*** state needed for rendering callbacks (see render.c) ***/

    bool m_flagBoundaryEdges;   /* mark boundary edges (use EdgeFlag) */

    /* list of triangles which could not be rendered as strips or fans */
    Face *m_pLonelyTriList;

    /*** state needed to cache single-contour polygons for renderCache() */

    bool m_emptyCache;     /* empty cache on next vertex() call */
    int m_cacheCount;     /* number of cached vertices */

    static const int c_tessMaxCache = 500;
    CachedVertex m_cache[c_tessMaxCache];  /* the vertex data */

    static const Float c_maxCoord;
    static const Float c_sentinelCoord;

    bool m_hasLastVertex;
    CachedVertex m_lastVertex;

    bool m_hasFatalError;

    VertexAllocator m_vertexAllocator;
    HalfEdgeAllocator m_halfEdgeAllocator;
    FaceAllocator m_faceAllocator;
    ActiveRegionAllocator m_activeRegionAllocator;
};

}; // namespace Tess

#endif
