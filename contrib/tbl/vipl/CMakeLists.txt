#
# ./tbl/vipl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


ADD_LIBRARY(vipl vipl_sources)

SOURCE_FILES(vipl_sources
)

AUX_SOURCE_DIRECTORY(Templates vipl_sources)

AUX_SOURCE_DIRECTORY(vipl_with_vbl_array_2d/Templates vipl_sources)
AUX_SOURCE_DIRECTORY(vipl_with_vnl_matrix/Templates vipl_sources)
