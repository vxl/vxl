# ./oul/ouml/CMakeLists.txt

SET(ouml_sources
  eigenfaces.h       eigenfaces.cxx
  image_database.h   image_database.cxx
  polygon_mesh.h     polygon_mesh.cxx

  image_ops.h        image_ops.txx
  io.h               io.txx

  inlines.h

  image_convert.txx
)

ADD_LIBRARY(ouml ${ouml_sources})
TARGET_LINK_LIBRARIES( ouml vnl_algo vil vpl )
