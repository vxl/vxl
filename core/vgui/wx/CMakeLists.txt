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

  vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vgui_wx LIBRARY_SOURCES ${vgui_wx_sources})
  target_link_libraries(${VXL_LIB_PREFIX}vgui_wx ${VXL_LIB_PREFIX}vgui ${wxWidgets_LIBRARIES})

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
