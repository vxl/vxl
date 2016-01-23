# mul/mvl2/CMakeLists.txt

doxygen_add_library(contrib/mul/mvl2
  DEPENDS core/vil core/vul
  PACKAGE contrib/mul
  DESCRIPTION "Classes for video file support"
  )

set(mvl2_sources
    mvl2_video_writer.h
    mvl2_video_reader.h        mvl2_video_reader.cxx
    mvl2_video_from_sequence.h mvl2_video_from_sequence.cxx
)

if(WIN32)
  if(NOT CYGWIN)
    # Might be a good idea to set(AVI_MS_NATIVE) here so below we
    # can properly distinguish between MS native AVI support and AVI
    # support from AVIFile.
    set(AVIFILE_FOUND "YES")
    set(AVIFILE_LIBRARIES "vfw32")
  endif(NOT CYGWIN)
endif(WIN32)

if(NOT AVIFILE_FOUND)
  find_package(AVIFile)
  if(AVIFILE_FOUND)
    include_directories( ${AVIFILE_INCLUDE_DIR} )
    add_definitions(${AVIFILE_DEFINITIONS} )
  endif(AVIFILE_FOUND)
endif(NOT AVIFILE_FOUND)

if(AVIFILE_FOUND)
  set(mvl2_sources ${mvl2_sources}
      mvl2_image_format_plugin.h mvl2_image_format_plugin.cxx
      mvl2_video_from_avi.h
      mvl2_video_to_avi.h
     )
  if(WIN32)
    set(mvl2_sources ${mvl2_sources}
        mvl2_video_from_avi_windows.h mvl2_video_from_avi_windows.cxx
        mvl2_video_to_avi_windows.h   mvl2_video_to_avi_windows.cxx
       )
  else(WIN32)
    set(mvl2_sources ${mvl2_sources}
        mvl2_video_from_avi_linux.h mvl2_video_from_avi_linux.cxx
        mvl2_video_to_avi_linux.h   mvl2_video_to_avi_linux.cxx
       )
  endif(WIN32)
else(AVIFILE_FOUND)
  set(AVIFILE_LIBRARIES "")
endif(AVIFILE_FOUND)

add_library(mvl2 ${mvl2_sources})
install_targets(/lib mvl2)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/mul/mvl2 ${mvl2_sources})
target_link_libraries( mvl2 vil vul ${AVIFILE_LIBRARIES} )

if(AVIFILE_FOUND)
  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif( BUILD_TESTING )

  add_subdirectory(examples)
endif(AVIFILE_FOUND)
