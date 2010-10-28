# This is bapl/examples/CMakeLists.txt

LINK_LIBRARIES( bapl rrel vul )
ADD_EXECUTABLE(bapl_lowe_recognizer bapl_lowe_recognizer.cxx )

LINK_LIBRARIES( bapl vul )
ADD_EXECUTABLE(bapl_make_pyramids bapl_make_pyramids.cxx )

LINK_LIBRARIES( bapl vul ipts )
ADD_EXECUTABLE(bapl_draw_keypoints bapl_draw_keypoints.cxx )
