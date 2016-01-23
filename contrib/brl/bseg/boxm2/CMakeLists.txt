# contrib/brl/bseg/boxm2/CMakeLists.txt
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
    boxm2_data.h                  boxm2_data.txx
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

add_library(boxm2  ${boxm2_sources})
target_link_libraries(boxm2 boxm2_basic boct brdb bsta vsph vbl_io vpgl vpgl_io vpgl_xio vpgl_algo vgl_xio vgl vnl_algo vnl vbl vil vul vsl vcl mcal bkml bvgl_algo)
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

#install the .h .txx and libs
install_targets(/lib boxm2)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/ ${boxm2_sources})
