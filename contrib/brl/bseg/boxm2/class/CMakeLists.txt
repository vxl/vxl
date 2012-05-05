# contrib/brl/bseg/boxm2/util/CMakeLists.txt
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(boxm2_class_sources
    boxm2_class_generate_samples.h    boxm2_class_generate_samples.cxx
   )

ADD_LIBRARY(boxm2_class  ${boxm2_class_sources})
TARGET_LINK_LIBRARIES(boxm2_class boxm2 bvgl vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl)

#install the .h .txx and libs
INSTALL_TARGETS(/lib boxm2_class)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2_class/ ${boxm2_class_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS(exe)
