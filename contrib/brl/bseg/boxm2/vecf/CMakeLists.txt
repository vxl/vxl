# library to support vector field operations

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
SET(boxm2_vecf_sources
    boxm2_vecf_eye_params.h
    boxm2_vecf_eye_scene.h                  boxm2_vecf_eye_scene.cxx
    boxm2_vecf_head_model.h                 boxm2_vecf_head_model.cxx
    boxm2_vecf_composite_head_parameters.h
    boxm2_vecf_composite_head_model.h       boxm2_vecf_composite_head_model.cxx
)

ADD_LIBRARY(boxm2_vecf  ${boxm2_vecf_sources})
TARGET_LINK_LIBRARIES(boxm2_vecf boxm2_io  boxm2_ocl boxm2_ocl_algo bocl brip baio vpgl vnl vidl vil_algo vil vgl_algo vgl_xio vgl vbl vul vsl vcl depth_map)

# OpenCL support code
SUBDIRS(ocl)


#processes
#subdirs(pro)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests) 
ENDIF( BUILD_TESTING )


