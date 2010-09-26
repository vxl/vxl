# This is brl/seg/bmdl/CMakeLists.txt

doxygen_add_library(contrib/brl/bseg/bmdl
  DEPENDS contrib/brl/bbas/imesh core/vil
  PACKAGE contrib/brl
  DESCRIPTION "Brown Lidar Mesh Modeling Library"
  )

ADD_DEFINITIONS(-DDEBUG_MEMORY=1)

SET( bmdl_sources
  bmdl_classify.txx    bmdl_classify.h
  bmdl_mesh.cxx        bmdl_mesh.h
)

AUX_SOURCE_DIRECTORY(Templates bmdl_sources)

ADD_LIBRARY(bmdl  ${bmdl_sources})

TARGET_LINK_LIBRARIES(bmdl vil_algo imesh vgl_algo vgl vnl vil)

#process lib
SUBDIRS(pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
