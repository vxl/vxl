# ./oxl/vgui/impl/qt/CMakeLists.txt
#

ADD_LIBRARY(vgui_qt vgui_qt_sources)

SOURCE_FILES(vgui_qt_sources
  vgui_qt
  vgui_qt_tag
  vgui_qt_window
  vgui_qt_adaptor
  vgui_qt_statusbar
  vgui_qt_dialog_impl
  vgui_qt_menu
  vgui_qt_adaptor_mocced
  vgui_qt_menu_mocced
  vgui_qt_dialog_impl_mocced
  vgui_qt_statusbar_mocced
)
