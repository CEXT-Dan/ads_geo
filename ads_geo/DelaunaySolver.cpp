#include "stdafx.h"
#include "DelaunaySolver.h"
#include "delaunator.h"


static double circum(const AcGePoint3d& pa, const AcGePoint3d& pb, const AcGePoint3d& pc)
{
    const auto a = sqrt(pow((pa[0] - pb[0]) * 2, 2) + pow((pa[1] - pb[1]) * 2, 2));
    const auto b = sqrt(pow((pb[0] - pc[0]) * 2, 2) + pow((pb[1] - pc[1]) * 2, 2));
    const auto c = sqrt(pow((pc[0] - pa[0]) * 2, 2) + pow((pc[1] - pa[1]) * 2, 2));
    const auto s = (a + b + c) / 2.0;
    const auto area = sqrt(s * (s - a) * (s - b) * (s - c));
    return a * b * c / (4.0 * area);
}

DelaunaySolver::DelaunaySolver(const std::vector<AcGePoint3d>& v)
    : inputPoints(v)
{
}

std::vector<std::array<AcGePoint3d, 3>> DelaunaySolver::solve()
{
    std::vector<double> coords;
    std::vector<std::array<AcGePoint3d, 3>> result;
    coords.reserve(inputPoints.size() * 2);
    for (const auto& item : inputPoints)
    {
        coords.emplace_back(item.x);
        coords.emplace_back(item.y);
    }
    try
    {
        delaunator::Delaunator d(coords);
        result.reserve(inputPoints.size());
        for (size_t i = 0; i < d.triangles.size(); i += 3)
        {
            result.emplace_back(std::array<AcGePoint3d, 3>
            {
                inputPoints[d.triangles[i + 0]],
                    inputPoints[d.triangles[i + 1]],
                    inputPoints[d.triangles[i + 2]]
            });
        }
    }
    catch (...)
    {
        acutPrintf(_T("\nDelaunaySolver failed"));
    }
    return result;
}

std::vector<size_t> DelaunaySolver::solveHalfEdges()
{
    std::vector<double> coords;
    std::vector<size_t> result;
    coords.reserve(inputPoints.size() * 2);
    for (const auto& item : inputPoints)
    {
        coords.emplace_back(item.x);
        coords.emplace_back(item.y);
    }
    try
    {
        delaunator::Delaunator d(coords);
        result.reserve(inputPoints.size());
        for (size_t i = 0; i < d.halfedges.size(); i++)
        {
            result.push_back(d.halfedges[i]);
        }
    }
    catch (...)
    {
        acutPrintf(_T("\nDelaunaySolver failed"));
    }
    return result;
}

std::vector<size_t> DelaunaySolver::solveTriangles()
{
    std::vector<double> coords;
    std::vector<size_t> result;
    coords.reserve(inputPoints.size() * 2);
    for (const auto& item : inputPoints)
    {
        coords.emplace_back(item.x);
        coords.emplace_back(item.y);
    }
    try
    {
        delaunator::Delaunator d(coords);
        result.reserve(inputPoints.size());
        for (size_t i = 0; i < d.triangles.size(); i++)
        {
            result.push_back(d.triangles[i]);
        }
    }
    catch (...)
    {
        acutPrintf(_T("\nDelaunaySolver failed"));
    }
    return result;
}

std::vector<std::array<AcGePoint3d, 3>> DelaunaySolver::solveConstrained(double alpha)
{
    std::vector<double> coords;
    std::vector<std::array<AcGePoint3d, 3>> result;
    coords.reserve(inputPoints.size() * 2);
    for (const auto& item : inputPoints)
    {
        coords.emplace_back(item.x);
        coords.emplace_back(item.y);
    }
    try
    {
        delaunator::Delaunator d(coords);
        result.reserve(inputPoints.size());
        for (size_t i = 0; i < d.triangles.size(); i += 3)
        {
            const auto& p0 = inputPoints[d.triangles[i + 0]];
            const auto& p1 = inputPoints[d.triangles[i + 1]];
            const auto& p2 = inputPoints[d.triangles[i + 2]];

            if (alpha < 0 || circum(p0, p1, p2) < alpha)
            {
                result.emplace_back(std::array<AcGePoint3d, 3>
                {
                    p0, p1, p2
                });
            }
        }
    }
    catch (...)
    {
        acutPrintf(_T("\nDelaunaySolver failed"));
    }
    return result;
}

