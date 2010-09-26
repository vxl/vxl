# oul/oufgl/CMakeLists.txt

doxygen_add_library(contrib/oul/oufgl
  DEPENDS core/vil1
  PACKAGE contrib/oul
  DESCRIPTION "Frame grabber library (for linux)"
  )

SET(oufgl_sources
    frame_grabber_v4l.h    frame_grabber_v4l.cxx
    frame_grabber_vil.h    frame_grabber_vil.cxx

    asyncio.h
    frame_grabber.h
)

ADD_LIBRARY(oufgl ${oufgl_sources})
INSTALL_TARGETS(/lib oufgl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/oul/oufgl ${oufgl_sources})
TARGET_LINK_LIBRARIES( oufgl vil vil1 rt)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS(examples)
