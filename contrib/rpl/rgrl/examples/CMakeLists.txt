INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )


ADD_EXECUTABLE( registration1
  registration1.cxx
)
TARGET_LINK_LIBRARIES( registration1 rgrl testlib)

ADD_EXECUTABLE( registration2
  registration2.cxx
)
TARGET_LINK_LIBRARIES( registration2 rgrl testlib)

ADD_EXECUTABLE( registration3
  registration3.cxx
)
TARGET_LINK_LIBRARIES( registration3 rgrl testlib)

ADD_EXECUTABLE( registration_landmark
  registration_landmark.cxx
)
TARGET_LINK_LIBRARIES( registration_landmark rgrl testlib)

ADD_EXECUTABLE( registration_retina_no_view
  registration_retina_no_view.cxx
)
TARGET_LINK_LIBRARIES( registration_retina_no_view rgrl  testlib)

ADD_EXECUTABLE( registration_simple_shapes
  registration_simple_shapes.cxx
)
TARGET_LINK_LIBRARIES( registration_simple_shapes rgrl testlib)

ADD_EXECUTABLE( registration_simple_shapes_outliers
  registration_simple_shapes_outliers.cxx
)
TARGET_LINK_LIBRARIES( registration_simple_shapes_outliers rgrl testlib)

ADD_EXECUTABLE( registration_blackbox
  registration_blackbox.cxx
)
TARGET_LINK_LIBRARIES( registration_blackbox rgrl testlib)

ADD_EXECUTABLE( registration_range_data
  registration_range_data.cxx
)
TARGET_LINK_LIBRARIES( registration_range_data rgrl testlib)

ADD_EXECUTABLE( registration_range_data_blackbox
  registration_range_data_blackbox.cxx
)
TARGET_LINK_LIBRARIES( registration_range_data_blackbox rgrl testlib)

ADD_EXECUTABLE( estimate_normals_3d
  estimate_normals_3d.cxx
)
TARGET_LINK_LIBRARIES( estimate_normals_3d rrel rsdl vnl_algo)

# pesudo matching executables
ADD_EXECUTABLE( match_2d
  match_2d.cxx 
)
TARGET_LINK_LIBRARIES( match_2d rgrl testlib vnl vil)

# multi-resolution version
ADD_EXECUTABLE( match_2d_multi
  match_2d_multi.cxx 
)
TARGET_LINK_LIBRARIES( match_2d_multi testlib rgrl vil_algo vil vnl)



#*************************** regression tests ***************************

ADD_TEST( registration1_test registration1
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt )

ADD_TEST( registration2_test registration2
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0003.pgm.txt)

ADD_TEST( registration3_test registration3
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002_low.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0003.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0003_low.pgm.txt)

ADD_TEST( registration_simple_shapes_test registration_simple_shapes )

ADD_TEST( registration_simple_shapes_outliers_test  registration_simple_shapes_outliers )

ADD_TEST( registration_blackbox registration_blackbox )

ADD_TEST( registration_landmark_test  registration_landmark
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0004.pgm.landmarks.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.landmarks.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/mask.png)

ADD_TEST( registration_retina_no_view_test  registration_retina_no_view
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.landmarks.invariant.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0004.pgm.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0004.pgm.landmarks.invariant.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/mask.png)

ADD_TEST( registration_range_data_test  registration_range_data
          ${CMAKE_CURRENT_SOURCE_DIR}/bun000.txt.out
          ${CMAKE_CURRENT_SOURCE_DIR}/bun045.txt.out )

ADD_TEST( registration_blackbox_range_data_test  registration_range_data_blackbox
          ${CMAKE_CURRENT_SOURCE_DIR}/dragonStandRight_0_normals.txt )

ADD_TEST( estimate_normals_3d_test estimate_normals_3d
          ${CMAKE_CURRENT_SOURCE_DIR}/IMG0002.pgm.txt IMG0002.pgm.out.txt )

ADD_TEST( pseudo_match_2d_test match_2d
          1 ${CMAKE_CURRENT_SOURCE_DIR}/sigcube1_face_features.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/sigcube1.png 
          ${CMAKE_CURRENT_SOURCE_DIR}/sigcube2.png
          -init ${CMAKE_CURRENT_SOURCE_DIR}/init_xform_flipped_y.txt
          -model affine )

ADD_TEST( pseudo_match_2d_mult_test match_2d_multi
          1 ${CMAKE_CURRENT_SOURCE_DIR}/sigcube1_face_features.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/sigcube1.png 
          ${CMAKE_CURRENT_SOURCE_DIR}/sigcube2.png
          -init ${CMAKE_CURRENT_SOURCE_DIR}/init_xform_flipped_y.txt
          -model affine )
