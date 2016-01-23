link_libraries( vil1 vpl vul vbl testlib )

set(HAS_IMAGE "NO")
if(EXISTS ${vxl_SOURCE_DIR}/Image/CMakeLists.txt )
  include_directories(${vxl_SOURCE_DIR}/Image)
  include_directories(${vxl_SOURCE_DIR}/COOL)
  include_directories(${vxl_SOURCE_DIR}/Numerics)
  include_directories(${vxl_SOURCE_DIR}/GeneralUtility)
  link_libraries( ImageClasses Basics Numerics )
  set(HAS_IMAGE "YES")
endif(EXISTS ${vxl_SOURCE_DIR}/Image/CMakeLists.txt )

if(HAS_IMAGE)
  add_definitions(-DHAS_IMAGE=1)
else(HAS_IMAGE)
  add_definitions(-DHAS_NO_IMAGE=1)
endif(HAS_IMAGE)

add_executable( conversions_image_test_all
  test_driver.cxx

  test_vil1_vbl_conversions.cxx
  test_image_conversions.cxx
)

add_test( NAME conversions_test_vil1_vbl_conversions COMMAND $<TARGET_FILE:conversions_image_test_all> test_vil1_vbl_conversions )
add_test( NAME conversions_test_image_conversions COMMAND $<TARGET_FILE:conversions_image_test_all> test_image_conversions )

add_executable( conversions_image_test_include test_include.cxx )
