#
# Find CMU 1394 Camera on Windows 2000
# for details see http://www-2.cs.cmu.edu/~iwan/1394/
#
# date  : 6. March 2002
# author: Jens Rittscher (rittsche@crd.ge.com)
#

if(WIN32)

  find_path( CMU1394CAMERA_VVID_INCLUDE_PATH 1394Camera.h
    ${CMAKE_SOURCE_DIR}/../1394camera6_3/include
  )

  find_library( CMU1394CAMERA_VVID_LIBRARY_PATH 1394camera
    ${CMAKE_SOURCE_DIR}/../1394camera6_3/lib
  )

  if(CMU1394CAMERA_VVID_INCLUDE_PATH)
    if(CMU1394CAMERA_VVID_LIBRARY_PATH)
      if(NOT HAS_CMU1394CAMERA)
       set(HAS_CMU1394CAMERA "YES")
       include_directories( ${CMU1394CAMERA_VVID_INCLUDE_PATH} )
       link_libraries( ${CMU1394CAMERA_VVID_LIBRARY_PATH} )
      endif()
    endif()
  endif()

endif()
