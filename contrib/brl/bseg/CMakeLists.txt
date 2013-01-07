# brl/bseg/CMakeLists.txt
# main package for segmentation-related code and applications
# Mutual dependencies: brip --> sdet --> (vpro,segv)

# float image operations
SUBDIRS(brip)

# segmentation feature detection library
SUBDIRS(sdet)

# Markov Random Fields - This library is deprecated
IF (NOT BUILD_NONDEPRECATED_ONLY)
SUBDIRS(bmrf)
ENDIF (NOT BUILD_NONDEPRECATED_ONLY)

# segmentation view library
SUBDIRS(segv)

# segmentation application executables
SUBDIRS(sbin)

# background modeling
SUBDIRS(bbgm)

# background modeling python interface
SUBDIRS(bbgm_batch)

# voxel world modeling
SUBDIRS(bvxm)

# voxel world modeling python interface
SUBDIRS(bvxm_batch)

# Lidar modeling (segmentation and meshing)
SUBDIRS(bmdl)

# Lidar modeling python interface
SUBDIRS(bmdl_batch)

# recognition library - uses background models of bbgm and/or bvxm
SUBDIRS(brec)

# recognition library - python interface
SUBDIRS(brec_batch)

# octree
SUBDIRS(boct)

# octree based modeling
SUBDIRS(boxm)

# updated, gpu compatible octree based modeling
SUBDIRS(boxm2)

# updated, gpu compatible octree based modeling
SUBDIRS(boxm2_batch)

# octree based modeling python interface
SUBDIRS(boxm_batch)

# voxel processing library
SUBDIRS(bvpl)

# voxel processing library python interface
SUBDIRS(bvpl_batch)

# voxel processing library python interface for octrees
SUBDIRS(bvpl_octree_batch)

# Brown affine patch library, SIFT keypoint & descriptor
SUBDIRS(bapl)

#the bridge lib to convert between boxm and boxm2 scenes
SUBDIRS(boxm_bridge)

#multi gpu library
SUBDIRS(boxm2_multi)

#space-time modeling 
SUBDIRS(bstm)
SUBDIRS(bstm_batch)
