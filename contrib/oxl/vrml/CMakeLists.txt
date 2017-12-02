# oxl/vrml/CMakeLists.txt

doxygen_add_library(contrib/oxl/vrml
  DEPENDS core/vul
  PACKAGE contrib/oxl
  )

set(vrml_sources
 vrml_out.cxx vrml_out.h
)

vxl_add_library(LIBRARY_NAME oxl_vrml LIBRARY_SOURCES ${vrml_sources})
target_link_libraries(oxl_vrml ${VXL_LIB_PREFIX}vul)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
