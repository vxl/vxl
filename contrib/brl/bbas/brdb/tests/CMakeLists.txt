# brl/bbas/brdb/tests/CMakeLists.txt

ADD_EXECUTABLE( brdb_test_all
  test_driver.cxx
  sample_database.cxx    sample_database.h
  test_value.cxx
  test_tuple.cxx
  test_relation.cxx
  test_database.cxx
#  test_database_manager.cxx
  test_query.cxx
)

TARGET_LINK_LIBRARIES( brdb_test_all brdb testlib )

ADD_TEST( NAME brdb_test_value COMMAND $<TARGET_FILE:brdb_test_all> test_value )
ADD_TEST( NAME brdb_test_tuple COMMAND $<TARGET_FILE:brdb_test_all> test_tuple )
if( BUILD_FOR_VXL_DASHBOARD )
## This test always fails in travis builds
ADD_TEST( NAME brdb_test_relation COMMAND $<TARGET_FILE:brdb_test_all> test_relation )
endif()
ADD_TEST( NAME brdb_test_database COMMAND $<TARGET_FILE:brdb_test_all> test_database )
#ADD_TEST( NAME brdb_test_database_manager COMMAND $<TARGET_FILE:brdb_test_all> test_database_manager )
ADD_TEST( NAME brdb_test_query COMMAND $<TARGET_FILE:brdb_test_all> test_query )

AUX_SOURCE_DIRECTORY(Templates brdb_test_value)

ADD_EXECUTABLE( brdb_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brdb_test_include brdb )
ADD_EXECUTABLE( brdb_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( brdb_test_template_include brdb )
