# ./oul/ouml/CMakeLists.txt

ADD_LIBRARY(ouml ouml_sources)

SOURCE_FILES(ouml_sources
  eigenfaces.h       eigenfaces.cxx
  image_database.h   image_database.cxx
  polygon_mesh.h     polygon_mesh.cxx

  image_ops.h        image_ops.txx
  io.h               io.txx

  inlines.h

  image_convert.txx
)
