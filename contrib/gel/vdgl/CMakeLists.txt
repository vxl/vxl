# ./gel/vdgl/CMakeLists.txt
#

ADD_LIBRARY(vdgl vdgl_sources)

SOURCE_FILES(vdgl_sources
  vdgl_OrthogRegress   vdgl_fit_line
  vdgl_digital_curve   vdgl_interpolator
  vdgl_digital_region  vdgl_interpolator_cubic
  vdgl_edgel           vdgl_interpolator_linear
  vdgl_edgel_chain     vdgl_ortho_regress
)

AUX_SOURCE_DIRECTORY(Templates vdgl_sources)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
