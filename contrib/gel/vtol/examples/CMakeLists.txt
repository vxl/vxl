INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

ADD_EXECUTABLE( vtol_example2d           example2d.cxx )
TARGET_LINK_LIBRARIES( vtol_example2d    vtol vcl )

ADD_EXECUTABLE( vtol_example3d           example3d.cxx )
TARGET_LINK_LIBRARIES( vtol_example3d    vtol vbl vcl )

ADD_EXECUTABLE( vtol_example_vdgl        example_vdgl.cxx )
TARGET_LINK_LIBRARIES( vtol_example_vdgl vtol vsol vdgl vcl )
