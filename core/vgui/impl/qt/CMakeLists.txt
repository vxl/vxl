# ./oxl/vgui/impl/qt/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindQt.cmake)

# Build here only if Qt is available:
IF(HAS_QT)

ADD_LIBRARY(vgui_qt vgui_qt_sources vgui_qt_moc_files)

SOURCE_FILES(vgui_qt_sources
  vgui_qt.cxx
  vgui_qt_tag.cxx
  vgui_qt_window.cxx
  vgui_qt_adaptor.cxx
  vgui_qt_statusbar.cxx
  vgui_qt_dialog_impl.cxx
  vgui_qt_menu.cxx
)

SOURCE_FILES(vgui_qt_moc_classes
  vgui_qt_adaptor.h
  vgui_qt_dialog_impl.h
  vgui_qt_menu.h
  vgui_qt_statusbar.h
)

QT_WRAP_CPP(vgui_qt vgui_qt_moc_files vgui_qt_moc_classes)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/vgui/impl/qt/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

ENDIF(HAS_QT)
