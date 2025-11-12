#pragma once
class DelaunaySolver
{
public:
    DelaunaySolver(const std::vector<AcGePoint3d>& v);
    std::vector<std::array<AcGePoint3d, 3>> solve();
    std::vector<size_t> solveHalfEdges();
    std::vector<size_t> solveTriangles();
    std::vector<std::array<AcGePoint3d, 3>> solveConstrained(double alpha);

    static int delaunaySolverLispFunc();
    static int delaunayConstrainedSolverLispFunc();
    static int delaunaySolverLispHalfEdgesFunc();
    static int delaunaySolverLispTrianglesFunc();

public:
    std::vector<AcGePoint3d> inputPoints;
};

