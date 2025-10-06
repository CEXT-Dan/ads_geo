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
    inline static std::unique_ptr<AcGePoint3dTree> pkdTree;
public:
    ArxAdsGeo() : AcRxArxApp()
    {
        pkdTree.reset();
    }
    virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt)
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);
        return (retCode);
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt)
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);
        pkdTree.reset();
        return (retCode);
    }

    virtual void RegisterServerComponents()
    {
    }

    static int ADSPREFIX(kdtreecreate(void))
    {
        AcResBufPtr pArgs(acedGetArgs());
        if (pkdTree == nullptr)
        {
            size_t _size = 0;
            for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
                _size++;
            
            AcGePoint3dVector pnts;
            pnts.reserve(_size);

            for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
            {
                if (pTail->restype == RT3DPOINT)
                    pnts.push_back(asPnt3d(pTail->resval.rpoint));
            }
            pkdTree.reset(new AcGePoint3dTree(std::move(pnts)));
            acedRetT();
        }
        else
        {
            acedRetNil();
        }
        return RSRSLT;
    }

    static int ADSPREFIX(kdtreeradiusSearch(void))
    {
        AcResBufPtr pArgs(acedGetArgs());
        if (pkdTree != nullptr)
        {
            int numArgs = 0;
            int numArgsFound = 0;
            int radius = 0;
            AcGePoint3d searchPoint;
            for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
            {
                switch (numArgs)
                {
                    case 0:
                    {
                        if (pTail->restype == RT3DPOINT)
                        {
                            searchPoint = asPnt3d(pTail->resval.rpoint);
                            numArgsFound++;
                        }
                        break;
                    }
                    case 1:
                    {
                        if (pTail->restype == RTSHORT)
                        {
                            radius = pTail->resval.rint;
                            numArgsFound++;
                        }
                        else if (pTail->restype == RTLONG)
                        {
                            radius = pTail->resval.rlong;
                            numArgsFound++;
                        }
                        else if (pTail->restype == RTREAL)
                        {
                            radius = pTail->resval.rreal;
                            numArgsFound++;
                        }
                        break;
                    }
                    default:
                        break;
                }
                numArgs++;
            }
            if (numArgsFound == 2)
            {
                auto knnresult = pkdTree->radiusSearch(searchPoint, radius);
                AcResBufPtr pResult(AcGePoint3dTree::resultToResbuf(knnresult));
                acedRetList(pResult.get());
            }
        }
        else
        {
            acedRetNil();
        }
        return RSRSLT;
    }

    static int ADSPREFIX(kdtreeknnSearch(void))
    {
        AcResBufPtr pArgs(acedGetArgs());
        if (pkdTree != nullptr)
        {
            int numArgs = 0;
            int numArgsFound = 0;
            int num_closest = 1;
            AcGePoint3d searchPoint;
            for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
            {
                switch (numArgs)
                {
                    case 0:
                    {
                        if (pTail->restype == RT3DPOINT)
                        {
                            searchPoint = asPnt3d(pTail->resval.rpoint);
                            numArgsFound++;
                        }
                        break;
                    }
                    case 1:
                    {
                        if (pTail->restype == RTSHORT)
                        {
                            num_closest = pTail->resval.rint;
                            numArgsFound++;
                        }
                        else if (pTail->restype == RTLONG)
                        {
                            num_closest = pTail->resval.rlong;
                            numArgsFound++;
                        }
                        break;
                    }
                    default:
                        break;
                }
                numArgs++;
            }
            if (numArgsFound == 2)
            {
                auto knnresult = pkdTree->knnSearch(searchPoint, num_closest);
                AcResBufPtr pResult(AcGePoint3dTree::resultToResbuf(knnresult));
                acedRetList(pResult.get());
            }
        }
        else
        {
            acedRetNil();
        }
        return RSRSLT;
    }

    static int ADSPREFIX(kdtreepoints(void))
    {
        constexpr const size_t AcGePoint3dSize = sizeof(AcGePoint3d);
        if (pkdTree != nullptr)
        {
            AcResBufPtr pResult(acutNewRb(RTLB));
            resbuf* pResultTail = pResult.get();
            for (const auto& item : pkdTree->inputPoints())
            {
                pResultTail = pResultTail->rbnext = acutNewRb(RT3DPOINT);
                memcpy_s(pResultTail->resval.rpoint, AcGePoint3dSize, asDblArray(item), AcGePoint3dSize);
            }
            pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
            acedRetList(pResult.get());
        }
        else
        {
            acedRetNil();
        }
        return RSRSLT;
    }

    static int ADSPREFIX(kdtreedestroy(void))
    {
        if (pkdTree != nullptr)
        {
            pkdTree.reset();
            acedRetT();
        }
        else
        {
            acedRetNil();
        }
        return RSRSLT;
    }

};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(ArxAdsGeo)
#pragma warning( disable: 4838 )
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreecreate, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreeradiusSearch, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreeknnSearch, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreepoints, false)
ACED_ADSSYMBOL_ENTRY_AUTO(ArxAdsGeo, kdtreedestroy, false)
#pragma warning( pop )