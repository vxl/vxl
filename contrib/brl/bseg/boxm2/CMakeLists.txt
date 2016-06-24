# contrib/brl/bseg/boxm2/CMakeLists.txt
if(HAS_GEOTIFF)
doxygen_add_book(contrib/brl/bseg/boxm2 "BOXM2 overview documentation")
doxygen_add_package(contrib/brl/bseg/boxm2 "BOXM2 Library")

include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${MUL_INCLUDE_DIR} )

set(boxm2_sources
    boxm2_export.h
    boxm2_block.h                 boxm2_block.cxx
    boxm2_block_metadata.h        boxm2_block_metadata.cxx
    boxm2_data_base.h             boxm2_data_base.cxx
    boxm2_data.h                  boxm2_data.hxx
    boxm2_data_traits.h
    boxm2_util.h                  boxm2_util.cxx
    boxm2_normal_albedo_array.h   boxm2_normal_albedo_array.cxx
    boxm2_feature_vector.h        boxm2_feature_vector.cxx
    boxm2_blocks_vis_graph.h          boxm2_blocks_vis_graph.cxx
   )

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

  include_directories( ${EXPAT_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

    set(boxm2_sources ${boxm2_sources}
        boxm2_scene.h                 boxm2_scene.cxx
        boxm2_scene_parser.h          boxm2_scene_parser.cxx
        boxm2_bounding_box_parser.h   boxm2_bounding_box_parser.cxx
       )

endif()

aux_source_directory(Templates boxm2_sources)

vxl_add_library(LIBRARY_NAME boxm2 LIBRARY_SOURCES  ${boxm2_sources})
target_link_libraries(boxm2 boxm2_basic boct brdb bsta vsph ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vpgl_xio ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl mcal bkml bvgl_algo)
if(EXPAT_FOUND)
target_link_libraries(boxm2 expatpp)
endif()

#basic
subdirs(basic)

#io
subdirs(io)

#c plus plus library
subdirs(cpp)

#opencl accelerated library
subdirs(ocl)

#reg lib
subdirs(class)

#registration Library
subdirs(reg)

#pro
subdirs(pro)

#tableaus
subdirs(view)

#util
subdirs(util)

# volumetric matching Library
subdirs(volm)

# vector flow library
subdirs(vecf)

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#install the .h .hxx and libs
else()
  message(STATUS "Skipping contrib/brl/bseg/boxm2: requires geotiff")
endif()
