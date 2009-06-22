# This is brl/bseg/makefile
# main package for segmentation-related code and applications

# float image operations
SUBDIRS += brip

# segmentation feature detection library
SUBDIRS += sdet

# video processors
SUBDIRS += vpro

# tracking
SUBDIRS += strk

# markov random fields - This library is deprecated
#SUBDIRS += bmrf

# misc
#SUBDIRS += bcons

# segmentation view library
SUBDIRS += segv

# segmentation application executables
SUBDIRS += sbin

# background modeling
SUBDIRS += bbgm

# background modeling python interface
SUBDIRS += bbgm_batch

#voxel world modeling
SUBDIRS += bvxm

#voxel world modeling batch
SUBDIRS += bvxm_batch

# Lidar modeling (segmentation and meshing)
SUBDIRS += bmdl

# Lidar modeling batch
SUBDIRS += bmdl_batch

# recognition library - uses background models of bbgm and/or bvxm
SUBDIRS += brec

# octree
SUBDIRS += boct

# octree based modeling 
SUBDIRS += boxm

# octree based modeling python interface
SUBDIRS += boxm_batch

# voxel processing library
SUBDIRS += bvpl
SUBDIRS += bvpl_batch

################################
include ${VXLROOT}/config/top.mk
################################
