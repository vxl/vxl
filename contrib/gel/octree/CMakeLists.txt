# gel/octree/CMakeLists.txt

INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

SET(octree_sources
 BaseCube.cxx          BaseCube.h
 Cube.cxx              Cube.h
 VoxmapImagePoints.cxx VoxmapImagePoints.h
 OctreeLevel.cxx       OctreeLevel.h
 VoxmapPoints.cxx      VoxmapPoints.h
 ConvexHull.cxx        ConvexHull.h
 Voxel.cxx             Voxel.h
 main.cxx
)

AUX_SOURCE_DIRECTORY(Templates octree_sources)

ADD_EXECUTABLE(octree ${octree_sources})
TARGET_LINK_LIBRARIES(octree mvl vil vgl vul)
