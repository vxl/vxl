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

#voxel world modeling
SUBDIRS(bvxm)

#voxel world modeling batch
SUBDIRS(bvxm_batch)

# Lidar modeling (segmentation and meshing)
SUBDIRS(bmdl)

# Lidar modeling batch
SUBDIRS(bmdl_batch)

# recognition library - uses background models of bbgm and/or bvxm
SUBDIRS(brec)

