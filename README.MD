# KDTree for Autolisp

A very fast KDTree(nanoflann) for Autolisp in ObjectARX

## tree functions

- (kdtreecreate listOfPoints)
- (kdtreeradiusSearch point3d radius)
- (kdtreeknnSearch point3d num_search)
- (kdtreepoints)
- (kdtreedestroy)

search return value is ((indexs)(distances))

## map functions

Wraps std::unordered_map for AutoLisp. While the key can be any of the lisp types listed, the intent to have a fast hash key for 3d points

- (mapclear)
- (mapinsert key value)
- (mapgetat key)
- (mapcontains key)
- (mapgetall)

Allowed types for key or value (int, double, ename, point2d, point3d, string)

## set functions

Wraps std::unordered_set for AutoLisp.

- (setclear)
- (setinsert key)
- (setcontains key)
- (setgetall)

Allowed types for key (int, double, ename, point2d, point3d, string)

## delaunay

Wraps delaunay for AutoLisp.

- (delaunay pntlist)
- (delaunayhalfedges pntlist)
- (delaunaytriangles pntlist)

## voronoi

Wraps voronoi for AutoLisp.

- (voronoicen pntlist)
- (voronoiccm pntlist)

## install

appload the arx in the releases

## Supported platforms

- AutoCAD 2021-2026
- BricsCAD 2026
- GstarCAD 2026
- ZwCAD 2026
- if you don't see a version for your platform, create an issue
