# core/vidl/CMakeLists.txt

project(vidl)

doxygen_add_library(core/vidl
  DEPENDS core/vul core/vil
  PACKAGE core-L2
  DESCRIPTION "Video Sequence Library"
)

option(WITH_FFMPEG "Include FFMPEG support if available" ON)
if(WITH_FFMPEG)
include( ${VXL_CMAKE_DIR}/NewCMake/FindFFMPEG.cmake )
endif(WITH_FFMPEG)

include( ${VXL_CMAKE_DIR}/NewCMake/FindDC1394.cmake )
include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
find_file(VIDEODEV_FOUND videodev.h /usr/include/linux/)
find_file(VIDEODEV2_FOUND videodev2.h /usr/include/linux/)

vxl_configure_file( ${CMAKE_CURRENT_LIST_DIR}/vidl_config.h.in
                    ${PROJECT_BINARY_DIR}/vidl_config.h include/vxl/core/vidl )

if( FFMPEG_FOUND )
  # Mark FFMPEG is found.  In future we can even export
  # FFMPEG setting for external use.
  SET(CORE_VIDEO_FFMPEG_FOUND ON CACHE INTERNAL "FFMPEG library has been found and is used in vidl")
  include_directories(${FFMPEG_INCLUDE_DIR})
endif()

if( DC1394_FOUND )
  include_directories( ${DC1394_INCLUDE_DIR} )
endif()

if(DIRECTSHOW_FOUND)
  include_directories(${DIRECTSHOW_INCLUDE_DIRS})

  # For Euresys Multicam Driver's DirectShow source filter.
  # - should I put this in a Module of its own?
  #include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow_ESF.cmake )
  find_path(DIRECTSHOW_ESF_INCLUDE_DIR ESFilter.h
            "C:/Program Files/Euresys/MultiCam/Include"
            DOC "What is the path where the file ESFilter.h can be found"
           )
  if(DIRECTSHOW_ESF_INCLUDE_DIR)
    set(DIRECTSHOW_ESF_FOUND "YES")
    set(DIRECTSHOW_ESF_INCLUDE_DIRS "${DIRECTSHOW_ESF_INCLUDE_DIR}")
  endif()

  if(DIRECTSHOW_ESF_FOUND)
    include_directories(${DIRECTSHOW_ESF_INCLUDE_DIRS})
  endif()
endif()

