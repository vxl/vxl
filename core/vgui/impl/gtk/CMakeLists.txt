# ./oxl/vgui/impl/gtk/CMakeLists.txt

ADD_LIBRARY(vgui_gtk vgui_gtk_sources)

SOURCE_FILES(vgui_gtk_sources
  vgui_gtk.cxx
  vgui_gtk_adaptor.cxx
  vgui_gtk_dialog_impl.cxx
  vgui_gtk_statusbar.cxx
  vgui_gtk_tag.cxx
  vgui_gtk_utils.cxx
  vgui_gtk_window.cxx
)

AUX_SOURCE_DIRECTORY(Templates vgui_gtk_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/vgui/impl/gtk/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
