# rpl/rsdl/CMakeLists.txt

SET(rsdl_sources
 rsdl_fwd.h

 rsdl_bounding_box.cxx rsdl_bounding_box.h
 rsdl_dist.cxx         rsdl_dist.h
 rsdl_kd_tree.cxx      rsdl_kd_tree.h
 rsdl_point.cxx        rsdl_point.h

 rsdl_bins_2d.txx      rsdl_bins_2d.h
 rsdl_borgefors.txx    rsdl_borgefors.h
 rsdl_bins.txx         rsdl_bins.h
)

AUX_SOURCE_DIRECTORY(Templates rsdl_sources)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(Templates/rsdl_bins_2d+double.int-.cxx PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(rsdl ${rsdl_sources})
TARGET_LINK_LIBRARIES( rsdl vnl vbl )

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
