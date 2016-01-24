# Brown digital geometry library
# brl/bbas/bdgl/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bdgl
  DEPENDS contrib/gel/vtol contrib/oxl/mvl contrib/mul/mbl core/vbl
  PACKAGE contrib/brl
)

include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bbas )
include_directories(${GEL_INCLUDE_DIR} )

set(bdgl_sources
    bdgl_export.h

    bdgl_curve_algs.cxx   bdgl_curve_algs.h
    bdgl_region_algs.cxx  bdgl_region_algs.h
    bdgl_curve_region.cxx bdgl_curve_region.h
    bdgl_peano_curve.cxx  bdgl_peano_curve.h
)

add_library(bdgl ${bdgl_sources})
install_targets(/lib bdgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bdgl ${bdgl_sources})

target_link_libraries(bdgl vdgl vsol vgl_algo vgl vnl_algo vnl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
