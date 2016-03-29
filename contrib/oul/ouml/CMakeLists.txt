# ./oul/ouml/CMakeLists.txt

doxygen_add_library(contrib/oul/ouml
  DEPENDS core/vnl core/vil1 core/vpl
  PACKAGE contrib/oul
  DESCRIPTION "Miscellaneous things"
  )

set(ouml_sources
  eigenfaces.h       eigenfaces.cxx
  image_database.h   image_database.cxx
  polygon_mesh.h     polygon_mesh.cxx
  CardinalSpline.h   CardinalSpline.cxx

  image_convert.h    image_convert.hxx
  image_ops.h        image_ops.hxx
  io.h               io.hxx

  inlines.h
)

vxl_add_library(LIBRARY_NAME ouml LIBRARY_SOURCES ${ouml_sources})
target_link_libraries( ouml ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vpl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
