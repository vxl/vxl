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
 endif(EXPATPP_FOUND)
endif(EXPAT_FOUND)

aux_source_directory(Templates bwm_video_sources)
add_library(bwm_video ${bwm_video_sources})

target_link_libraries( bwm_video vidl bpgl bpgl_algo brip vil vbl vgl vul vpl vsl vgl_algo vnl_algo vnl bsta bsol vsol vpgl vpgl_algo)

if(EXPAT_FOUND)
  target_link_libraries( bwm_video vidl)
  if(EXPATPP_FOUND)
    target_link_libraries(bwm_video ${EXPATPP_LIBRARIES})
  endif(EXPATPP_FOUND)
endif(EXPAT_FOUND)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

add_subdirectory(exe)

#install the .h .txx and libs
install_targets(/lib bwm_video)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bwm/video/ ${bwm_video_sources})
