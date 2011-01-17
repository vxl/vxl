#executable to run a cpp change detection
ADD_EXECUTABLE( boxm2_cpp_change_detection_exe
  boxm2_cpp_change_detection_main.cxx
)
TARGET_LINK_LIBRARIES( boxm2_cpp_change_detection_exe boxm2_cpp_pro sdet  vil_pro vpgl_pro vul )
#executable to run a cpp update
ADD_EXECUTABLE( boxm2_cpp_update_exe
  boxm2_cpp_update_main.cxx
)
TARGET_LINK_LIBRARIES( boxm2_cpp_update_exe boxm2_cpp_pro sdet vil_pro vpgl_pro vul )
#executable to run a cpp render
ADD_EXECUTABLE( boxm2_cpp_render_exe
  boxm2_cpp_render_main.cxx
)
TARGET_LINK_LIBRARIES( boxm2_cpp_render_exe boxm2_cpp_pro  vil_pro vpgl_pro vul )
