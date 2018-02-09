# brl/bseg/baml/tests/CMakeLists.txt

add_executable( baml_test_all
  test_driver.cxx
  test_appearance.cxx
  test_dem_appear.cxx
)

target_link_libraries( baml_test_all baml ${VXL_LIB_PREFIX}imesh ${VXL_LIB_PREFIX}brip ${VXL_LIB_PREFIX}vsol ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib)

# add_test( NAME baml_test_appearance COMMAND $<TARGET_FILE:baml_test_all> test_appearance )
# add_test( NAME baml_test_dem_appear COMMAND $<TARGET_FILE:baml_test_all> test_dem_appear )
