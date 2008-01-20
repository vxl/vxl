PROJECT(wx_example)

SET(WXWIDGETS_USE_LIBS base core gl)
INCLUDE(${MODULE_PATH}/NewCMake/FindwxWidgets.cmake)
IF(WXWIDGETS_FOUND)
  IF(WXWIDGETS_INCLUDE_DIR)
    INCLUDE_DIRECTORIES(${WXWIDGETS_INCLUDE_DIR})
  ENDIF(WXWIDGETS_INCLUDE_DIR)

  IF(WXWIDGETS_CXX_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WXWIDGETS_CXX_FLAGS}")
  ENDIF(WXWIDGETS_CXX_FLAGS)

  IF(WXWIDGETS_LINK_DIRECTORIES)
    LINK_DIRECTORIES(${WXWIDGETS_LINK_DIRECTORIES})
  ENDIF(WXWIDGETS_LINK_DIRECTORIES)

  # Simple example not using the Document/View architecture.
  SET(wx_childview_example_sources
    wx_childview_app.h     wx_childview_app.cxx
    wx_childview_frame.h   wx_childview_frame.cxx
  )

  ADD_EXECUTABLE(wx_childview_example WIN32 ${wx_childview_example_sources})
  TARGET_LINK_LIBRARIES(wx_childview_example vgui)
  IF(WXWIDGETS_LIBRARIES)
    TARGET_LINK_LIBRARIES(wx_childview_example ${WXWIDGETS_LIBRARIES})
  ENDIF(WXWIDGETS_LIBRARIES)

  # Document/View architecture example
  SET(wx_docview_example_sources
    wx_docview_app.h     wx_docview_app.cxx
    wx_docview_frame.h   wx_docview_frame.cxx
    wx_docview_doc.h     wx_docview_doc.cxx
    wx_docview_view.h    wx_docview_view.cxx
  )

  ADD_EXECUTABLE(wx_docview_example WIN32 ${wx_docview_example_sources})
  TARGET_LINK_LIBRARIES(wx_docview_example vgui)
  IF(WXWIDGETS_LIBRARIES)
    TARGET_LINK_LIBRARIES(wx_docview_example ${WXWIDGETS_LIBRARIES})
  ENDIF(WXWIDGETS_LIBRARIES)
ENDIF(WXWIDGETS_FOUND)