set(vidl_sources
    vidl_pixel_format.h           vidl_pixel_format.cxx
    vidl_color.h                  vidl_color.cxx
    vidl_frame.h                  vidl_frame.cxx           vidl_frame_sptr.h
    vidl_pixel_iterator.h         vidl_pixel_iterator.cxx
                                  vidl_pixel_iterator.hxx
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
set(vidl_sources ${vidl_sources}
    vidl_ffmpeg_istream.h          vidl_ffmpeg_istream.cxx
    vidl_ffmpeg_ostream.h          vidl_ffmpeg_ostream.cxx
    vidl_ffmpeg_ostream_params.h   vidl_ffmpeg_ostream_params.cxx
   )

# These are still conditioned on ffmpeg being available. They should
# also be made unconditional.
if( FFMPEG_FOUND )
  set(vidl_sources ${vidl_sources}
      vidl_ffmpeg_init.h             vidl_ffmpeg_init.cxx
      vidl_ffmpeg_convert.h          vidl_ffmpeg_convert.cxx
      vidl_ffmpeg_istream_v1.hxx     vidl_ffmpeg_ostream_v1.hxx
      vidl_ffmpeg_istream_v2.hxx     vidl_ffmpeg_ostream_v2.hxx
      vidl_ffmpeg_istream_v3.hxx     vidl_ffmpeg_ostream_v3.hxx
                                     vidl_ffmpeg_ostream_v4.hxx
      vidl_ffmpeg_istream_v0.9.hxx   vidl_ffmpeg_ostream_v0.9.hxx
                                     vidl_ffmpeg_ostream_v0.11.hxx
      vidl_ffmpeg_istream_v56.hxx    vidl_ffmpeg_ostream_v56.hxx
      vidl_ffmpeg_pixel_format.h     vidl_ffmpeg_pixel_format.cxx
     )
# Stub implementation, when ffmeg is not available
else()
  set(vidl_sources ${vidl_sources}
      vidl_ffmpeg_istream_stub.hxx
      vidl_ffmpeg_ostream_stub.hxx
     )
endif()

if( DC1394_FOUND )
  set(vidl_sources ${vidl_sources}
      vidl_dc1394_istream.h          vidl_dc1394_istream.cxx
     )
endif()

if(DIRECTSHOW_FOUND)
  set(vidl_sources ${vidl_sources}
      vidl_exception.h
      vidl_dshow.h                      vidl_dshow.cxx
      vidl_dshow_file_istream.h         vidl_dshow_file_istream.cxx
      vidl_dshow_live_istream.h         vidl_dshow_live_istream.cxx
      vidl_dshow_istream_params.h       vidl_dshow_istream_params.cxx
     )
  if(DIRECTSHOW_ESF_FOUND)
    set(vidl_sources ${vidl_sources}
        vidl_dshow_istream_params_esf.h vidl_dshow_istream_params_esf.cxx
       )
  endif()
endif()

if(VIDEODEV_FOUND)
   set(vidl_sources ${vidl_sources}
       vidl_v4l_params.h   vidl_v4l_params.cxx
       vidl_v4l_istream.h  vidl_v4l_istream.cxx
      )
endif()

if(VIDEODEV2_FOUND)
   set(vidl_sources ${vidl_sources}
       vidl_v4l2_devices.h       vidl_v4l2_devices.cxx
       vidl_v4l2_device.h        vidl_v4l2_device.cxx        vidl_v4l2_device_sptr.h
       vidl_v4l2_pixel_format.h  vidl_v4l2_pixel_format.cxx
       vidl_v4l2_istream.h       vidl_v4l2_istream.cxx
       vidl_v4l2_control.h       vidl_v4l2_control.cxx
      )
endif()

aux_source_directory(Templates vidl_sources)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vidl LIBRARY_SOURCES ${vidl_sources})
set(CURR_LIB_NAME vidl)
set_vxl_library_properties(
     TARGET_NAME ${VXL_LIB_PREFIX}${CURR_LIB_NAME}
     BASE_NAME ${CURR_LIB_NAME}
     EXPORT_HEADER_FILE ${VXLCORE_BINARY_INCLUDE_DIR}/${CURR_LIB_NAME}/${CURR_LIB_NAME}_export.h
     INSTALL_DIR   ${VXL_INSTALL_INCLUDE_DIR}/core/${CURR_LIB_NAME}
     USE_HIDDEN_VISIBILITY
)

target_link_libraries( ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl )
if( FFMPEG_FOUND )
  target_link_libraries( ${VXL_LIB_PREFIX}vidl ${FFMPEG_LIBRARIES} )
endif()

if( DC1394_FOUND )
  target_link_libraries( ${VXL_LIB_PREFIX}vidl ${DC1394_LIBRARIES} )
endif()

if(DIRECTSHOW_FOUND)
  target_link_libraries( ${VXL_LIB_PREFIX}vidl ${DIRECTSHOW_LIBRARIES} )
endif()

# The pixel format conversion function table is generated by a recursive
# template metaprogam with N*N levels of recursion (for N pixel formats)
# The default template depth limit in gcc as of version 4.0.1 is 500.
# This compiler flag is needed to expand that limit.
include(CheckCXXCompilerFlag)
set( TEMPLATE_DEPTH_FLAG -ftemplate-depth-1000 )
CHECK_CXX_COMPILER_FLAG(${TEMPLATE_DEPTH_FLAG} HAS_TEMPLATE_DEPTH_FLAG)
if( HAS_TEMPLATE_DEPTH_FLAG )
  set_target_properties( ${VXL_LIB_PREFIX}vidl PROPERTIES COMPILE_FLAGS ${TEMPLATE_DEPTH_FLAG})
endif()

# vgui add on library
add_subdirectory(gui)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if( VXL_BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()
