INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

ADD_EXECUTABLE( example2d example2d.cxx )
TARGET_LINK_LIBRARIES( example2d vtol vsol vgl vbl vnl vul vcl )
