# oxl/vgui/impl/gtk/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindGTK.cmake)

SET(vgui_IMPL_GTK "" CACHE INTERNAL "Set if vgui has GTK")

# Build here only if GTK is available:
IF (HAS_GTK)
INCLUDE_DIRECTORIES(${GTK_INCLUDE_PATH})

ADD_LIBRARY(vgui_gtk vgui_gtk_sources)
SET(vgui_IMPL_GTK "vgui_gtk" CACHE INTERNAL "Set if vgui has GTK")

SOURCE_FILES(vgui_gtk_sources
 vgui_gtk.cxx             vgui_gtk.h
 vgui_gtk_adaptor.cxx     vgui_gtk_adaptor.h
 vgui_gtk_dialog_impl.cxx vgui_gtk_dialog_impl.h
 vgui_gtk_statusbar.cxx   vgui_gtk_statusbar.h
 vgui_gtk_tag.cxx
 vgui_gtk_utils.cxx       vgui_gtk_utils.h
 vgui_gtk_window.cxx      vgui_gtk_window.h
)

AUX_SOURCE_DIRECTORY(Templates vgui_gtk_sources)

ENDIF (HAS_GTK)
