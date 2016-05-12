link_libraries( ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib )

set(HAS_COOL "NO")
if(EXISTS ${CMAKE_SOURCE_DIR}/COOL/CMakeLists.txt )
  include_directories(${CMAKE_SOURCE_DIR}/COOL)
  include_directories(${CMAKE_SOURCE_DIR}/Numerics)
  link_libraries( TJCOOL )
  set(HAS_COOL "YES")
endif()

set(HAS_NUMERICS "NO")
if(EXISTS ${CMAKE_SOURCE_DIR}/Numerics/CMakeLists.txt )
  include_directories(${CMAKE_SOURCE_DIR}/Numerics)
  link_libraries( Numerics iue-math )
  set(HAS_NUMERICS "YES")
endif()

if(HAS_COOL)
  add_definitions(-DHAS_COOL=1)
else()
  add_definitions(-DHAS_NO_COOL=1)
endif()

if(HAS_NUMERICS)
  add_definitions(-DHAS_NUMERICS=1)
else()
  add_definitions(-DHAS_NO_NUMERICS=1)
endif()

add_executable( conversions_math_test_all
  test_driver.cxx

  test_matrix_conversions.cxx
  test_CoolMatrix_conversions.cxx
  test_IUE_matrix_conversions.cxx
)

add_test( NAME conversions_test_matrix_conversions COMMAND $<TARGET_FILE:conversions_math_test_all> test_matrix_conversions )
add_test( NAME conversions_test_CoolMatrix_conversions COMMAND $<TARGET_FILE:conversions_math_test_all> test_CoolMatrix_conversions )
add_test( NAME conversions_test_IUE_matrix_conversions COMMAND $<TARGET_FILE:conversions_math_test_all> test_IUE_matrix_conversions )

add_executable( conversions_math_test_include test_include.cxx )
