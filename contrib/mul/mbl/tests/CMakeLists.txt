#
# ./mul/mbl/tests/CMakeLists.txt
#
#   Ian Scott
#

INCLUDE( ${allvxl_SOURCE_DIR}/mul/mbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )

ADD_EXECUTABLE( test_mbl test_mbl.cxx )
ADD_TEST(mbl_test_mbl test_mbl)

ADD_EXECUTABLE( test_index_sort test_index_sort.cxx )
ADD_TEST(mbl_test_index_sort test_index_sort)

ADD_EXECUTABLE( test_lru_cache test_lru_cache.cxx )
ADD_TEST(test_lru_cache test_lru_cache)
