# multi-view camera library

#INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )

SET(cam_sources
camera.h	camera.cpp
ZhangCamera.h zhangCamera.cpp
CameraNode.h CameraNode.cpp
cam_test.cpp
)

AUX_SOURCE_DIRECTORY(Templates cam_sources)

ADD_LIBRARY(bcal ${cam_sources})

TARGET_LINK_LIBRARIES(bcal vsol vnl vbl vcsl)

