# oxl/vrml/CMakeLists.txt

SOURCE_FILES(vrml_sources
 vrml_out.cxx vrml_out.h
)

ADD_LIBRARY(vrml vrml_sources)
TARGET_LINK_LIBRARIES(vrml vbl)
