# ./oul/oufgl/CMakeLists.txt

ADD_LIBRARY(oufgl oufgl_sources)

SOURCE_FILES(oufgl_sources
  asyncio.h                 asyncio.cxx
  frame_grabber_pxc200.h    frame_grabber_pxc200.cxx

  bt848.h
  bt848-addons.h
  frame_grabber.h
  pxc200.h
  sysdep.h
)
