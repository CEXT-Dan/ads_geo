// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "AcGePoint3dTreeAdapter.h"
#include "AdsMap.h"
#include "VoronoiSolver.h"
#include "DelaunaySolver.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

#if defined(_ZRXTARGET)
#define ADSPREFIX(x) zds_ ## x
#elif defined(_GRXTARGET)
#define ADSPREFIX(x) gds_ ## x
#else
#define ADSPREFIX(x) ads_ ## x
#endif

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class ArxAdsGeo : public AcRxArxApp
{
public:
    ArxAdsGeo() : AcRxArxApp()
    {
    }

    virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt)
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);
        return (retCode);
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt)
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);
        return (retCode);
    }

    virtual void RegisterServerComponents()
    {
    }

    static int ADSPREFIX(kdtreecreate(void))
    {
        return AcGePoint3dTreeWrapper::kdtreecreate();
    }

    static int ADSPREFIX(kdtreeradiusSearch(void))
    {
        return AcGePoint3dTreeWrapper::kdtreeradiusSearch();
    }

    static int ADSPREFIX(kdtreeknnSearch(void))
    {
        return AcGePoint3dTreeWrapper::kdtreeknnSearch();
    }

    static int ADSPREFIX(kdtreepoints(void))
    {
        return AcGePoint3dTreeWrapper::kdtreepoints();
    }

    static int ADSPREFIX(kdtreedestroy(void))
    {
        return AcGePoint3dTreeWrapper::kdtreedestroy();
    }

    static int ADSPREFIX(mapclear(void))
    {
        return AdsObjectMapWrapper::AdsObjectMapClear();
    }

    static int ADSPREFIX(mapinsert(void))
    {
        return AdsObjectMapWrapper::AdsObjectMapInsert();
    }

    static int ADSPREFIX(mapgetat(void))
    {
        return AdsObjectMapWrapper::AdsObjectMapgetat();
    }

    static int ADSPREFIX(mapcontains(void))
    {
        return AdsObjectMapWrapper::AdsObjectMapContains();
    }

    static int ADSPREFIX(mapgetall(void))
    {
        return AdsObjectMapWrapper::AdsObjectMapgetall();
    }

    static int ADSPREFIX(setclear(void))
    {
        return AdsObjectSetWrapper::AdsObjectSetClear();
    }

    static int ADSPREFIX(setinsert(void))
    {
        return AdsObjectSetWrapper::AdsObjectSetInsert();
    }

    static int ADSPREFIX(setcontains(void))
    {
        return AdsObjectSetWrapper::AdsObjectSetContains();
    }

    static int ADSPREFIX(setgetall(void))
    {
        return AdsObjectSetWrapper::AdsObjectSetgetall();
    }

    static int ADSPREFIX(voronoiccm(void))
    {
        return VoronoiSolver::voronoiCCMSolverLispFunc();
    }

    static int ADSPREFIX(voronoicen(void))
    {
        return VoronoiSolver::voronoiCENSolverLispFunc();
    }

    static int ADSPREFIX(delaunay(void))
    {
        return DelaunaySolver::delaunaySolverLispFunc();
    }

    static int ADSPREFIX(delaunayhalfedges(void))
    {
        return DelaunaySolver::delaunaySolverLispHalfEdgesFunc();
    }

    static int ADSPREFIX(delaunaytriangles(void))
    {
        return DelaunaySolver::delaunaySolverLispTrianglesFunc();
    }
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(ArxAdsGeo)
#pragma warning ( push )
#pragma warning( disable: 4838 )
//tree
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreecreate, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreeradiusSearch, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreeknnSearch, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreepoints, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreedestroy, false)
//map
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, mapclear, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, mapinsert, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, mapgetat, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, mapcontains, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, mapgetall, false)
//set
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, setclear, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, setinsert, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, setcontains, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, setgetall, false)
//voronoi
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, voronoiccm, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, voronoicen, false)
//delaunay
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, delaunay, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, delaunayhalfedges, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, delaunaytriangles, false)

#pragma warning( pop )
