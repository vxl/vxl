# testing code for brct library
# brl/bmvl/brct/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR} )

# determines if vgui is built and opengl is available
INCLUDE( ${VXLCORE_INCLUDE_DIR}/vgui/UseVGUI.cmake )

IF(HAS_VGUI)

SET(test_brct_sources
brct_windows_frame.cxx	brct_windows_frame.h
test_brct_kalman.cxx
)

AUX_SOURCE_DIRECTORY(Templates test_brct_sources)

ADD_EXECUTABLE(test_brct ${test_brct_sources})
ADD_TEST(test_brct_kalman test_brct)

TARGET_LINK_LIBRARIES(test_brct vgui brct)

ENDIF(HAS_VGUI)
