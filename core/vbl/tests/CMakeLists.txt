#
# ./vxl/vbl/tests/CMakeLists.txt
#
#   ian.m.scott@stud.man.ac.uk
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )

ADD_LIBRARY(vbl_test vbl_test_sources)

SOURCE_FILES( vbl_test_sources
vbl_test_classes.cxx
)
AUX_SOURCE_DIRECTORY(Templates vbl_test_sources)

LINK_LIBRARIES( vbl_test )
ADD_EXECUTABLE( vbl_test_all vbl_test_all.cxx )
ADD_EXECUTABLE( vbl_test_array vbl_test_array.cxx )
ADD_EXECUTABLE( vbl_test_sparse_array_2d vbl_test_sparse_array_2d.cxx )
ADD_EXECUTABLE( vbl_test_sparse_array_3d vbl_test_sparse_array_3d.cxx )
ADD_EXECUTABLE( vbl_test_smart_ptr vbl_test_smart_ptr.cxx ) 
