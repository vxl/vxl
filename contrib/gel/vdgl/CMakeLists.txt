#
# ./gel/vdgl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


ADD_LIBRARY(vdgl vdgl_sources)

SOURCE_FILES(vdgl_sources
vdgl_OrthogRegress.cxx   vdgl_fit_line.cxx
vdgl_digital_curve.cxx   vdgl_interpolator.cxx
vdgl_digital_region.cxx  vdgl_interpolator_cubic.cxx
vdgl_edgel.cxx           vdgl_interpolator_linear.cxx
vdgl_edgel_chain.cxx     vdgl_ortho_regress.cxx
)

