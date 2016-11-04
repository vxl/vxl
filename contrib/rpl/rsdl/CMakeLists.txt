# rpl/rsdl/CMakeLists.txt

set(rsdl_sources
 rsdl_fwd.h

 rsdl_bounding_box.cxx rsdl_bounding_box.h
 rsdl_dist.cxx         rsdl_dist.h
 rsdl_kd_tree.cxx      rsdl_kd_tree.h
                       rsdl_kd_tree_sptr.h
 rsdl_point.cxx        rsdl_point.h

 rsdl_bins_2d.hxx      rsdl_bins_2d.h
 rsdl_borgefors.hxx    rsdl_borgefors.h
 rsdl_bins.hxx         rsdl_bins.h
)

aux_source_directory(Templates rsdl_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(Templates/rsdl_bins_2d+double.int-.cxx PROPERTIES COMPILE_FLAGS -O1)
endif()

vxl_add_library(LIBRARY_NAME rsdl LIBRARY_SOURCES ${rsdl_sources})
target_link_libraries( rsdl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
