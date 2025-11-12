#pragma once
class VoronoiSolver
{
    using PointArray = std::array<AcGePoint3d, 2>;
public:
    VoronoiSolver(const std::vector<AcGePoint3d>& v);
    delaunator::Edges solveCCM();
    delaunator::Edges solveCEN();

    static int voronoiCCMSolverLispFunc();
    static int voronoiCENSolverLispFunc();

public:
    std::vector<AcGePoint3d> inputPoints;
};

