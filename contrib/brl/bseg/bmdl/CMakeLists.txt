# This is brl/seg/bmdl/CMakeLists.txt

ADD_DEFINITIONS(-DDEBUG_MEMORY=1)

SET( bmdl_sources
  bmdl_classify.txx    bmdl_classify.h   
  bmdl_mesh.cxx        bmdl_mesh.h
)

AUX_SOURCE_DIRECTORY(Templates bmdl_sources)

ADD_LIBRARY(bmdl  ${bmdl_sources})

TARGET_LINK_LIBRARIES(bmdl vil_algo imesh vgl vgl_algo)

#process lib
SUBDIRS(pro)

