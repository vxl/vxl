INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

ADD_EXECUTABLE( vtol_example2d example2d.cxx )
ADD_EXECUTABLE( vtol_example3d example3d.cxx )
TARGET_LINK_LIBRARIES( vtol_example2d vtol vsol vgl vbl vnl vul vcl )
TARGET_LINK_LIBRARIES( vtol_example3d vtol vsol vgl vbl vnl vul vcl )
