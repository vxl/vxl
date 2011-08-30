# contrib/brl/bseg/boxm2/CMakeLists.txt
doxygen_add_book(contrib/brl/bseg/boxm2 "BOXM2 overview documentation")
doxygen_add_package(contrib/brl/bseg/boxm2 "BOXM2 Library")

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

  INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

    SET(boxm2_sources
        boxm2_scene.h                 boxm2_scene.cxx
        boxm2_block.h                 boxm2_block.cxx
        boxm2_block_metadata.h        boxm2_block_metadata.cxx
        boxm2_data_base.h             boxm2_data_base.cxx
        boxm2_data.h                  boxm2_data.txx
        boxm2_data_traits.h
        boxm2_util.h                  boxm2_util.cxx
        boxm2_scene_parser.h          boxm2_scene_parser.cxx
       )

    AUX_SOURCE_DIRECTORY(Templates boxm2_sources)

    ADD_LIBRARY(boxm2  ${boxm2_sources})
    TARGET_LINK_LIBRARIES(boxm2 boxm2_basic boct brdb bsta vbl_io expatpp vpgl_algo vpgl vgl_xio vgl vnl_algo vnl vbl vil vul vsl vcl)

    #basic
    subdirs(basic)

    #io
    subdirs(io)

    #c plus plus library
    subdirs(cpp)

    #opencl accelerated library
    subdirs(ocl)

    #exes
    subdirs(exe)

    #pro
    subdirs(pro)

    #tableaus
    subdirs(view)

    #tests
    SUBDIRS(tests)

ENDIF(EXPAT_FOUND)
