# oxl/vgui/impl/glut/CMakeLists.txt

INCLUDE(${MODULE_PATH}/FindGLUT.cmake)
INCLUDE(${MODULE_PATH}/FindX11.cmake)

SET(vgui_IMPL_GLUT "" CACHE INTERNAL "Set if vgui has glut")

# Build here only if GLUT is available:
IF (HAS_GLUT)

OPTION(VGUI_USE_GLUT "Should Glut be compiled into vgui?" YES)

IF(VGUI_USE_GLUT)

INCLUDE_DIRECTORIES(${GLUT_INCLUDE_PATH})

SET(vgui_IMPL_GLUT "vgui_glut" CACHE INTERNAL "Set if vgui has glut")

SET(vgui_glut_sources
 vgui_glut_adaptor.cxx    vgui_glut_adaptor.h
 vgui_glut_tag.cxx
 vgui_glut_popup_impl.cxx vgui_glut_popup_impl.h
 vgui_glut_window.cxx     vgui_glut_window.h
 vgui_glut_impl.cxx       vgui_glut_impl.h
 vgui_glut_slab.cxx       vgui_glut_slab.h
)

SET(vgui_glut_sources ${vgui_glut_sources}
 menu_hack.h
)
IF (HAS_X11)
  INCLUDE_DIRECTORIES(${X11_INCLUDE_PATH})
  SET(vgui_glut_sources ${vgui_glut_sources}
   menu_hack_X11.cxx
  )
ELSE (HAS_X11)
  SET(vgui_glut_sources ${vgui_glut_sources}
   menu_hack_none.cxx
  )
ENDIF (HAS_X11)

AUX_SOURCE_DIRECTORY(Templates vgui_glut_sources)

ADD_LIBRARY(vgui_glut ${vgui_glut_sources})
TARGET_LINK_LIBRARIES(vgui_glut vgui_base ${GLUT_LIBRARIES})
IF (HAS_X11)
  TARGET_LINK_LIBRARIES(vgui_glut ${X11_LIBRARIES})
ENDIF (HAS_X11)

ENDIF(VGUI_USE_GLUT)

ENDIF (HAS_GLUT)
