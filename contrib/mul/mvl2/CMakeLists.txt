# ./contrib/mul/mvl2/CMakeLists.txt
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
  mbl vimt vul
)
    
IF (NOT WIN32)
  INCLUDE(${MODULE_PATH}/NewCMake/FindAVIFile.cmake)
  INCLUDE_DIRECTORIES( ${AVIFILE_INCLUDE_DIR} )
  ADD_DEFINITIONS (${AVIFILE_DEFINITIONS} )
ELSE (NOT WIN32)
  SET(AVIFILE_FOUND "YES")
  SET(AVIFILE_LIBRARIES "")
ENDIF (NOT WIN32)

IF (AVIFILE_FOUND)
  SET(mvl2_sources ${mvl2_sources}
      mvl2_image_format_plugin.h mvl2_image_format_plugin.cxx
      mvl2_video_from_avi.h
      mvl2_video_to_avi.h
     )
  IF (WIN32)
    SET(mvl2_sources ${mvl2_sources}
        mvl2_video_from_avi_windows.h mvl2_video_from_avi_windows.cxx
        mvl2_video_to_avi_windows.h mvl2_video_to_avi_windows.cxx
       )
  ELSE (WIN32)
    SET(mvl2_sources ${mvl2_sources}
        mvl2_video_from_avi_linux.h mvl2_video_from_avi_linux.cxx
        mvl2_video_to_avi_linux.h	  mvl2_video_to_avi_linux.cxx
       )
  ENDIF (WIN32)
ELSE (AVIFILE_FOUND)
  SET(AVIFILE_LIBRARIES "")
ENDIF (AVIFILE_FOUND)

TARGET_LINK_LIBRARIES( mvl2 
                       mbl vil2 ${AVIFILE_LIBRARIES} 
                     )

IF (AVIFILE_FOUND)
  SUBDIRS(examples)
ENDIF (AVIFILE_FOUND)
