# KDTree for Autolisp

A very fast KDTree(nanoflann) for Autolisp

## lisp functions

- (kdtreecreate listOfPoints)
- (kdtreeradiusSearch point3d radius)
- (kdtreeknnSearch point3d num_search)
- (kdtreepoints)
- (kdtreedestroy)

search return value is ((indexs)(distances))
