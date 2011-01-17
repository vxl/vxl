
# executable to convert a bit scene to a boxm2_scene
ADD_EXECUTABLE( boxm2_convert_bit_scene_exe
  boxm2_convert_bit_scene.cxx
)
TARGET_LINK_LIBRARIES( boxm2_convert_bit_scene_exe boxm2 boxm2_io boxm_ocl vbl vnl vcl)

#executable to replicate a scene (four times for tests)
ADD_EXECUTABLE( boxm2_rep_scene
  boxm2_rep_scene.cxx
)
TARGET_LINK_LIBRARIES( boxm2_rep_scene boxm2 boxm2_io boxm_ocl vbl vnl vcl)

