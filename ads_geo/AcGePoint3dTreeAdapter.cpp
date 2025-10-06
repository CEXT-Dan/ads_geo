#include "stdafx.h"
#include "AcGePoint3dTreeAdapter.h"

//-----------------------------------------------------------------------------------------
//AcGePoint3dTreeAdapter
AcGePoint3dTreeAdapter::AcGePoint3dTreeAdapter(AcGePoint3dVector&& points)
    : mpoints(std::move(points))
{
}

size_t AcGePoint3dTreeAdapter::kdtree_get_point_count() const
{
    return mpoints.size();
}

double AcGePoint3dTreeAdapter::kdtree_get_pt(const size_t idx, const size_t dim) const
{
    switch (dim)
    {
        case 0:
            return mpoints.at(idx).x;
        case 1:
            return mpoints.at(idx).y;
        default:
            return mpoints.at(idx).z;
    }
}

//-----------------------------------------------------------------------------------------
//AcGePoint3dTree
AcGePoint3dTree::AcGePoint3dTree(AcGePoint3dVector&& points)
    : madapter(std::move(points))
{
    create();
}

void AcGePoint3dTree::create()
{
    nanoflann::KDTreeSingleIndexAdaptorParams params;
    params.leaf_max_size = leafSize;
    params.n_thread_build = 0;
    mpTree.reset(new kd_tree3d_t(3, madapter, params));
    mpTree->buildIndex();
}

AcGePoint3dTreeResult AcGePoint3dTree::radiusSearch(const AcGePoint3d& point, double radius) const
{
    AcGePoint3dTreeResult result;
    nanoflann::SearchParameters params;
    std::vector<nanoflann::ResultItem<uint32_t, double>> ret_matches;
    const size_t num_matches = mpTree->radiusSearch(asDblArray(point), radius, ret_matches, params);
    for (size_t idx = 0; idx < num_matches; idx++)
    {
        result.indexes.push_back(ret_matches.at(idx).first);
        result.distances.push_back(ret_matches.at(idx).second);
    }
    return result;
}

AcGePoint3dTreeResult AcGePoint3dTree::knnSearch(const AcGePoint3d& point, int num_closest) const
{
    AcGePoint3dTreeResult result;
    std::vector<uint32_t> ret_index(num_closest);
    std::vector<double> out_dist_sqr(num_closest);
    const size_t num_matches = mpTree->knnSearch(asDblArray(point), num_closest, &ret_index[0], &out_dist_sqr[0]);
    for (size_t idx = 0; idx < num_matches; idx++)
    {
        result.indexes.push_back(ret_index.at(idx));
        result.distances.push_back(out_dist_sqr.at(idx));
    }
    return result;
}

AcGePoint3dVector AcGePoint3dTree::inputPoints() const
{
    return madapter.mpoints;
}

resbuf* AcGePoint3dTree::resultToResbuf(const AcGePoint3dTreeResult& knnresult)
{
    resbuf* pResultHead =acutNewRb(RTLB);
    resbuf* pResultTail = pResultHead;
    {
        {//indexes
            pResultTail = pResultTail->rbnext = acutNewRb(RTLB);
            for (auto item : knnresult.indexes)
            {
                pResultTail = pResultTail->rbnext = acutNewRb(RTLONG);
                pResultTail->resval.rlong = item;
            }
            pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
        }
        {//distances
            pResultTail = pResultTail->rbnext = acutNewRb(RTLB);
            for (auto item : knnresult.distances)
            {
                pResultTail = pResultTail->rbnext = acutNewRb(RTREAL);
                pResultTail->resval.rreal = item;
            }
            pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
        }
        pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
    }
    return pResultHead;
}

//-----------------------------------------------------------------------------------------
//AcGePoint3dTreeWrapper
int AcGePoint3dTreeWrapper::kdtreecreate()
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

int AcGePoint3dTreeWrapper::kdtreeradiusSearch()
{
    AcResBufPtr pArgs(acedGetArgs());
    if (pkdTree != nullptr)
    {
        int argNum = 0;
        int argNumFound = 0;
        double radius = 0.0;
        AcGePoint3d searchPoint;
        for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
        {
            switch (argNum)
            {
                case 0:
                {
                    if (pTail->restype == RT3DPOINT)
                    {
                        searchPoint = asPnt3d(pTail->resval.rpoint);
                        argNumFound++;
                    }
                    break;
                }
                case 1:
                {
                    if (pTail->restype == RTSHORT)
                    {
                        radius = pTail->resval.rint;
                        argNumFound++;
                    }
                    else if (pTail->restype == RTLONG)
                    {
                        radius = pTail->resval.rlong;
                        argNumFound++;
                    }
                    else if (pTail->restype == RTREAL)
                    {
                        radius = pTail->resval.rreal;
                        argNumFound++;
                    }
                    break;
                }
                default:
                    break;
            }
            argNum++;
        }
        if (argNumFound == 2)
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

int AcGePoint3dTreeWrapper::kdtreeknnSearch()
{
    AcResBufPtr pArgs(acedGetArgs());
    if (pkdTree != nullptr)
    {
        int argNum = 0;
        int argNumFound = 0;
        int num_closest = 1;
        AcGePoint3d searchPoint;
        for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
        {
            switch (argNum)
            {
                case 0:
                {
                    if (pTail->restype == RT3DPOINT)
                    {
                        searchPoint = asPnt3d(pTail->resval.rpoint);
                        argNumFound++;
                    }
                    break;
                }
                case 1:
                {
                    if (pTail->restype == RTSHORT)
                    {
                        num_closest = pTail->resval.rint;
                        argNumFound++;
                    }
                    else if (pTail->restype == RTLONG)
                    {
                        num_closest = pTail->resval.rlong;
                        argNumFound++;
                    }
                    break;
                }
                default:
                    break;
            }
            argNum++;
        }
        if (argNumFound == 2)
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

int AcGePoint3dTreeWrapper::kdtreepoints()
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

int AcGePoint3dTreeWrapper::kdtreedestroy()
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
