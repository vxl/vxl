# mul/msm/CMakeLists.txt
#
#   Tim Cootes
#

doxygen_add_library(contrib/mul/msm/utils
  DEPENDS contrib/mul/msm
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's SM Library"
)

SET(msm_utils_sources
  msm_draw_shape_to_eps.h       msm_draw_shape_to_eps.cxx
  msm_shape_mode_view.h         msm_shape_mode_view.cxx
)

# AUX_SOURCE_DIRECTORY(Templates msm_utils_sources)
ADD_LIBRARY(msm_utils ${msm_utils_sources} )
TARGET_LINK_LIBRARIES(msm_utils msm vgl)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
