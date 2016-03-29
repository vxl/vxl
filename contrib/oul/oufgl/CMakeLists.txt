# oul/oufgl/CMakeLists.txt

doxygen_add_library(contrib/oul/oufgl
  DEPENDS core/vil1
  PACKAGE contrib/oul
  DESCRIPTION "Frame grabber library (for linux)"
  )

set(oufgl_sources
    frame_grabber_v4l.h    frame_grabber_v4l.cxx
    frame_grabber_vil.h    frame_grabber_vil.cxx

    asyncio.h
    frame_grabber.h
)

vxl_add_library(LIBRARY_NAME oufgl LIBRARY_SOURCES ${oufgl_sources})
target_link_libraries( oufgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil1 rt)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(examples)
