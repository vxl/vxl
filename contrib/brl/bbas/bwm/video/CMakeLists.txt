# world modeling application
# contrib/bwm/video/CMakeLists.txt


SET(bwm_video_sources
   bwm_video_cam_istream.cxx        bwm_video_cam_istream.h        
   bwm_video_cam_ostream.cxx        bwm_video_cam_ostream.h        
   bwm_video_corr.cxx               bwm_video_corr.h
   bwm_video_site_io.cxx            bwm_video_site_io.h
                                    bwm_video_site_io_defs.h
   bwm_video_corr_processor.cxx     bwm_video_corr_processor.h
)

AUX_SOURCE_DIRECTORY(Templates bwm_video_sources)
ADD_LIBRARY(bwm_video ${bwm_video_sources})

TARGET_LINK_LIBRARIES( bwm_video vidl2 brip sdet vil vbl vgl vgl_algo vnl_algo vnl bsta vsol vpgl vpgl_algo bgeo)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)




