# This is bapl/examples/CMakeLists.txt

ADD_EXECUTABLE(bapl_lowe_recognizer bapl_lowe_recognizer.cxx )
TARGET_LINK_LIBRARIES(bapl_lowe_recognizer bapl rrel vil vul)

ADD_EXECUTABLE(bapl_make_pyramids bapl_make_pyramids.cxx )
TARGET_LINK_LIBRARIES(bapl_make_pyramids bapl vil vul)

ADD_EXECUTABLE(bapl_draw_keypoints bapl_draw_keypoints.cxx )
TARGET_LINK_LIBRARIES(bapl_draw_keypoints bapl ipts vil vul)
