# oxl/vgui/impl/gtk/CMakeLists.txt

INCLUDE(${MODULE_PATH}/FindGTK.cmake)

SET(vgui_IMPL_GTK "" CACHE INTERNAL "Set if vgui has GTK")

# Build here only if GTK is available:
IF (HAS_GTK)

OPTION(VGUI_USE_GTK "Should GTK be compiled into vgui?" YES)

IF(VGUI_USE_GTK)

INCLUDE_DIRECTORIES(${GTK_INCLUDE_PATH})

SET(vgui_IMPL_GTK "vgui_gtk" CACHE INTERNAL "Set if vgui has GTK")

SET(vgui_gtk_sources
 vgui_gtk.cxx             vgui_gtk.h
 vgui_gtk_adaptor.cxx     vgui_gtk_adaptor.h
 vgui_gtk_dialog_impl.cxx vgui_gtk_dialog_impl.h
 vgui_gtk_statusbar.cxx   vgui_gtk_statusbar.h
 vgui_gtk_tag.cxx
 vgui_gtk_utils.cxx       vgui_gtk_utils.h
 vgui_gtk_window.cxx      vgui_gtk_window.h
)

AUX_SOURCE_DIRECTORY(Templates vgui_gtk_sources)

ADD_LIBRARY(vgui_gtk ${vgui_gtk_sources})
TARGET_LINK_LIBRARIES(vgui_gtk vgui_base vul ${GTK_LIBRARIES})

ENDIF(VGUI_USE_GTK)

ENDIF (HAS_GTK)
