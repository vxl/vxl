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
BigSparseArray3D
OctreeLevel
VoxmapPoints
BitArray3D
ConvexHull
Voxel
main
)

AUX_SOURCE_DIRECTORY(Templates octree_sources)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)

LINK_LIBRARIES( 
vcl vul vbl vnl vnl_algo vil png tiff zlib jpeg netlib ge_cnl mvl vgl
)
