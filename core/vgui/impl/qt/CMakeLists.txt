# ./oxl/vgui/impl/qt/CMakeLists.txt

ADD_LIBRARY(vgui_qt vgui_qt_sources)

SOURCE_FILES(vgui_qt_sources
  vgui_qt.cxx
  vgui_qt_tag.cxx
  vgui_qt_window.cxx
  vgui_qt_adaptor.cxx
  vgui_qt_statusbar.cxx
  vgui_qt_dialog_impl.cxx
  vgui_qt_menu.cxx
  vgui_qt_adaptor_mocced.cxx
  vgui_qt_menu_mocced.cxx
  vgui_qt_dialog_impl_mocced.cxx
  vgui_qt_statusbar_mocced.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/vgui/impl/qt/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
