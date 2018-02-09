# brl/bseg/CMakeLists.txt
# main package for segmentation-related code and applications
# Mutual dependencies: brip --> sdet --> (vpro,segv)

# float image operations
add_subdirectory(brip)

# segmentation feature detection library
add_subdirectory(sdet)

# segmentation view library
add_subdirectory(segv)

# segmentation application executables
add_subdirectory(sbin)

# background modeling
add_subdirectory(bbgm)

# background modeling python interface
add_subdirectory(bbgm_batch)

# voxel world modeling
add_subdirectory(bvxm)

# voxel world modeling python interface
add_subdirectory(bvxm_batch)

# Lidar modeling (segmentation and meshing)
add_subdirectory(bmdl)

# Lidar modeling python interface
add_subdirectory(bmdl_batch)

# recognition library - uses background models of bbgm and/or bvxm
add_subdirectory(brec)

# recognition library - python interface
add_subdirectory(brec_batch)

# octree
add_subdirectory(boct)

# octree based modeling
add_subdirectory(boxm)

# updated, gpu compatible octree based modeling
add_subdirectory(boxm2)

# updated, gpu compatible octree based modeling
add_subdirectory(boxm2_batch)

# octree based modeling python interface
add_subdirectory(boxm_batch)

# semi-global matching
add_subdirectory(bsgm)

# voxel processing library
add_subdirectory(bvpl)

# voxel processing library python interface
add_subdirectory(bvpl_batch)

# voxel processing library python interface for octrees
add_subdirectory(bvpl_octree_batch)

# Brown affine patch library, SIFT keypoint & descriptor
add_subdirectory(bapl)

#the bridge lib to convert between boxm and boxm2 scenes
add_subdirectory(boxm_bridge)

#multi gpu library
add_subdirectory(boxm2_multi)

#space-time modeling
add_subdirectory(bstm)
add_subdirectory(bstm_batch)
add_subdirectory(bstm_multi)

#the bridge lib to convert between bvxm and boxm2 scenes
add_subdirectory(bvxm_bridge)

#event trigger library
add_subdirectory(betr)
add_subdirectory(betr_batch)

# VSI conflation tools
add_subdirectory(vcon)

# appearance modeling library
add_subdirectory(baml)
