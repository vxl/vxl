# core/vidl/CMakeLists.txt

PROJECT(vidl)

doxygen_add_library(core/vidl
  DEPENDS core/vul core/vil
  PACKAGE core-L2
  DESCRIPTION "Video Sequence Library"
)

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindFFMPEG.cmake )
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindDC1394.cmake )
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
FIND_FILE (VIDEODEV_FOUND videodev.h /usr/include/linux/)
FIND_FILE (VIDEODEV2_FOUND videodev2.h /usr/include/linux/)

CONFIGURE_FILE( ${vxl_SOURCE_DIR}/core/vidl/vidl_config.h.in
                ${vxl_BINARY_DIR}/core/vidl/vidl_config.h )

IF( FFMPEG_FOUND )
  INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR})
ENDIF( FFMPEG_FOUND )

IF(DIRECTSHOW_FOUND)
  INCLUDE_DIRECTORIES(${DIRECTSHOW_INCLUDE_DIRS})

  # For Euresys Multicam Driver's DirectShow source filter.
  # - should I put this in a Module of its own?
  #INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow_ESF.cmake )
  FIND_PATH(DIRECTSHOW_ESF_INCLUDE_DIR ESFilter.h
            "C:/Program Files/Euresys/MultiCam/Include"
            DOC "What is the path where the file ESFilter.h can be found"
           )
  IF(DIRECTSHOW_ESF_INCLUDE_DIR)
    SET(DIRECTSHOW_ESF_FOUND "YES")
    SET(DIRECTSHOW_ESF_INCLUDE_DIRS "${DIRECTSHOW_ESF_INCLUDE_DIR}")
  ENDIF(DIRECTSHOW_ESF_INCLUDE_DIR)

  IF(DIRECTSHOW_ESF_FOUND)
    INCLUDE_DIRECTORIES(${DIRECTSHOW_ESF_INCLUDE_DIRS})
  ENDIF(DIRECTSHOW_ESF_FOUND)
ENDIF(DIRECTSHOW_FOUND)

SET(vidl_sources
    vidl_config.h.in
    vidl_pixel_format.h           vidl_pixel_format.cxx
    vidl_color.h                  vidl_color.cxx
    vidl_frame.h                  vidl_frame.cxx           vidl_frame_sptr.h
    vidl_pixel_iterator.h         vidl_pixel_iterator.cxx
                                  vidl_pixel_iterator.txx
    vidl_convert.h                vidl_convert.cxx

    vidl_istream.h                vidl_istream_sptr.h
    vidl_image_list_istream.h     vidl_image_list_istream.cxx
    vidl_ostream.h                vidl_ostream_sptr.h
    vidl_image_list_ostream.h     vidl_image_list_ostream.cxx
    vidl_iidc1394_params.h        vidl_iidc1394_params.cxx

    vidl_istream_image_resource.h vidl_istream_image_resource.cxx
   )

# These files are compiled unconditionally.  They will automatically
# compile in stub versions if ffmpeg isn't available.
SET(vidl_sources ${vidl_sources}
    vidl_ffmpeg_istream.h          vidl_ffmpeg_istream.cxx
    vidl_ffmpeg_ostream.h          vidl_ffmpeg_ostream.cxx
    vidl_ffmpeg_ostream_params.h   vidl_ffmpeg_ostream_params.cxx
   )

# These are still conditioned on ffmpeg being available. They should
# also be made unconditional.
IF( FFMPEG_FOUND )
  SET(vidl_sources ${vidl_sources}
      vidl_ffmpeg_init.h             vidl_ffmpeg_init.cxx
      vidl_ffmpeg_convert.h          vidl_ffmpeg_convert.cxx
      vidl_ffmpeg_istream_v1.txx     vidl_ffmpeg_ostream_v1.txx
      vidl_ffmpeg_istream_v2.txx     vidl_ffmpeg_ostream_v2.txx
      vidl_ffmpeg_istream_v3.txx     vidl_ffmpeg_ostream_v3.txx
                                     vidl_ffmpeg_ostream_v4.txx
     )
