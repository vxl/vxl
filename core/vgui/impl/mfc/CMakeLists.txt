# ./oxl/vgui/impl/mfc/CMakeLists.txt

ADD_LIBRARY(vgui_mfc vgui_mfc_sources)

SOURCE_FILES(vgui_mfc_sources
  vgui_mfc.cxx
  vgui_mfc_adaptor.cxx
  vgui_mfc_app.cxx
  vgui_mfc_app_init.cxx
  vgui_mfc_dialog_impl.cxx
  vgui_mfc_doc.cxx
  vgui_mfc_mainfrm.cxx
  vgui_mfc_statusbar.cxx
  vgui_mfc_tag.cxx
  vgui_mfc_util.cxx
  vgui_mfc_utils.cxx
  vgui_mfc_view.cxx
  vgui_mfc_window.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/vgui/impl/mfc/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
