# gel/jvid/CMakeLists.txt

INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake )
INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindMPEG.cmake )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/oxl )

# Build here only if OpenGL (and thus vgui) is available:
IF( HAS_OPENGL )

  SUBDIRS( bin )

  ADD_LIBRARY( jvid jvid_sources )

  SOURCE_FILES( jvid_sources
    jvx_manager.cxx jvx_manager.h
  )

  AUX_SOURCE_DIRECTORY( Templates jvid_sources )

ENDIF( HAS_OPENGL )
