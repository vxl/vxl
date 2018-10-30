# brl/bbas/brdb/tests/CMakeLists.txt

add_executable( brdb_test_all
  test_driver.cxx
  sample_database.cxx    sample_database.h
  test_value.cxx
  test_tuple.cxx
  test_relation.cxx
  test_database.cxx
#  test_database_manager.cxx
  test_query.cxx
)

target_link_libraries( brdb_test_all brdb ${VXL_LIB_PREFIX}testlib )

add_test( NAME brdb_test_value COMMAND $<TARGET_FILE:brdb_test_all> test_value )
add_test( NAME brdb_test_tuple COMMAND $<TARGET_FILE:brdb_test_all> test_tuple )

add_test( NAME brdb_test_database COMMAND $<TARGET_FILE:brdb_test_all> test_database )
#add_test( NAME brdb_test_database_manager COMMAND $<TARGET_FILE:brdb_test_all> test_database_manager )
add_test( NAME brdb_test_query COMMAND $<TARGET_FILE:brdb_test_all> test_query )

aux_source_directory(Templates brdb_test_value)

add_executable( brdb_test_include test_include.cxx )
target_link_libraries( brdb_test_include brdb )
add_executable( brdb_test_template_include test_template_include.cxx )
target_link_libraries( brdb_test_template_include brdb )
