# ./oul/ouml/CMakeLists.txt

doxygen_add_library(contrib/oul/ouml
  DEPENDS core/vnl core/vil1 core/vpl
  PACKAGE contrib/oul
  DESCRIPTION "Miscellaneous things"
  )

SET(ouml_sources
  eigenfaces.h       eigenfaces.cxx
  image_database.h   image_database.cxx
  polygon_mesh.h     polygon_mesh.cxx
  CardinalSpline.h   CardinalSpline.cxx

  image_convert.h    image_convert.txx
  image_ops.h        image_ops.txx
  io.h               io.txx

  inlines.h
)

ADD_LIBRARY(ouml ${ouml_sources})
INSTALL_TARGETS(/lib ouml)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/oul/ouml ${ouml_sources})
TARGET_LINK_LIBRARIES( ouml vgl vnl_algo vnl_io vnl vil1 vsl vpl )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
