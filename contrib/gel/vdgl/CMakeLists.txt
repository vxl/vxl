# gel/vdgl/CMakeLists.txt

SET(vdgl_sources
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

AUX_SOURCE_DIRECTORY(Templates vdgl_sources)

ADD_LIBRARY(vdgl ${vdgl_sources})
TARGET_LINK_LIBRARIES(vdgl vsol vnl_algo vnl vgl vbl_io vbl vsl vul vcl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
