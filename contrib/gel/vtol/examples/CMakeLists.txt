INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

ADD_EXECUTABLE( vtol_example2d example2d.cxx )
TARGET_LINK_LIBRARIES( vtol_example2d vtol vsol vgl vbl vnl vul vcl )
