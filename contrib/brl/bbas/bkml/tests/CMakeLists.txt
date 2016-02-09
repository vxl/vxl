# This is contrib/brl/bbas/bkml/tests/CMakeLists.txt

add_executable(bkml_test_all
                test_driver.cxx
                test_bkml.cxx
              )

target_link_libraries(bkml_test_all bkml ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )
add_test( NAME bkml_test_bkml COMMAND $<TARGET_FILE:bkml_test_all> test_bkml)

add_executable(bkml_test_include test_include.cxx )
target_link_libraries( bkml_test_include bkml )