# Stub implementation, when ffmeg is not available
ELSE( FFMPEG_FOUND )
  SET(vidl_sources ${vidl_sources}
      vidl_ffmpeg_istream_stub.txx
      vidl_ffmpeg_ostream_stub.txx
     )
ENDIF( FFMPEG_FOUND )

IF( DC1394_FOUND )
  SET(vidl_sources ${vidl_sources}
      vidl_dc1394_istream.h          vidl_dc1394_istream.cxx
     )
ENDIF( DC1394_FOUND )

IF(DIRECTSHOW_FOUND)
  SET(vidl_sources ${vidl_sources}
      vidl_exception.h
      vidl_dshow.h                      vidl_dshow.cxx
      vidl_dshow_file_istream.h         vidl_dshow_file_istream.cxx
      vidl_dshow_live_istream.h         vidl_dshow_live_istream.cxx
      vidl_dshow_istream_params.h       vidl_dshow_istream_params.cxx
     )
  IF(DIRECTSHOW_ESF_FOUND)
    SET(vidl_sources ${vidl_sources}
        vidl_dshow_istream_params_esf.h vidl_dshow_istream_params_esf.cxx
       )
  ENDIF(DIRECTSHOW_ESF_FOUND)
ENDIF(DIRECTSHOW_FOUND)

IF(VIDEODEV_FOUND)
   SET(vidl_sources ${vidl_sources}
       vidl_v4l_params.h   vidl_v4l_params.cxx
       vidl_v4l_istream.h  vidl_v4l_istream.cxx
       vidl_v4l_ulong_fix.h
      )
ENDIF(VIDEODEV_FOUND)

IF(VIDEODEV2_FOUND)
   SET(vidl_sources ${vidl_sources}
       vidl_v4l2_devices.h       vidl_v4l2_devices.cxx
       vidl_v4l2_device.h        vidl_v4l2_device.cxx        vidl_v4l2_device_sptr.h
       vidl_v4l2_pixel_format.h  vidl_v4l2_pixel_format.cxx
       vidl_v4l2_istream.h       vidl_v4l2_istream.cxx
       vidl_v4l2_control.h       vidl_v4l2_control.cxx
      )
ENDIF(VIDEODEV2_FOUND)

AUX_SOURCE_DIRECTORY(Templates vidl_sources)

ADD_LIBRARY(vidl ${vidl_sources})
INSTALL_TARGETS(/lib vidl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vidl ${vidl_sources})

TARGET_LINK_LIBRARIES( vidl vil vul vbl )
IF( FFMPEG_FOUND )
  TARGET_LINK_LIBRARIES( vidl ${FFMPEG_LIBRARIES} )
ENDIF( FFMPEG_FOUND )

IF( DC1394_FOUND )
  TARGET_LINK_LIBRARIES( vidl ${DC1394_LIBRARIES} )
ENDIF( DC1394_FOUND )

IF(DIRECTSHOW_FOUND)
  TARGET_LINK_LIBRARIES( vidl ${DIRECTSHOW_LIBRARIES} )
ENDIF(DIRECTSHOW_FOUND)

# The pixel format conversion function table is generated by a recursive
# template metaprogam with N*N levels of recursion (for N pixel formats)
# The default template depth limit in gcc as of version 4.0.1 is 500.
# This compiler flag is needed to expand that limit.
INCLUDE(CheckCXXCompilerFlag)
SET( TEMPLATE_DEPTH_FLAG -ftemplate-depth-1000 )
CHECK_CXX_COMPILER_FLAG(${TEMPLATE_DEPTH_FLAG} HAS_TEMPLATE_DEPTH_FLAG)
IF( HAS_TEMPLATE_DEPTH_FLAG )
  SET_TARGET_PROPERTIES( vidl PROPERTIES COMPILE_FLAGS ${TEMPLATE_DEPTH_FLAG})
ENDIF( HAS_TEMPLATE_DEPTH_FLAG )

# vgui add on library
SUBDIRS( gui )

IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
