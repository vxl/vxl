# ./gel/octree/CMakeLists.txt

ADD_EXECUTABLE(octree octree_sources)

SOURCE_FILES(octree_sources
  BaseCube.cxx
  Cube.cxx
  VoxmapImagePoints.cxx
  OctreeLevel.cxx
  VoxmapPoints.cxx
  ConvexHull.cxx
  Voxel.cxx
  main.cxx
)

AUX_SOURCE_DIRECTORY(Templates octree_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/octree/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
# This will also link with vnl, vnl-algo, vbl
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
# This will also link with png, tiff, zlib, jpeg (if available)
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
