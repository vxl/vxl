# cul/bundler/CMakeLists.txt
# Structure-from-motion Pipeline

doxygen_add_library(contrib/cul/bundler
  DEPENDS core/vil core/vgl core/vpgl vpgl_algo bapl rrel rsdl bpgl_algo
  PACKAGE contrib/cul
  DESCRIPTION "Structure-from-Motion Pipeline. Based off Noah Snavely's Bundler, more info at phototour.cs.washington.edu/bundler/"
  )

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VXL_CUL_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${RPL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bundler_sources
    bundler.h               bundler.cxx

    bundler_sfm.h
    bundler_sfm_impl.h      bundler_sfm_impl.cxx

    bundler_tracks.h
    bundler_tracks_impl.h   bundler_tracks_impl.cxx

    bundler_utils.h         bundler_utils.cxx

    bundler_settings.h      
    bundler_inters.h        bundler_inters.cxx
   )

ADD_LIBRARY(bundler ${bundler_sources})

TARGET_LINK_LIBRARIES(bundler vpgl_algo bpgl_algo vpgl vpgl_file_formats vsol vgl_algo vnl_algo vnl vgl vil vul vbl rrel bapl rsdl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
