# multi-view camera library

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )

SET(cam_sources
 camera.h               camera.cpp
 CameraNode.h           CameraNode.cpp
 CameraGraph.h	        CameraGraph.cpp
 CalibrationObject.h    CalibrationObject.cpp
 CalibratePlane.h       CalibratePlane.cpp
 ZhangCameraNode.h      ZhangCameraNode.cpp
 ZhangLinearCalibrate.h ZhangLinearCalibrate.cpp
)

AUX_SOURCE_DIRECTORY(Templates cam_sources)

ADD_LIBRARY(bcal ${cam_sources})
TARGET_LINK_LIBRARIES(bcal vsol vnl vbl vcsl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
