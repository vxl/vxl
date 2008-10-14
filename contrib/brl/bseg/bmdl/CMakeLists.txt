# This is brl/seg/bmdl/CMakeLists.txt
 
#INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
#INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )


SET( bmdl_sources
  bmdl_classify.cxx    bmdl_classify.h
  bmdl_mesh.cxx        bmdl_mesh.h
)

#AUX_SOURCE_DIRECTORY(Templates bmdl_sources)

ADD_LIBRARY(bmdl  ${bmdl_sources})

TARGET_LINK_LIBRARIES(bmdl vil_algo imesh)

#process lib
SUBDIRS(pro)

