# oxl/vrml/CMakeLists.txt

doxygen_add_library(contrib/oxl/vrml
  DEPENDS core/vul
  PACKAGE contrib/oxl
  )

set(vrml_sources
 vrml_out.cxx vrml_out.h
)

add_library(oxl_vrml ${vrml_sources})
target_link_libraries(oxl_vrml vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
