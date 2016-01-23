# brl/bbas/bnl/CMakeLists.txt
include_directories(${BRL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(bnl_sources
    bnl_parabolic_interpolator.cxx    bnl_parabolic_interpolator.h
    bnl_quadratic_interpolator.cxx    bnl_quadratic_interpolator.h
   )

add_library(bnl ${bnl_sources})
install_targets(/lib bnl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bnl ${bnl_sources})

target_link_libraries(bnl vnl vnl_algo)

add_subdirectory(algo)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
