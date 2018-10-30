project(wx_example)

#include(FindwxWidgets.cmake)
find_package(wxWidgets COMPONENTS base core gl)
if(wxWidgets_FOUND)
  if(wxWidgets_INCLUDE_DIRS)
    include_directories(${wxWidgets_INCLUDE_DIRS})
  endif()

  if(wxWidgets_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
  endif()

  if(wxWidgets_LINK_DIRECTORIES)
    link_directories(${wxWidgets_LINK_DIRECTORIES})
  endif()

  # Simple example not using the Document/View architecture.
  set(wx_childview_example_sources
    wx_childview_app.h     wx_childview_app.cxx
    wx_childview_frame.h   wx_childview_frame.cxx
  )

  add_executable(wx_childview_example WIN32 ${wx_childview_example_sources})
  target_link_libraries(wx_childview_example ${VXL_LIB_PREFIX}vgui)
  if(wxWidgets_LIBRARIES)
    target_link_libraries(wx_childview_example ${wxWidgets_LIBRARIES})
  endif()

  # Document/View architecture example
  set(wx_docview_example_sources
    wx_docview_app.h     wx_docview_app.cxx
    wx_docview_frame.h   wx_docview_frame.cxx
    wx_docview_doc.h     wx_docview_doc.cxx
    wx_docview_view.h    wx_docview_view.cxx
  )

  add_executable(wx_docview_example WIN32 ${wx_docview_example_sources})
  target_link_libraries(wx_docview_example ${VXL_LIB_PREFIX}vgui)
  if(wxWidgets_LIBRARIES)
    target_link_libraries(wx_docview_example ${wxWidgets_LIBRARIES})
  endif()


  # Examples using vgui_wx
  if(VXL_BUILD_VGUI_WX AND wxWidgets_wxrc_EXECUTABLE)

    # Example uses the wxWidgets XML resource fi
    set(wx_xrc_example_sources
      wx_xrc_app.h    wx_xrc_app.cxx
      wx_xrc_frame.h  wx_xrc_frame.cxx
    )
    WXWIDGETS_ADD_RESOURCES(wx_xrc_example_sources wx_xrc.xrc)
    add_executable(wx_xrc_example WIN32 MACOSX_BUNDLE ${wx_xrc_example_sources})
    target_link_libraries(wx_xrc_example ${VXL_LIB_PREFIX}vgui_wx)

  endif()

endif()
