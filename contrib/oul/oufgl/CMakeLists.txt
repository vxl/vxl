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

add_library(oufgl ${oufgl_sources})
install_targets(/lib oufgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/oul/oufgl ${oufgl_sources})
target_link_libraries( oufgl vil vil1 rt)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

add_subdirectory(examples)
