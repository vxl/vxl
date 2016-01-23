# vpgl/ihog/tests/CMakeLists.txt

add_executable( ihog_test_all
  test_driver.cxx
  test_minimizer.cxx
)

target_link_libraries( ihog_test_all ihog testlib vul vnl vgl vil)
add_test( NAME ihog_test_minimizer COMMAND $<TARGET_FILE:ihog_test_all> test_minimizer )

add_executable( ihog_test_include test_include.cxx )
target_link_libraries( ihog_test_include ihog)
