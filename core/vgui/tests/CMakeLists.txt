# ./oxl/vgui/test/CMakeLists.txt

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
ADD_TEST( vgui_test_pixels vgui_test_pixels )
IF(X11_FOUND)
 ADD_EXECUTABLE( vgui_test_drawpix_speed test_drawpix_speed.cxx )
#ADD_TEST( vgui_test_drawpix_speed vgui_test_drawpix_speed )
 ADD_EXECUTABLE( vgui_test_acceleration test_acceleration.cxx )
#ADD_TEST( vgui_test_acceleration vgui_test_acceleration )
ENDIF(X11_FOUND)
IF(HAS_SDL)
 ADD_EXECUTABLE( test_SDL ../impl/SDL/test-SDL.cxx )
#ADD_TEST( test_SDL test_SDL )
ENDIF(HAS_SDL)

# Don't compile these by default.
IF(EXTRA_TESTS)
SUBDIRS( old-tests Fl gtk Xm )
ENDIF(EXTRA_TESTS)
