# gel/vdgl/CMakeLists.txt

doxygen_add_library(contrib/gel/vdgl
  DEPENDS contrib/gel/vsol
  PACKAGE contrib/gel
  DESCRIPTION "Digital Geometry"
  )

set(vdgl_sources
 vdgl_digital_curve.cxx       vdgl_digital_curve.h     vdgl_digital_curve_sptr.h
 vdgl_digital_region.cxx      vdgl_digital_region.h    vdgl_digital_region_sptr.h
 vdgl_edgel.cxx               vdgl_edgel.h
 vdgl_edgel_chain.cxx         vdgl_edgel_chain.h       vdgl_edgel_chain_sptr.h
 vdgl_fit_line.cxx            vdgl_fit_line.h
 vdgl_fit_lines.cxx           vdgl_fit_lines.h
 vdgl_fit_lines_params.cxx    vdgl_fit_lines_params.h
 vdgl_interpolator.cxx        vdgl_interpolator.h      vdgl_interpolator_sptr.h
 vdgl_interpolator_cubic.cxx  vdgl_interpolator_cubic.h
 vdgl_interpolator_linear.cxx vdgl_interpolator_linear.h
 vdgl_ortho_regress.cxx       vdgl_ortho_regress.h     vdgl_OrthogRegress.h
)

aux_source_directory(Templates vdgl_sources)

add_library(vdgl ${vdgl_sources})
install_targets(/lib vdgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/vdgl ${vdgl_sources})
target_link_libraries(vdgl vsol vgl_algo vgl vnl_algo vnl vbl_io vbl vsl vul vcl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
