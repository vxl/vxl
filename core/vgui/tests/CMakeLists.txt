# This is core/vgui/test/CMakeLists.txt

project( vgui_tests )

include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if( HAS_VGUI )

  # No need to do FindGLUT since the parent CMakeLists.txt did it for
  # us...

  link_libraries( ${VXL_LIB_PREFIX}vgui )

  add_executable( vgui_test_popup test_popup.cxx )
  #add_test( NAME vgui_test_popup COMMAND $<TARGET_FILE:vgui_test_popup> )
  add_executable( vgui_test_timer test_timer.cxx )
  #add_test( NAME vgui_test_timer COMMAND $<TARGET_FILE:vgui_test_timer> )
  add_executable( vgui_test_events test_events.cxx )
  #add_test( NAME vgui_test_events COMMAND $<TARGET_FILE:vgui_test_events> )
  add_executable( vgui_test_front_back test_front_back.cxx )
  #add_test( NAME vgui_test_front_back COMMAND $<TARGET_FILE:vgui_test_front_back> )

  if(X11_FOUND)
    add_executable( vgui_test_drawpix_speed test_drawpix_speed.cxx )
    #add_test( NAME vgui_test_drawpix_speed COMMAND $<TARGET_FILE:vgui_test_drawpix_speed> )
    add_executable( vgui_test_acceleration test_acceleration.cxx )
    #add_test( NAME vgui_test_acceleration COMMAND $<TARGET_FILE:vgui_test_acceleration> )
  endif()

  add_executable( vgui_test_all
    test_driver.cxx

    test_displaybase.cxx
    test_pixels.cxx
    test_image_tableau.cxx
    test_range_map.cxx
  )
  target_link_libraries( vgui_test_all ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib )

  add_test( NAME vgui_test_displaybase COMMAND $<TARGET_FILE:vgui_test_all> test_displaybase )
  add_test( NAME vgui_test_pixels COMMAND $<TARGET_FILE:vgui_test_all> test_pixels )
  add_test( NAME vgui_test_image_tableau COMMAND $<TARGET_FILE:vgui_test_all> test_image_tableau ${CMAKE_CURRENT_SOURCE_DIR}/test_image.ppm )
  add_test( NAME vgui_test_range_map COMMAND $<TARGET_FILE:vgui_test_all> test_range_map)

  add_executable( vgui_test_include test_include.cxx )
  target_link_libraries( vgui_test_include ${VXL_LIB_PREFIX}vgui )
  add_executable( vgui_test_template_include test_template_include.cxx )
  target_link_libraries( vgui_test_template_include ${VXL_LIB_PREFIX}vgui )

  add_executable( vgui_test_drawpix_speed2 test_drawpix_speed2.cxx )
  add_executable( vgui_test_rubberband_dialog test_rubberband_dialog.cxx )

endif()
