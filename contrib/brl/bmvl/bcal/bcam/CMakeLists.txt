# multi-view camera library

#INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )

SET(cam_test_sources
 cam_test.cpp
 CameraGraph.h	CameraGraph.cpp
)

ADD_EXECUTABLE(bcam_test ${cam_test_sources})
LINK_LIBRARIES(bcal)

SET(cam_sources
 camera.h          camera.cpp
 CameraNode.h      CameraNode.cpp
 CalibrationObject.h CalibrationObject.cpp
 CalibratePlane.h CalibratePlane.cpp
 ZhangCameraNode.h ZhangCameraNode.cpp
 ZhangLinearCalibrate.h ZhangLinearCalibrate.cpp
)

AUX_SOURCE_DIRECTORY(Templates cam_sources)

ADD_LIBRARY(bcal ${cam_sources})
TARGET_LINK_LIBRARIES(bcal vsol vnl vbl vcsl)

