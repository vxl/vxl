# ./oul/oufgl/CMakeLists.txt

SET(oufgl_sources
  frame_grabber_v4l.h    frame_grabber_v4l.cxx
  frame_grabber_vil.h    frame_grabber_vil.cxx

  frame_grabber.h
)

ADD_LIBRARY(oufgl ${oufgl_sources})
TARGET_LINK_LIBRARIES( oufgl vil vil1 rt)

SUBDIRS(examples)
