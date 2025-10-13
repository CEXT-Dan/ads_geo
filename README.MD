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
(mapclear)
(mapinsert key value)
(mapgetat key value)
(mapcontains key)
(mapgetall)

Allowed types for key or value (int, double, ename, point2d, point3d, string)


## install

appload the arx in the releases

## Supported platforms

* AutoCAD 2025-2026
* if you don't see a version for your platform, create an issue
