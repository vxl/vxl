# ./oxl/vgui/impl/glut/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindX11.cmake)

# Build here only if GLUT is available:
IF(HAS_GLUT)

ADD_LIBRARY(vgui_glut vgui_glut_sources)

SOURCE_FILES(vgui_glut_sources
  vgui_glut_adaptor.cxx
  vgui_glut_tag.cxx
  vgui_glut_popup_impl.cxx
  vgui_glut_window.cxx
  vgui_glut_impl.cxx
  vgui_glut_slab.cxx
)

IF(HAS_X11)
  SOURCE_FILES(vgui_glut_sources
    menu_hack_X11.cxx
  )
ELSE(HAS_X11)
  SOURCE_FILES(vgui_glut_sources
    menu_hack_none.cxx
  )
ENDIF(HAS_X11)

AUX_SOURCE_DIRECTORY(Templates vgui_glut_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/vgui/impl/glut/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

ENDIF(HAS_GLUT)
