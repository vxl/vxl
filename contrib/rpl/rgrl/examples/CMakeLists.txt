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


#*************************** regression tests ***************************

ADD_TEST( registration1_test registration1
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0002.pgm.txt )

ADD_TEST( registration2_test registration2
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0002.pgm.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0003.pgm.txt)

ADD_TEST( registration3_test registration3
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0002.pgm.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0002_low.pgm.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0003.pgm.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0003_low.pgm.txt)

ADD_TEST( registration_simple_shapes_test registration_simple_shapes )

ADD_TEST( registration_simple_shapes_outliers_test  registration_simple_shapes_outliers )

ADD_TEST( registration_blackbox registration_blackbox )

ADD_TEST( registration_landmark_test  registration_landmark
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0004.pgm.landmarks.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0002.pgm.landmarks.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/mask.png)

ADD_TEST( registration_retina_no_view_test  registration_retina_no_view
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0002.pgm.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0002.pgm.landmarks.invariant.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0004.pgm.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0004.pgm.landmarks.invariant.txt
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/mask.png)

ADD_TEST( registration_range_data_test  registration_range_data
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/bun000.txt.out
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/bun045.txt.out )

ADD_TEST( registration_blackbox_range_data_test  registration_range_data_blackbox
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/dragonStandRight_0_normals.txt )

ADD_TEST( estimate_normals_3d_test estimate_normals_3d
          ${rpi_SOURCE_DIR}/publ/rgrl/examples/IMG0002.pgm.txt IMG0002.pgm.out.txt )
