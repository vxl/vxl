#
# ./contrib/mul//mvl2/CMakeLists.txt
#
#   Franck Bettinger
#


SET(mvl2_sources
  mvl2_video_writer.h mvl2_video_writer.cxx
  mvl2_video_reader.h mvl2_video_reader.cxx
  mvl2_video_from_sequence.h mvl2_video_from_sequence.cxx
)

ADD_LIBRARY(mvl2 mvl2_sources)

TARGET_LINK_LIBRARIES( mvl2 
  mbl mvl2 vimt vul
)
    
IF (NOT WIN32)
  INCLUDE(${MODULE_PATH}/NewCMake/FindAVIFile.cmake)
  INCLUDE_DIRECTORIES( ${AVIFILE_INCLUDE_DIR} )
  ADD_DEFINITIONS (${AVIFILE_DEFINITIONS} )


  IF (AVIFILE_FOUND)
    SET(mvl2_sources ${mvl2_sources}
      mvl2_image_format_plugin.h mvl2_image_format_plugin.cxx
      mvl2_video_from_avi.h
      mvl2_video_from_avi_linux.h mvl2_video_from_avi_linux.cxx
      mvl2_video_to_avi.h
      mvl2_video_to_avi_linux.h	  mvl2_video_to_avi_linux.cxx
    )
    TARGET_LINK_LIBRARIES( mvl2 
      mbl mvl2 vil2 ${AVIFILE_LIBRARIES} 
    )
  ELSE(AVIFILE_FOUND)
    TARGET_LINK_LIBRARIES( mvl2 
      mbl mvl2 vil2
    )
  ENDIF(AVIFILE_FOUND)

ELSE (NOT WIN32)
  SET(mvl2_sources ${mvl2_sources}
      mvl2_image_format_plugin.h mvl2_image_format_plugin.cxx
      mvl2_video_from_avi.h
      mvl2_video_from_avi_windows.h mvl2_video_from_avi_windows.cxx
      mvl2_video_to_avi.h
      mvl2_video_to_avi_windows.h mvl2_video_to_avi_windows.cxx
  )
  TARGET_LINK_LIBRARIES( mvl2 
    mbl mvl2 vil2
  )
ENDIF (NOT WIN32)



IF (WIN32)
  SUBDIRS(examples)
ELSE (WIN32)
  IF (AVIFILE_FOUND)
    SUBDIRS(examples)
  ENDIF (AVIFILE_FOUND)
ENDIF (WIN32)
