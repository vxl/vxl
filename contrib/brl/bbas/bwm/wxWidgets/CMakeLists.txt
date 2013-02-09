PROJECT(wx_example)

FIND_PACKAGE(wxWidgets COMPONENTS mono) #base core gl xml xrc)
IF(wxWidgets_FOUND)
  
  INCLUDE( ${wxWidgets_USE_FILE} )
  #INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})

  # Simple example not using the Document/View architecture.
  SET(bwm_wx_example_sources
    bwm_wx_app.h     bwm_wx_app.cxx
    bwm_wx_frame.h   bwm_wx_frame.cxx
    bwm_wx_menu.h
  )
  
  CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/crosscut.xrc ${CMAKE_CURRENT_BINARY_DIR}/crosscut.xrc COPYONLY )
  WXWIDGETS_ADD_RESOURCES(bwm_wx_example_sources crosscut.xrc)
  ADD_EXECUTABLE(bwm_wx_example WIN32 ${bwm_wx_example_sources} )
  TARGET_LINK_LIBRARIES(bwm_wx_example bwm vgui vgui_wx ${wxWidgets_LIBRARIES})

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(wxWidgets_FOUND)
