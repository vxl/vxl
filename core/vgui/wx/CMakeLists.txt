# core/vgui/wx/CMakeLists.txt
PROJECT(vgui_wx)

# Build an additional library of widgets for use with vgui.
# These widgets are intended for use with wxWidgets applications
# that use a vgui_wx_adaptor, NOT for vgui applications that are
# rendered using wxWidgets as a Toolkit.

FIND_PACKAGE(wxWidgets COMPONENTS base core gl adv html xml xrc)
IF(wxWidgets_FOUND)
  INCLUDE( ${wxWidgets_USE_FILE} )

  SET(vgui_wx_sources
    vgui_wx.cxx                     vgui_wx.h

    # wxWidgets
    wxSliderPanel.cxx               wxSliderPanel.h

    # XML Resource Handlers
    vgui_wx_adaptorXmlHandler.cxx   vgui_wx_adaptorXmlHandler.h
    wxSliderPanelXmlHandler.cxx     wxSliderPanelXmlHandler.h
  )

  ADD_LIBRARY(vgui_wx ${vgui_wx_sources})
  TARGET_LINK_LIBRARIES(vgui_wx vgui ${wxWidgets_LIBRARIES})

ENDIF(wxWidgets_FOUND)
