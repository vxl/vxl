# ./oul/ouel/CMakeLists.txt

doxygen_add_library(contrib/oul/ouel
  DEPENDS core/vnl
  PACKAGE contrib/oul
  DESCRIPTION "Estimation/filtering library"
  )

SET(ouel_sources
  kalman_filter.h    kalman_filter.cxx
)

ADD_LIBRARY(ouel ${ouel_sources})
INSTALL_TARGETS(/lib ouel)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/oul/ouel ${ouel_sources})
TARGET_LINK_LIBRARIES( ouel vnl_algo vnl )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
