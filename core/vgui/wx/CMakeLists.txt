# core/vgui/wx/CMakeLists.txt
project(vgui_wx)

# Build an additional library of widgets for use with vgui.
# These widgets are intended for use with wxWidgets applications
# that use a vgui_wx_adaptor, NOT for vgui applications that are
# rendered using wxWidgets as a Toolkit.

find_package(wxWidgets COMPONENTS base core gl adv html xml xrc)
if(wxWidgets_FOUND)
  include( ${wxWidgets_USE_FILE} )

  set(vgui_wx_sources
    vgui_wx.cxx                     vgui_wx.h

    # wxWidgets
    wxSliderPanel.cxx               wxSliderPanel.h
    wxVideoControl.cxx              wxVideoControl.h

    # XML Resource Handlers
    vgui_wx_adaptorXmlHandler.cxx   vgui_wx_adaptorXmlHandler.h
    wxSliderPanelXmlHandler.cxx     wxSliderPanelXmlHandler.h
    wxVideoControlXmlHandler.cxx    wxVideoControlXmlHandler.h
  )

  add_library(vgui_wx ${vgui_wx_sources})
  target_link_libraries(vgui_wx vgui ${wxWidgets_LIBRARIES})

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif(BUILD_TESTING)

endif(wxWidgets_FOUND)
