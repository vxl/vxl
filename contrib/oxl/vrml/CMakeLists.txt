# oxl/vrml/CMakeLists.txt

SET(vrml_sources
 vrml_out.cxx vrml_out.h
)

ADD_LIBRARY(vrml ${vrml_sources})
TARGET_LINK_LIBRARIES(vrml vgl vnl vul)
