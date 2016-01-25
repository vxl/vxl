# cul/bundler/CMakeLists.txt
# Structure-from-motion Pipeline

doxygen_add_library(contrib/cul/bundler
  DEPENDS core/vil core/vgl core/vpgl vpgl_algo bapl rrel rsdl bpgl_algo
  PACKAGE contrib/cul
  DESCRIPTION "Structure-from-Motion Pipeline. Based off Noah Snavely's Bundler, more info at phototour.cs.washington.edu/bundler/"
  )

include_directories(${VXLCORE_INCLUDE_DIR})
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

add_library(bundler ${bundler_sources})

target_link_libraries(bundler vpgl_algo bpgl_algo vpgl vpgl_file_formats vsol vgl_algo vnl_algo vnl vgl vil vul vbl rrel bapl rsdl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
