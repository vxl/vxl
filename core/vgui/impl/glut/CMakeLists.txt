#
# ./oxl/vgui/impl/glut/CMakeLists.txt
#
#   perera@cs.rpi.edu
#

ADD_LIBRARY(vgui_glut vgui_glut_sources)

SOURCE_FILES(vgui_glut_sources
vgui_glut_adaptor
vgui_glut_tag
menu_hack_none
vgui_glut_popup_impl
vgui_glut_window
vgui_glut
vgui_glut_slab
)

IF(X11_LIBRARY)
  INCLUDE_DIRECTORIES( ${X11_INCLUDE_PATH} )
  SOURCE_FILES(vgui_glut_sources
    menu_hack_X11
  )
ENDIF(X11_LIBRARY)

INCLUDE_DIRECTORIES(${GLUT_INCLUDE_PATH})
