#
# ./gel/vdgl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#


ADD_LIBRARY(vdgl vdgl_sources)

SOURCE_FILES(vdgl_sources
vdgl_OrthogRegress   vdgl_fit_line
vdgl_digital_curve   vdgl_interpolator
vdgl_digital_region  vdgl_interpolator_cubic
vdgl_edgel           vdgl_interpolator_linear
vdgl_edgel_chain     vdgl_ortho_regress
)

