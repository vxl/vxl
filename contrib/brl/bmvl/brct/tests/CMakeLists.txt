# testing code for brct library
# brl/bmvl/brct/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR} )

# determines if vgui is built and opengl is available
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF(HAS_VGUI)

SET(brct_test_sources
  brct_menus.cxx          brct_menus.h
  brct_windows_frame.cxx  brct_windows_frame.h
  test_brct_kalman.cxx
)

AUX_SOURCE_DIRECTORY(Templates brct_test_sources)

ADD_EXECUTABLE(brct_test ${brct_test_sources})
#ADD_TEST(brct_test_kalman brct_test)

TARGET_LINK_LIBRARIES(brct_test vcl vgui brct bugl bgui)

ENDIF(HAS_VGUI)
