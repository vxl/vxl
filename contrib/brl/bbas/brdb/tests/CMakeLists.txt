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

ADD_TEST( brdb_test_value             brdb_test_all test_value )
ADD_TEST( brdb_test_tuple             brdb_test_all test_tuple )
ADD_TEST( brdb_test_relation          brdb_test_all test_relation )
ADD_TEST( brdb_test_database          brdb_test_all test_database )
#ADD_TEST( brdb_test_database_manager  brdb_test_all test_database_manager )
ADD_TEST( brdb_test_query             brdb_test_all test_query )

AUX_SOURCE_DIRECTORY(Templates brdb_test_value)

ADD_EXECUTABLE( brdb_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brdb_test_include brdb )
