# contrib/brl/bseg/boxm2/CMakeLists.txt
doxygen_add_book(contrib/brl/bseg/boxm2 "BOXM2 overview documentation")
doxygen_add_package(contrib/brl/bseg/boxm2 "BOXM2 Library")

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

SET(boxm2_sources
    boxm2_block.h                 boxm2_block.cxx
    boxm2_block_metadata.h        boxm2_block_metadata.cxx
    boxm2_data_base.h             boxm2_data_base.cxx
    boxm2_data.h                  boxm2_data.txx
    boxm2_data_traits.h
    boxm2_util.h                  boxm2_util.cxx
    boxm2_normal_albedo_array.h   boxm2_normal_albedo_array.cxx
    boxm2_feature_vector.h        boxm2_feature_vector.cxx
   )

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

  INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )

    SET(boxm2_sources ${boxm2_sources}
        boxm2_scene.h                 boxm2_scene.cxx
        boxm2_scene_parser.h          boxm2_scene_parser.cxx
        boxm2_bounding_box_parser.h   boxm2_bounding_box_parser.cxx
       )

ENDIF(EXPAT_FOUND)

AUX_SOURCE_DIRECTORY(Templates boxm2_sources)

ADD_LIBRARY(boxm2  ${boxm2_sources})
TARGET_LINK_LIBRARIES(boxm2 boxm2_basic boct brdb bsta vsph vbl_io vpgl vpgl_io vpgl_xio vpgl_algo vgl_xio vgl vnl_algo vnl vbl vil vul vsl vcl mcal bkml bvgl_algo)
IF(EXPAT_FOUND)
TARGET_LINK_LIBRARIES(boxm2 expatpp)
ENDIF(EXPAT_FOUND)

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

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#install the .h .txx and libs
INSTALL_TARGETS(/lib boxm2)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/ ${boxm2_sources})
