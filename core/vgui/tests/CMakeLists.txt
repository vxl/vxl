# This is core/vgui/test/CMakeLists.txt

INCLUDE( ${VXLCORE_INCLUDE_DIR}/vgui/UseVGUI.cmake )

IF( HAS_VGUI )

  # No need to do FindGLUT since the parent CMakeLists.txt did it for
  # us...

  LINK_LIBRARIES( vgui )

  ADD_EXECUTABLE( vgui_test_popup test_popup.cxx )
  #ADD_TEST( vgui_test_popup vgui_test_popup )
  ADD_EXECUTABLE( vgui_test_timer test_timer.cxx )
  #ADD_TEST( vgui_test_timer vgui_test_timer )
  ADD_EXECUTABLE( vgui_test_events test_events.cxx )
  #ADD_TEST( vgui_test_events vgui_test_events )
  ADD_EXECUTABLE( vgui_test_front_back test_front_back.cxx )
  #ADD_TEST( vgui_test_front_back vgui_test_front_back )
  ADD_EXECUTABLE( vgui_test_pixels test_pixels.cxx )
  TARGET_LINK_LIBRARIES( vgui_test_pixels testlib )
  ADD_TEST( vgui_test_pixels vgui_test_pixels )

  IF(X11_FOUND)
    ADD_EXECUTABLE( vgui_test_drawpix_speed test_drawpix_speed.cxx )
    #ADD_TEST( vgui_test_drawpix_speed vgui_test_drawpix_speed )
    ADD_EXECUTABLE( vgui_test_acceleration test_acceleration.cxx )
    #ADD_TEST( vgui_test_acceleration vgui_test_acceleration )
  ENDIF(X11_FOUND)

  ADD_EXECUTABLE( vgui_test_include test_include.cxx )
  TARGET_LINK_LIBRARIES( vgui_test_include vgui )

  ADD_EXECUTABLE( vgui_test_displaybase test_displaybase.cxx )
  TARGET_LINK_LIBRARIES( vgui_test_displaybase vgui )
  ADD_TEST( vgui_test_displaybase vgui_test_displaybase )

ENDIF( HAS_VGUI )
