# gel/octree/CMakeLists.txt

ADD_EXECUTABLE(octree octree_sources)

SOURCE_FILES(octree_sources
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

INCLUDE(${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt)
# This will also link with vnl, vnl-algo, vbl
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt)
# This will also link with png, tiff, zlib, jpeg (if available)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt)
INCLUDE(${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt)
