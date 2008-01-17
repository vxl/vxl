#
# ./mul/msm/CMakeLists.txt
#
#   Tim Cootes
#

SET(msm_sources
#  msm_add_all_loaders.h       msm_add_all_loaders.cxx
  msm_points.h                msm_points.cxx
)

AUX_SOURCE_DIRECTORY(Templates msm_sources)
ADD_LIBRARY(msm ${msm_sources} )
TARGET_LINK_LIBRARIES(msm vimt vpdfl mbl)

#SUBDIRS(tools)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
