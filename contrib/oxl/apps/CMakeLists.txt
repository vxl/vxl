# ./oxl/apps/CMakeLists.txt

INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)

# Build here only if OpenGL is available:
IF(HAS_OPENGL)

SUBDIRS( vxl_vrml_viewer glmovie )

ENDIF(HAS_OPENGL)
