#include "stdafx.h"
#include "VoronoiSolver.h"
#include "delaunator.h"


VoronoiSolver::VoronoiSolver(const std::vector<AcGePoint3d>& v)
    : inputPoints(v)
{
}

delaunator::Edges VoronoiSolver::solveCCM()
{
    std::vector<double> coords;
    delaunator::Edges result;
    coords.reserve(inputPoints.size() * 2);
    for (const auto& item : inputPoints)
    {
        coords.emplace_back(item.x);
        coords.emplace_back(item.y);
    }
    try
    {
        delaunator::Voronoiator v(coords, inputPoints);
        result = v.getVoronoiEdgesCCM();
    }
    catch (...)
    {
        acutPrintf(_T("\nVoronoiSolver failed"));
    }
    return result;
}

delaunator::Edges VoronoiSolver::solveCEN()
{
    std::vector<double> coords;
    delaunator::Edges result;
    coords.reserve(inputPoints.size() * 2);
    for (const auto& item : inputPoints)
    {
        coords.emplace_back(item.x);
        coords.emplace_back(item.y);
    }
    try
    {
        delaunator::Voronoiator v(coords, inputPoints);
        result = v.getVoronoiEdgesCEN();
    }
    catch (...)
    {
        acutPrintf(_T("\nVoronoiSolver failed"));
    }
    return result;
}

int VoronoiSolver::voronoiCCMSolverLispFunc()
{
    std::vector<AcGePoint3d> points;
    {
        AcResBufPtr pArgs(acedGetArgs());
        for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
        {
            switch (pTail->restype)
            {
                case RTPOINT:
                case RT3DPOINT:
                {
                    points.push_back(asPnt3d(pTail->resval.rpoint));
                    break;
                }
                default:
                    break;
            }
        }
    }
    if (points.size() > 2)
    {
        VoronoiSolver solver(std::move(points));
        const auto& result = solver.solveCCM();
        if (result.size())
        {
            AcResBufPtr pRes(acutNewRb(RTLB));
            resbuf* pResTail = pRes.get();
            for (auto& item : result)
            {
                pResTail = pResTail->rbnext = acutNewRb(RTLB);

                pResTail = pResTail->rbnext = acutNewRb(RT3DPOINT);
                memcpy(pResTail->resval.rpoint, asDblArray(item.a), sizeof(pResTail->resval.rpoint));

                pResTail = pResTail->rbnext = acutNewRb(RT3DPOINT);
                memcpy(pResTail->resval.rpoint, asDblArray(item.b), sizeof(pResTail->resval.rpoint));

                pResTail = pResTail->rbnext = acutNewRb(RTLE);
            }
            pResTail = pResTail->rbnext = acutNewRb(RTLE);
            acedRetList(pRes.get());
        }
    }
    return (RSRSLT);
}

int VoronoiSolver::voronoiCENSolverLispFunc()
{
    std::vector<AcGePoint3d> points;
    {
        AcResBufPtr pArgs(acedGetArgs());
        for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
        {
            switch (pTail->restype)
            {
                case RTPOINT:
                case RT3DPOINT:
                {
                    points.push_back(asPnt3d(pTail->resval.rpoint));
                    break;
                }
                default:
                    break;
            }
        }
    }
    if (points.size() > 2)
    {
        VoronoiSolver solver(std::move(points));
        const auto& result = solver.solveCEN();
        if (result.size())
        {
            AcResBufPtr pRes(acutNewRb(RTLB));
            resbuf* pResTail = pRes.get();
            for (auto& item : result)
            {
                pResTail = pResTail->rbnext = acutNewRb(RTLB);

                pResTail = pResTail->rbnext = acutNewRb(RT3DPOINT);
                memcpy(pResTail->resval.rpoint, asDblArray(item.a), sizeof(pResTail->resval.rpoint));

                pResTail = pResTail->rbnext = acutNewRb(RT3DPOINT);
                memcpy(pResTail->resval.rpoint, asDblArray(item.b), sizeof(pResTail->resval.rpoint));

                pResTail = pResTail->rbnext = acutNewRb(RTLE);
            }
            pResTail = pResTail->rbnext = acutNewRb(RTLE);
            acedRetList(pRes.get());
        }
    }
    return (RSRSLT);
}
