#pragma once

#include "nanoflann.hpp"

constexpr int leafSize = 12;
using AcGePoint3dVector = std::vector<AcGePoint3d>;

struct AcGePoint3dTreeResult
{
    std::vector<int> indexes;
    std::vector<double> distances;
};

//-----------------------------------------------------------------------------------------
//AcGePoint3dTreeAdapter
struct AcGePoint3dTreeAdapter
{
    explicit AcGePoint3dTreeAdapter(AcGePoint3dVector&& points);
    ~AcGePoint3dTreeAdapter() = default;
    size_t kdtree_get_point_count() const;
    double kdtree_get_pt(const size_t idx, const size_t dim) const;
    template <class BBOX>
    inline bool kdtree_get_bbox(BBOX&) const
    {
        return false;
    }
    AcGePoint3dVector mpoints;
};

//-----------------------------------------------------------------------------------------
//AcGePoint3dTree
class AcGePoint3dTree
{
    using kd_tree3d_t = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<double, AcGePoint3dTreeAdapter>, AcGePoint3dTreeAdapter, 3>;

public:
    explicit AcGePoint3dTree(AcGePoint3dVector&& points);
    ~AcGePoint3dTree() = default;
    void                    create();
    AcGePoint3dTreeResult   radiusSearch(const AcGePoint3d& point, double radius) const;
    AcGePoint3dTreeResult   knnSearch(const AcGePoint3d& point, int num_closest) const;
    AcGePoint3dVector       inputPoints() const;

    //free me
    static resbuf*/*      */resultToResbuf(const AcGePoint3dTreeResult& knnresult);

private:
    AcGePoint3dTreeAdapter madapter;
    std::unique_ptr<kd_tree3d_t> mpTree;
};

