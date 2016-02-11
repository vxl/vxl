# gel/octree/CMakeLists.txt

include_directories(${OXL_INCLUDE_DIR})

set(octree_sources
 BaseCube.h            Cube.h
 VoxmapImagePoints.cxx VoxmapImagePoints.h
 OctreeLevel.cxx       OctreeLevel.h
 VoxmapPoints.cxx      VoxmapPoints.h
 ConvexHull.cxx        ConvexHull.h
 Voxel.cxx             Voxel.h
 main.cxx
)

add_executable(octree ${octree_sources})
install_targets(/bin octree)
target_link_libraries(octree mvl ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