int DelaunaySolver::delaunaySolverLispFunc()
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
        DelaunaySolver solver(std::move(points));
        const auto& result = solver.solve();
        if (result.size())
        {
            AcResBufPtr pRes(acutNewRb(RTLB));
            resbuf* pResTail = pRes.get();
            for (auto& item : result)
            {
                pResTail = pResTail->rbnext = acutNewRb(RTLB);
                for (auto& sitem : item)
                {
                    pResTail = pResTail->rbnext = acutNewRb(RT3DPOINT);
                    memcpy(pResTail->resval.rpoint, asDblArray(sitem), sizeof(pResTail->resval.rpoint));
                }
                pResTail = pResTail->rbnext = acutNewRb(RTLE);
            }
            pResTail = pResTail->rbnext = acutNewRb(RTLE);
            acedRetList(pRes.get());
        }
    }
    return (RSRSLT);
}

int DelaunaySolver::delaunayConstrainedSolverLispFunc()
{
    double alpha = -1;
    std::vector<AcGePoint3d> points;
    {
        AcResBufPtr pArgs(acedGetArgs());
        for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
        {
            switch (pTail->restype)
            {
                case RTSHORT:
                    alpha = pTail->resval.rint;
                    break;
                case RTLONG:
                    alpha = pTail->resval.rlong;
                    break;
                case RTREAL:
                    alpha = pTail->resval.rreal;
                    break;
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
        DelaunaySolver solver(std::move(points));
        const auto& result = solver.solveConstrained(alpha);
        if (result.size())
        {
            AcResBufPtr pRes(acutNewRb(RTLB));
            resbuf* pResTail = pRes.get();
            for (auto& item : result)
            {
                pResTail = pResTail->rbnext = acutNewRb(RTLB);
                for (auto& sitem : item)
                {
                    pResTail = pResTail->rbnext = acutNewRb(RT3DPOINT);
                    memcpy(pResTail->resval.rpoint, asDblArray(sitem), sizeof(pResTail->resval.rpoint));
                }
                pResTail = pResTail->rbnext = acutNewRb(RTLE);
            }
            pResTail = pResTail->rbnext = acutNewRb(RTLE);
            acedRetList(pRes.get());
        }
    }
    return (RSRSLT);
}

int DelaunaySolver::delaunaySolverLispHalfEdgesFunc()
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
        DelaunaySolver solver(std::move(points));
        const auto& result = solver.solveHalfEdges();
        if (result.size())
        {
            AcResBufPtr pRes(acutNewRb(RTLB));
            resbuf* pResTail = pRes.get();
            for (auto& item : result)
            {
                pResTail = pResTail->rbnext = acutNewRb(RTLONG);
                pResTail->resval.rlong = (item == delaunator::INVALID_INDEX ? -1 : item);
            }
            pResTail = pResTail->rbnext = acutNewRb(RTLE);
            acedRetList(pRes.get());
        }
    }
    return (RSRSLT);
}

int DelaunaySolver::delaunaySolverLispTrianglesFunc()
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
        DelaunaySolver solver(std::move(points));
        const auto& result = solver.solveTriangles();
        if (result.size())
        {
            AcResBufPtr pRes(acutNewRb(RTLB));
            resbuf* pResTail = pRes.get();
            for (auto& item : result)
            {
                pResTail = pResTail->rbnext = acutNewRb(RTLONG);
                pResTail->resval.rlong = (item == delaunator::INVALID_INDEX ? -1 : item);
            }
            pResTail = pResTail->rbnext = acutNewRb(RTLE);
            acedRetList(pRes.get());
        }
    }
    return (RSRSLT);
}
