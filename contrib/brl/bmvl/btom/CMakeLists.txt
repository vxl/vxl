# library for tomography
# brl/bmvl/btom/CMakeLists.txt

doxygen_add_library(contrib/brl/bmvl/btom
  DEPENDS contrib/gel/gevd
  PACKAGE contrib/brl
  )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bmvl )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

SET(btom_sources
    btom_gauss_cylinder.cxx         btom_gauss_cylinder.h  btom_gauss_cylinder_sptr.h
    btom_slice_simulator.cxx        btom_slice_simulator.h
    btom_slice_simulator_params.cxx btom_slice_simulator_params.h
)
AUX_SOURCE_DIRECTORY(Templates btom_sources)

ADD_LIBRARY(btom ${btom_sources})
INSTALL_TARGETS(/lib btom)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/btom ${btom_sources})

TARGET_LINK_LIBRARIES(btom gevd vil1 vnl vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
