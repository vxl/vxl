#
# ./mul/mipa/CMakeLists.txt
#
#   Tim Cootes
#

SET(mipa_sources
  mipa_orientation_histogram.h     mipa_orientation_histogram.txx
  mipa_sample_histo_boxes.h        mipa_sample_histo_boxes.txx
  mipa_vector_normaliser.h         mipa_vector_normaliser.cxx
  mipa_l2norm_vector_normaliser.h  mipa_l2norm_vector_normaliser.cxx
  mipa_identity_normaliser.h       mipa_identity_normaliser.cxx
  mipa_block_normaliser.h          mipa_block_normaliser.cxx
  mipa_ms_block_normaliser.h       mipa_ms_block_normaliser.cxx
    mipa_add_all_loaders.h         mipa_add_all_loaders.cxx
)

AUX_SOURCE_DIRECTORY(Templates mipa_sources)
ADD_LIBRARY(mipa ${mipa_sources} )
TARGET_LINK_LIBRARIES(mipa vimt vpdfl mbl)

SUBDIRS(tools)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
