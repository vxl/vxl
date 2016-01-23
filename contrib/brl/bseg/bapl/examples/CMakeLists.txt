# This is bapl/examples/CMakeLists.txt

add_executable(bapl_lowe_recognizer bapl_lowe_recognizer.cxx )
target_link_libraries(bapl_lowe_recognizer bapl rrel vil vul)

add_executable(bapl_make_pyramids bapl_make_pyramids.cxx )
target_link_libraries(bapl_make_pyramids bapl vil vul)

add_executable(bapl_draw_keypoints bapl_draw_keypoints.cxx )
target_link_libraries(bapl_draw_keypoints bapl ipts vil vul)
