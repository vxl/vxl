# mul/mvl2/CMakeLists.txt

SET(mvl2_sources
    mvl2_video_writer.h        mvl2_video_writer.cxx
    mvl2_video_reader.h        mvl2_video_reader.cxx
    mvl2_video_from_sequence.h mvl2_video_from_sequence.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    # Might be a good idea to SET (AVI_MS_NATIVE) here so below we
    # can properly distinguish between MS native AVI support and AVI
    # support from AVIFile.
    SET(AVIFILE_FOUND "YES")
    SET(AVIFILE_LIBRARIES "vfw32")
  ENDIF (NOT CYGWIN)
ENDIF (WIN32)

IF (NOT AVIFILE_FOUND)
  INCLUDE(${CMAKE_ROOT}/Modules/FindAVIFile.cmake)
  IF(AVIFILE_FOUND)
    INCLUDE_DIRECTORIES( ${AVIFILE_INCLUDE_DIR} )
    ADD_DEFINITIONS (${AVIFILE_DEFINITIONS} )
  ENDIF(AVIFILE_FOUND)
ENDIF (NOT AVIFILE_FOUND)

IF (AVIFILE_FOUND)
  SET(mvl2_sources ${mvl2_sources}
      mvl2_image_format_plugin.h mvl2_image_format_plugin.cxx
      mvl2_video_from_avi.h
      mvl2_video_to_avi.h
     )
  IF (WIN32)
    SET(mvl2_sources ${mvl2_sources}
        mvl2_video_from_avi_windows.h mvl2_video_from_avi_windows.cxx
        mvl2_video_to_avi_windows.h   mvl2_video_to_avi_windows.cxx
       )
  ELSE (WIN32)
    SET(mvl2_sources ${mvl2_sources}
        mvl2_video_from_avi_linux.h mvl2_video_from_avi_linux.cxx
        mvl2_video_to_avi_linux.h   mvl2_video_to_avi_linux.cxx
       )
  ENDIF (WIN32)
ELSE (AVIFILE_FOUND)
  SET(AVIFILE_LIBRARIES "")
ENDIF (AVIFILE_FOUND)


ADD_LIBRARY(mvl2 ${mvl2_sources})

TARGET_LINK_LIBRARIES( mvl2 vil vul ${AVIFILE_LIBRARIES} )

IF (AVIFILE_FOUND)
  SUBDIRS(examples)
ENDIF (AVIFILE_FOUND)
