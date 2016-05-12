# brl/bbas/bnl/CMakeLists.txt
include_directories(${BRL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(bnl_sources
    bnl_parabolic_interpolator.cxx    bnl_parabolic_interpolator.h
    bnl_quadratic_interpolator.cxx    bnl_quadratic_interpolator.h
   )

vxl_add_library(LIBRARY_NAME bnl LIBRARY_SOURCES ${bnl_sources})

target_link_libraries(bnl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_algo)

add_subdirectory(algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
