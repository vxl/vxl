# oxl/vgui/impl/mfc/CMakeLists.txt

SET(vgui_IMPL_MFC "" CACHE INTERNAL "Set if vgui has mfc")

IF (HAS_MFC)

OPTION(VGUI_USE_MFC "Should MFC be compiled into vgui?" YES)

IF(VGUI_USE_MFC)

SET(vgui_IMPL_MFC "vgui_mfc" CACHE INTERNAL "Set if vgui has mfc")

SET(vgui_mfc_sources
 vgui_mfc.cxx             vgui_mfc.h
 vgui_mfc_adaptor.cxx     vgui_mfc_adaptor.h
 vgui_mfc_app.cxx         vgui_mfc_app.h
 vgui_mfc_app_init.cxx    vgui_mfc_app_init.h
 vgui_mfc_dialog_impl.cxx vgui_mfc_dialog_impl.h
 vgui_mfc_doc.cxx         vgui_mfc_doc.h
 vgui_mfc_mainfrm.cxx     vgui_mfc_mainfrm.h
 vgui_mfc_statusbar.cxx   vgui_mfc_statusbar.h
 vgui_mfc_tag.cxx
 vgui_mfc_util.cxx        vgui_mfc_util.h
 vgui_mfc_utils.cxx       vgui_mfc_utils.h
 vgui_mfc_view.cxx        vgui_mfc_view.h
 vgui_mfc_window.cxx      vgui_mfc_window.h
)

ADD_LIBRARY(vgui_mfc ${vgui_mfc_sources})
TARGET_LINK_LIBRARIES(vgui_mfc vgui_base)

ENDIF(VGUI_USE_MFC)

ENDIF (HAS_MFC)
