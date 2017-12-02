include_directories( ${VXL_MUL_INCLUDE_DIR} )


add_executable( registration1
  registration1.cxx
)
target_link_libraries( registration1 rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration2
  registration2.cxx
)
target_link_libraries( registration2 rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration3
  registration3.cxx
)
target_link_libraries( registration3 rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration_landmark
  registration_landmark.cxx
)
target_link_libraries( registration_landmark rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration_retina_no_view
  registration_retina_no_view.cxx
)
target_link_libraries( registration_retina_no_view rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration_simple_shapes
  registration_simple_shapes.cxx
)
target_link_libraries( registration_simple_shapes rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration_simple_shapes_outliers
  registration_simple_shapes_outliers.cxx
)
target_link_libraries( registration_simple_shapes_outliers rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration_blackbox
  registration_blackbox.cxx
)
target_link_libraries( registration_blackbox rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration_range_data
  registration_range_data.cxx
)
target_link_libraries( registration_range_data rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( registration_range_data_blackbox
  registration_range_data_blackbox.cxx
)
target_link_libraries( registration_range_data_blackbox rgrl ${VXL_LIB_PREFIX}testlib)

add_executable( estimate_normals_3d
  estimate_normals_3d.cxx
)
target_link_libraries( estimate_normals_3d rrel rsdl ${VXL_LIB_PREFIX}vnl_algo)

# pesudo matching executables
add_executable( match_2d
  match_2d.cxx
)
target_link_libraries( match_2d rgrl ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)

# multi-resolution version
add_executable( match_2d_multi
  match_2d_multi.cxx
)
target_link_libraries( match_2d_multi ${VXL_LIB_PREFIX}testlib rgrl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)



#*************************** regression tests ***************************

add_test( NAME rgrl_registration1_test COMMAND $<TARGET_FILE:registration1>
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt )

add_test( NAME rgrl_registration2_test COMMAND $<TARGET_FILE:registration2>
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0003.pgm.txt)

add_test( NAME rgrl_registration3_test COMMAND $<TARGET_FILE:registration3>
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002_low.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0003.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0003_low.pgm.txt)

add_test( NAME rgrl_registration_simple_shapes_test COMMAND $<TARGET_FILE:registration_simple_shapes> )

add_test( NAME rgrl_registration_simple_shapes_outliers_test COMMAND $<TARGET_FILE:registration_simple_shapes_outliers> )

add_test( NAME rgrl_registration_blackbox COMMAND $<TARGET_FILE:registration_blackbox> )

add_test( NAME rgrl_registration_landmark_test COMMAND $<TARGET_FILE:registration_landmark>
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0004.pgm.landmarks.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.landmarks.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/mask.png)

add_test( NAME rgrl_registration_retina_no_view_test COMMAND $<TARGET_FILE:registration_retina_no_view>
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.landmarks.invariant.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0004.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0004.pgm.landmarks.invariant.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/mask.png)

add_test( NAME rgrl_registration_range_data_test COMMAND $<TARGET_FILE:registration_range_data>
          ${CMAKE_CURRENT_SOURCE_DIR}/bun000.txt.out
          ${CMAKE_CURRENT_SOURCE_DIR}/bun045.txt.out )

add_test( NAME rgrl_registration_blackbox_range_data_test COMMAND $<TARGET_FILE:registration_range_data_blackbox>
          ${CMAKE_CURRENT_SOURCE_DIR}/dragonStandRight_0_normals.txt )

add_test( NAME rgrl_estimate_normals_3d_test COMMAND $<TARGET_FILE:estimate_normals_3d>
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt IMG0002.pgm.out.txt )

add_test( NAME rgrl_pseudo_match_2d_test COMMAND $<TARGET_FILE:match_2d>
          1 ${CMAKE_CURRENT_SOURCE_DIR}/sigcube1_face_features.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/sigcube1.png
          ${CMAKE_CURRENT_SOURCE_DIR}/sigcube2.png
          -init ${CMAKE_CURRENT_SOURCE_DIR}/init_xform_flipped_y.txt
          -model affine )

add_test( NAME rgrl_pseudo_match_2d_mult_test COMMAND $<TARGET_FILE:match_2d_multi>
          1 ${CMAKE_CURRENT_SOURCE_DIR}/sigcube1_face_features.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/sigcube1.png
          ${CMAKE_CURRENT_SOURCE_DIR}/sigcube2.png
          -init ${CMAKE_CURRENT_SOURCE_DIR}/init_xform_flipped_y.txt
          -model affine )
