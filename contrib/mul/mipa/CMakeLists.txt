#
# ./mul/mipa/CMakeLists.txt
#
#   Tim Cootes
#

SET(mipa_sources
  mipa_orientation_histogram.h  mipa_orientation_histogram.txx
  mipa_sample_histo_boxes.h     mipa_sample_histo_boxes.txx
)

AUX_SOURCE_DIRECTORY(Templates mipa_sources)
ADD_LIBRARY(mipa ${mipa_sources} )
TARGET_LINK_LIBRARIES(mipa vimt vpdfl mbl)

SUBDIRS(tools)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
