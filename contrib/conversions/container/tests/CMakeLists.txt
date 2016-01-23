link_libraries( vnl testlib )

set(HAS_COOL "NO")
if(EXISTS ${vxl_SOURCE_DIR}/COOL/CMakeLists.txt )
  include_directories(${vxl_SOURCE_DIR}/COOL)
  include_directories(${vxl_SOURCE_DIR}/Numerics)
  link_libraries( TJCOOL )
  set(HAS_COOL "YES")
endif( EXISTS ${vxl_SOURCE_DIR}/COOL/CMakeLists.txt )

if(HAS_COOL)
  add_definitions(-DHAS_COOL=1)
else(HAS_COOL)
  add_definitions(-DHAS_NO_COOL=1)
endif(HAS_COOL)

add_executable( conversions_container_test_all
  test_driver.cxx

  test_container.cxx
  test_COOL_container.cxx
)

add_test( NAME conversions_test_container COMMAND $<TARGET_FILE:conversions_container_test_all> test_container )
add_test( NAME conversions_test_COOL_container COMMAND $<TARGET_FILE:conversions_container_test_all> test_COOL_container )

add_executable( conversions_container_test_include test_include.cxx )
