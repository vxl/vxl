INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt )

ADD_EXECUTABLE( test_popup test_popup.cxx )
ADD_EXECUTABLE( test_timer test_timer.cxx )
ADD_EXECUTABLE( test_events test_events.cxx )
ADD_EXECUTABLE( test_front_back test_front_back.cxx )
ADD_EXECUTABLE( test_pixels test_pixels.cxx )
IF(HAS_X11)
 ADD_EXECUTABLE( test_drawpix_speed test_drawpix_speed.cxx )
 ADD_EXECUTABLE( test_acceleration test_acceleration.cxx )
ENDIF(HAS_X11)

# Don't compile these by default.
IF(EXTRA_TESTS)
SUBDIRS( old-tests Fl gtk Xm )
ENDIF(EXTRA_TESTS)
