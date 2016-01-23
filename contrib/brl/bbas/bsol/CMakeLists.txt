# Additional functions to support vsol
# brl/bbas/bsol/CMakeLists.txt

doxygen_add_library(contrib/brl/bbas/bsol
  DEPENDS contrib/gel/vsol core/vul core/vbl
  PACKAGE contrib/brl
  )

include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bbas )
include_directories(${GEL_INCLUDE_DIR} )

set(bsol_sources
    dll.h

    bsol_hough_line_index.cxx   bsol_hough_line_index.h   bsol_hough_line_index_sptr.h
    bsol_algs.cxx               bsol_algs.h
    bsol_distance_histogram.cxx bsol_distance_histogram.h
    bsol_intrinsic_curve_2d.cxx bsol_intrinsic_curve_2d.h bsol_intrinsic_curve_2d_sptr.h
    bsol_intrinsic_curve_3d.cxx bsol_intrinsic_curve_3d.h bsol_intrinsic_curve_3d_sptr.h
    bsol_point_index_2d.cxx     bsol_point_index_2d.h
    bsol_point_index_3d.cxx     bsol_point_index_3d.h
)
aux_source_directory(Templates bsol_sources)

add_library(bsol ${bsol_sources})
install_targets(/lib bsol)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bsol ${bsol_sources})
target_link_libraries(bsol vsol vgl_algo vgl vnl vbl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
