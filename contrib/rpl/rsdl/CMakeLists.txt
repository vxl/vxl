# rpl/rsdl/CMakeLists.txt

SOURCE_FILES(rsdl_sources
 rsdl_fwd.h
 rsdl_test.h

 rsdl_bounding_box.cxx rsdl_bounding_box.h
 rsdl_dist.cxx         rsdl_dist.h
 rsdl_kd_tree.cxx      rsdl_kd_tree.h
 rsdl_point.cxx        rsdl_point.h

 rsdl_bins_2d.txx      rsdl_bins_2d.h
 rsdl_borgefors.txx    rsdl_borgefors.h
)

AUX_SOURCE_DIRECTORY(Templates rsdl_sources)

ADD_LIBRARY(rsdl rsdl_sources)
TARGET_LINK_LIBRARIES(rsdl vnl vbl vil)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
