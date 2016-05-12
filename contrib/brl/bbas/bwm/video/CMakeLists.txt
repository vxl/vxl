# world modeling application
# brl/bbas/bwm/video/CMakeLists.txt


set(bwm_video_sources
   bwm_video_export.h
   bwm_video_cam_istream.cxx     bwm_video_cam_istream.h   bwm_video_cam_istream_sptr.h
   bwm_video_cam_ostream.cxx     bwm_video_cam_ostream.h   bwm_video_cam_ostream_sptr.h
   bwm_video_corr.cxx            bwm_video_corr.h          bwm_video_corr_sptr.h
   bwm_video_registration.cxx    bwm_video_registration.h
                                 bwm_video_site_io_defs.h
)

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  include_directories(${EXPAT_INCLUDE_DIRS} )
  include_directories(${EXPATPP_INCLUDE_DIRS} )

  set(bwm_video_sources ${bwm_video_sources}
     bwm_video_site_io.cxx         bwm_video_site_io.h
     bwm_video_corr_processor.cxx  bwm_video_corr_processor.h
  )
 endif()
endif()

aux_source_directory(Templates bwm_video_sources)
vxl_add_library(LIBRARY_NAME bwm_video LIBRARY_SOURCES ${bwm_video_sources})

target_link_libraries( bwm_video ${VXL_LIB_PREFIX}vidl bpgl bpgl_algo brip ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl bsta bsol vsol ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo)

if(EXPAT_FOUND)
  target_link_libraries( bwm_video ${VXL_LIB_PREFIX}vidl)
  if(EXPATPP_FOUND)
    target_link_libraries(bwm_video ${EXPATPP_LIBRARIES})
  endif()
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(exe)

#install the .h .hxx and libs
