#
# ./oxl/vgui/impl/glut/CMakeLists.txt
#
#   perera@cs.rpi.edu
#

ADD_LIBRARY(vgui_glut vgui_glut_sources)

SOURCE_FILES(vgui_glut_sources
vgui_glut_adaptor
vgui_glut_tag
vgui_glut_popup_impl
vgui_glut_window
vgui_glut
vgui_glut_slab
)

INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindX11.cmake )

IF(HAS_X11)
  SOURCE_FILES(vgui_glut_sources
    menu_hack_X11
  )
ELSE(HAS_X11)
  SOURCE_FILES(vgui_glut_sources
    menu_hack_none
  )
ENDIF(HAS_X11)
