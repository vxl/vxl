PROJECT(wx_example)

#INCLUDE(FindwxWidgets.cmake)
FIND_PACKAGE(wxWidgets COMPONENTS base core gl)
IF(wxWidgets_FOUND)
  IF(wxWidgets_INCLUDE_DIRS)
    INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
  ENDIF(wxWidgets_INCLUDE_DIRS)

  IF(wxWidgets_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
  ENDIF(wxWidgets_CXX_FLAGS)

  IF(wxWidgets_LINK_DIRECTORIES)
    LINK_DIRECTORIES(${wxWidgets_LINK_DIRECTORIES})
  ENDIF(wxWidgets_LINK_DIRECTORIES)

  # Simple example not using the Document/View architecture.
  SET(wx_childview_example_sources
    wx_childview_app.h     wx_childview_app.cxx
    wx_childview_frame.h   wx_childview_frame.cxx
  )

  ADD_EXECUTABLE(wx_childview_example WIN32 ${wx_childview_example_sources})
  TARGET_LINK_LIBRARIES(wx_childview_example vgui)
  IF(wxWidgets_LIBRARIES)
    TARGET_LINK_LIBRARIES(wx_childview_example ${wxWidgets_LIBRARIES})
  ENDIF(wxWidgets_LIBRARIES)

  # Document/View architecture example
  SET(wx_docview_example_sources
    wx_docview_app.h     wx_docview_app.cxx
    wx_docview_frame.h   wx_docview_frame.cxx
    wx_docview_doc.h     wx_docview_doc.cxx
    wx_docview_view.h    wx_docview_view.cxx
  )

  ADD_EXECUTABLE(wx_docview_example WIN32 ${wx_docview_example_sources})
  TARGET_LINK_LIBRARIES(wx_docview_example vgui)
  IF(wxWidgets_LIBRARIES)
    TARGET_LINK_LIBRARIES(wx_docview_example ${wxWidgets_LIBRARIES})
  ENDIF(wxWidgets_LIBRARIES)


  # Examples using vgui_wx
  IF(BUILD_VGUI_WX AND wxWidgets_wxrc_EXECUTABLE)

    # Example uses the wxWidgets XML resource fi
    SET(wx_xrc_example_sources
      wx_xrc_app.h    wx_xrc_app.cxx
      wx_xrc_frame.h  wx_xrc_frame.cxx
    )
    WXWIDGETS_ADD_RESOURCES(wx_xrc_example_sources wx_xrc.xrc)
    ADD_EXECUTABLE(wx_xrc_example WIN32 MACOSX_BUNDLE ${wx_xrc_example_sources})
    TARGET_LINK_LIBRARIES(wx_xrc_example vgui_wx)

  ENDIF(BUILD_VGUI_WX AND wxWidgets_wxrc_EXECUTABLE)

ENDIF(wxWidgets_FOUND)
