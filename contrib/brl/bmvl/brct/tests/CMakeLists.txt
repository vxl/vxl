# testing code for brct library
# brl/bmvl/brct/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bmvl)
SET(BUILD_VGUI_TEST "NO")
IF(BUILD_VGUI_TEST)
# determines if vgui is built and opengl is available
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF(HAS_VGUI)

SET(brct_test_sources
  brct_menus.cxx          brct_menus.h
  brct_windows_frame.cxx  brct_windows_frame.h
  test_brct_kalman.cxx
)


ADD_EXECUTABLE(brct_test_vgui ${brct_test_sources})
TARGET_LINK_LIBRARIES(brct_test_vgui vcl vgui brct bugl bgui)

ENDIF(HAS_VGUI)
ENDIF(BUILD_VGUI_TEST)


ADD_EXECUTABLE(brct_test_all 
  test_driver.cxx
  brct_test_compute_P.cxx
  brct_test_synthetic_data.cxx
)
TARGET_LINK_LIBRARIES(brct_test_all brct bugl bgui testlib)	
