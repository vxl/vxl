# oxl/vgui/impl/qt/CMakeLists.txt

INCLUDE(${MODULE_PATH}/FindQt.cmake)

SET(vgui_IMPL_QT "" CACHE INTERNAL "Set if vgui has Qt")

# Build here only if Qt is available:
IF (HAS_QT)

OPTION(VGUI_USE_QT "Should Qt be compiled into vgui?" YES)

IF(VGUI_USE_QT)

INCLUDE_DIRECTORIES(${QT_INCLUDE_PATH})

SET(vgui_IMPL_QT "vgui_qt" CACHE INTERNAL "Set if vgui has qt")

SET(vgui_qt_sources
 vgui_qt.cxx             vgui_qt.h
 vgui_qt_tag.cxx
 vgui_qt_window.cxx      vgui_qt_window.h
 vgui_qt_adaptor.cxx     vgui_qt_adaptor.h
 vgui_qt_statusbar.cxx   vgui_qt_statusbar.h
 vgui_qt_dialog_impl.cxx vgui_qt_dialog_impl.h
 vgui_qt_menu.cxx        vgui_qt_menu.h
)

QT_WRAP_CPP(vgui_qt vgui_qt_moc_files
 vgui_qt_adaptor.h
 vgui_qt_statusbar.h
 vgui_qt_dialog_impl.h
 vgui_qt_menu.h
)

ADD_LIBRARY(vgui_qt ${vgui_qt_sources} vgui_qt_moc_files)
TARGET_LINK_LIBRARIES(vgui_qt vgui_base vul vbl ${QT_LIBRARIES})

ENDIF(VGUI_USE_QT)

ENDIF (HAS_QT)
