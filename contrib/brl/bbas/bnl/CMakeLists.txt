# brl/bbas/bnl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(bnl_sources
    bnl_parabolic_interpolator.cxx    bnl_parabolic_interpolator.h
    bnl_quadratic_interpolator.cxx    bnl_quadratic_interpolator.h
   )

ADD_LIBRARY(bnl ${bnl_sources})
INSTALL_TARGETS(/lib bnl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bnl ${bnl_sources})

TARGET_LINK_LIBRARIES(bnl vnl vnl_algo)

SUBDIRS(algo)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
