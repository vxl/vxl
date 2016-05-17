# cul/bundler/CMakeLists.txt
# Structure-from-motion Pipeline

doxygen_add_library(contrib/cul/bundler
  DEPENDS core/vil core/vgl core/vpgl vpgl_algo bapl rrel rsdl bpgl_algo
  PACKAGE contrib/cul
  DESCRIPTION "Structure-from-Motion Pipeline. Based off Noah Snavely's Bundler, more info at phototour.cs.washington.edu/bundler/"
  )

include_directories(${VXL_CUL_INCLUDE_DIR})

include_directories(${GEL_INCLUDE_DIR})
include_directories(${RPL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR})
include_directories(${BRL_INCLUDE_DIR}/bbas)

set(bundler_sources
    bundler.h               bundler.cxx

    bundler_sfm.h
    bundler_sfm_impl.h      bundler_sfm_impl.cxx

    bundler_tracks.h
    bundler_tracks_impl.h   bundler_tracks_impl.cxx

    bundler_utils.h         bundler_utils.cxx

    bundler_settings.h
    bundler_inters.h        bundler_inters.cxx
   )

vxl_add_library(LIBRARY_NAME bundler LIBRARY_SOURCES ${bundler_sources})

target_link_libraries(bundler ${VXL_LIB_PREFIX}vpgl_algo bpgl_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_file_formats vsol ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl rrel bapl rsdl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
