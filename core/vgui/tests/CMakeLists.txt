# This is core/vgui/test/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

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

  IF(X11_FOUND)
    ADD_EXECUTABLE( vgui_test_drawpix_speed test_drawpix_speed.cxx )
    #ADD_TEST( vgui_test_drawpix_speed vgui_test_drawpix_speed )
    ADD_EXECUTABLE( vgui_test_acceleration test_acceleration.cxx )
    #ADD_TEST( vgui_test_acceleration vgui_test_acceleration )
  ENDIF(X11_FOUND)

  ADD_EXECUTABLE( vgui_test_all
    test_driver.cxx

    test_displaybase.cxx
    test_pixels.cxx
    test_image_tableau.cxx
  )
  TARGET_LINK_LIBRARIES( vgui_test_all vgui vul testlib )

  ADD_TEST( vgui_test_displaybase         vgui_test_all test_displaybase )
  ADD_TEST( vgui_test_pixels              vgui_test_all test_pixels )
  ADD_TEST( vgui_test_image_tableau       vgui_test_all test_image_tableau ${CMAKE_CURRENT_SOURCE_DIR}/test_image.ppm )

  ADD_EXECUTABLE( vgui_test_include test_include.cxx )
  TARGET_LINK_LIBRARIES( vgui_test_include vgui )

ENDIF( HAS_VGUI )
