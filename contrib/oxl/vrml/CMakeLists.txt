# oxl/vrml/CMakeLists.txt

doxygen_add_library(contrib/oxl/vrml
  DEPENDS core/vul
  PACKAGE contrib/oxl
  )

SET(vrml_sources
 vrml_out.cxx vrml_out.h
)

ADD_LIBRARY(oxl_vrml ${vrml_sources})
TARGET_LINK_LIBRARIES(oxl_vrml vul)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
