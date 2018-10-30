#
# ./mul/mipa/CMakeLists.txt
#
#   Tim Cootes
#

doxygen_add_library(contrib/mul/mipa
  DEPENDS contrib/mul/vimt contrib/mul/vpdfl contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's Image Processing Library"
)

set(mipa_sources
  mipa_orientation_histogram.h     mipa_orientation_histogram.hxx
  mipa_sample_histo_boxes.h        mipa_sample_histo_boxes.hxx
  mipa_vector_normaliser.h         mipa_vector_normaliser.cxx
  mipa_l2norm_vector_normaliser.h  mipa_l2norm_vector_normaliser.cxx
  mipa_identity_normaliser.h       mipa_identity_normaliser.cxx
  mipa_block_normaliser.h          mipa_block_normaliser.cxx
  mipa_ms_block_normaliser.h       mipa_ms_block_normaliser.cxx
  mipa_add_all_loaders.h           mipa_add_all_loaders.cxx
)

aux_source_directory(Templates mipa_sources)
vxl_add_library(LIBRARY_NAME mipa LIBRARY_SOURCES ${mipa_sources} )
target_link_libraries(mipa vimt vpdfl)

if(VXL_BUILD_MUL_TOOLS)
  add_subdirectory(tools)
endif()

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
