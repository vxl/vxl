# ./oxl/vgui/test/CMakeLists.txt

ADD_EXECUTABLE( test_popup test_popup.cxx )
#ADD_TEST( vgui_test_popup test_popup     )
ADD_EXECUTABLE( test_timer test_timer.cxx )
#ADD_TEST( vgui_test_timer test_timer     )
ADD_EXECUTABLE( test_events test_events.cxx )
#ADD_TEST( vgui_test_events test_events     )
ADD_EXECUTABLE( test_front_back test_front_back.cxx )
#ADD_TEST( vgui_test_front_back test_front_back     )
ADD_EXECUTABLE( test_pixels test_pixels.cxx )
ADD_TEST(  vgui_test_pixels test_pixels     )
IF(HAS_X11)
 ADD_EXECUTABLE( test_drawpix_speed test_drawpix_speed.cxx )
#ADD_TEST(  vgui_test_drawpix_speed test_drawpix_speed     )
 ADD_EXECUTABLE( test_acceleration test_acceleration.cxx )
#ADD_TEST(  vgui_test_acceleration test_acceleration     )
ENDIF(HAS_X11)

# Don't compile these by default.
IF(EXTRA_TESTS)
SUBDIRS( old-tests Fl gtk Xm )
ENDIF(EXTRA_TESTS)

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt )
