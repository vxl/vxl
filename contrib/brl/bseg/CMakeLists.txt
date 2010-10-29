# brl/bseg/CMakeLists.txt
# main package for segmentation-related code and applications
# Mutual dependencies: brip --> sdet --> (vpro,segv)

# float image operations
SUBDIRS(brip)

# segmentation feature detection library
SUBDIRS(sdet)

# video processors
SUBDIRS(vpro)

# tracking
SUBDIRS(strk)

# Markov Random Fields - This library is deprecated
#SUBDIRS(bmrf)

# misc
#SUBDIRS(bcons)

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

# octree based modeling python interface
SUBDIRS(boxm_batch)

# voxel processing library
SUBDIRS(bvpl)

# voxel processing library python interface
SUBDIRS(bvpl_batch)

# voxel processing library python interface for octrees
SUBDIRS(bvpl_octree_batch)

#SUBDIRS(btpl_satellite_tools)

SUBDIRS(bvpgl)

SUBDIRS(bapl)
