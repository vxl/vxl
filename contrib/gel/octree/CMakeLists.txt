#
# ./gel/octree/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_EXECUTABLE(octree octree_sources)

SOURCE_FILES(octree_sources
BaseCube
Cube
VoxmapImagePoints
OctreeLevel
VoxmapPoints
ConvexHull
Voxel
main
)

AUX_SOURCE_DIRECTORY(Templates octree_sources)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
# This will link with mvl, vgl, vnl, vnl-algo, vbl
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
# This will link with vil, png, tiff, zlib, jpeg
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
