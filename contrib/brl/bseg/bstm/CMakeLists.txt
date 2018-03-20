if (HAS_GEOTIFF)
# contrib/brl/bseg/bstm/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_sources bstm_block_metadata.h bstm_block_metadata.cxx
                 bstm_time_tree.h bstm_time_tree.cxx
                 bstm_scene.h bstm_scene.cxx
                 bstm_scene_parser.h bstm_scene_parser.cxx
                 bstm_data_base.h bstm_data_base.cxx
                 bstm_data.h
                 bstm_block.h bstm_block.cxx
                 bstm_data_traits.h
                 bstm_time_block.h bstm_time_block.cxx
                 bstm_util.cxx bstm_util.h)

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${EXPAT_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )
endif()


aux_source_directory(Templates bstm_sources)

vxl_add_library(LIBRARY_NAME bstm LIBRARY_SOURCES  ${bstm_sources})
target_link_libraries(bstm boxm2 bstm_basic ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl)

if(EXPAT_FOUND)
  target_link_libraries(bstm expatpp)
endif()

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#basic
subdirs(basic)

#io
subdirs(io)

#c plus plus library
subdirs(cpp)

#opencl section
subdirs(ocl)

#view section
# this is causing confusing compiler errors with Visual Studio 2013 Professional;
# also had to comment out ocl/exe in the ocl CMakeLists.txt
subdirs(view)

#util
subdirs(util)

#processes
subdirs(pro)
else()
  message(STATUS "Skipping contrib/brl/bseg/bstm: requires geotiff")
endif()
