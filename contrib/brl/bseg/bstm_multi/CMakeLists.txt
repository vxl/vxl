if (HAS_GEOTIFF)
#contrib / brl / bseg / bstm_multi / CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_multi_sources
  block_data.h
  block_data_base.h
  bstm_multi_block.h bstm_multi_block.cxx
  bstm_multi_block_metadata.h bstm_multi_block_metadata.cxx
  bstm_multi_tree_util.h bstm_multi_tree_util.cxx
  bstm_multi_typedefs.h
  space_time_scene.h space_time_scene.hxx
  space_time_scene_parser.h space_time_scene_parser.hxx
  )

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if (EXPAT_FOUND)
  include_directories( ${EXPAT_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )
endif()

aux_source_directory(Templates bstm_multi_sources)

vxl_add_library(LIBRARY_NAME bstm_multi LIBRARY_SOURCES ${bstm_multi_sources})
target_link_libraries(bstm_multi bstm boxm2 bstm_basic ${VXL_LIB_PREFIX} vgl ${VXL_LIB_PREFIX} vnl_algo ${VXL_LIB_PREFIX} vnl)

if ( EXPAT_FOUND )
  target_link_libraries(bstm_multi expatpp)
endif()

if ( BUILD_TESTING )
  add_subdirectory(tests)
endif()

# #basic
subdirs(basic)

#io
subdirs(io)

#c plus plus library
# Missing functions that prevent building with shared Libs:
# bstm_multi_cpp_bstm_to_bstm_multi_scene_process
# NOT SUPPORTED OR TESTED subdirs(cpp)  -- Does not compile with shared libs due to many missing function definitions.

# #opencl section
#subdirs(ocl)

# #view section
#subdirs(view)

# #util
#subdirs(util)

#processes
# Missing functions that prevent building with shared Libs:
#  bstm_multi_bundle_to_scene_process,
#  bstm_multi_boxm2_scene_to_bstm_multi_process,
#  bstm_multi_scene_lvcs_process,
#  bstm_multi_scene_bbox_process,
# NOT SUPPORTED OR TESTED subdirs(pro)  -- Does not compile with shared libs due to many missing function definitions.

else()
  message(STATUS "Skipping contrib/brl/bseg/bstm_multi: requires geotiff")
endif()
