# ./oul/oufgl/CMakeLists.txt

SET(oufgl_sources
  asyncio.h                 asyncio.cxx
  frame_grabber_pxc200.h    frame_grabber_pxc200.cxx
  frame_grabber_v4l.h    frame_grabber_v4l.cxx
  frame_grabber_vil.h    frame_grabber_vil.cxx

  bt848.h
  bt848-addons.h
  frame_grabber.h
  pxc200.h
  sysdep.h
)

ADD_LIBRARY(oufgl ${oufgl_sources})
TARGET_LINK_LIBRARIES( oufgl vil vil1 rt)

SUBDIRS(examples)
