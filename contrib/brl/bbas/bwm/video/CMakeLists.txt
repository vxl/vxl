# world modeling application
# brl/bbas/bwm/video/CMakeLists.txt


SET(bwm_video_sources
   dll.h

   bwm_video_cam_istream.cxx     bwm_video_cam_istream.h   bwm_video_cam_istream_sptr.h
   bwm_video_cam_ostream.cxx     bwm_video_cam_ostream.h   bwm_video_cam_ostream_sptr.h
   bwm_video_corr.cxx            bwm_video_corr.h          bwm_video_corr_sptr.h
   bwm_video_registration.cxx    bwm_video_registration.h
                                 bwm_video_site_io_defs.h
)

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )

  SET(bwm_video_sources ${bwm_video_sources}
     bwm_video_site_io.cxx         bwm_video_site_io.h
     bwm_video_corr_processor.cxx  bwm_video_corr_processor.h
  )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

AUX_SOURCE_DIRECTORY(Templates bwm_video_sources)
ADD_LIBRARY(bwm_video ${bwm_video_sources})

TARGET_LINK_LIBRARIES( bwm_video vidl bpgl bpgl_algo brip vil vbl vgl vul vpl vsl vgl_algo vnl_algo vnl bsta bsol vsol vpgl vpgl_algo)

IF(EXPAT_FOUND)
  TARGET_LINK_LIBRARIES( bwm_video vidl)
  IF(EXPATPP_FOUND)
    TARGET_LINK_LIBRARIES(bwm_video ${EXPATPP_LIBRARIES})
  ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS(exe)

#install the .h .txx and libs
INSTALL_TARGETS(/lib bwm_video)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bwm/video/ ${bwm_video_sources})
