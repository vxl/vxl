# oxl/vgui/impl/glut/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindX11.cmake)

SET(vgui_IMPL_GLUT "" CACHE INTERNAL "Set if vgui has glut")

# Build here only if GLUT is available:
IF (HAS_GLUT)

INCLUDE_DIRECTORIES(${GLUT_INCLUDE_PATH})

ADD_LIBRARY(vgui_glut vgui_glut_sources)
SET(vgui_IMPL_GLUT "vgui_glut" CACHE INTERNAL "Set if vgui has glut")

SOURCE_FILES(vgui_glut_sources
 vgui_glut_adaptor.cxx    vgui_glut_adaptor.h
 vgui_glut_tag.cxx
 vgui_glut_popup_impl.cxx vgui_glut_popup_impl.h
 vgui_glut_window.cxx     vgui_glut_window.h
 vgui_glut_impl.cxx       vgui_glut_impl.h
 vgui_glut_slab.cxx       vgui_glut_slab.h
)

SOURCE_FILES(vgui_glut_sources
 menu_hack.h
)
IF (HAS_X11)
  INCLUDE_DIRECTORIES(${X11_INCLUDE_PATH})
  SOURCE_FILES(vgui_glut_sources
   menu_hack_X11.cxx
  )
ELSE (HAS_X11)
  SOURCE_FILES(vgui_glut_sources
   menu_hack_none.cxx
  )
ENDIF (HAS_X11)

AUX_SOURCE_DIRECTORY(Templates vgui_glut_sources)

ENDIF (HAS_GLUT)
