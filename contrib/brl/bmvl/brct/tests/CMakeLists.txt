# testing code for brct library
# brl/bmvl/brct/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )

SET(test_brct_sources
test_brct_kalman.cxx
test_brct.cxx
)

AUX_SOURCE_DIRECTORY(Templates test_brct_sources)

ADD_EXECUTABLE(test_brct ${test_brct_sources})

TARGET_LINK_LIBRARIES(test_brct vgui vdgl vtol vsol vil1 vnl vbl brct)

