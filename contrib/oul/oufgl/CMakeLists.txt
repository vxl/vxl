# ./oul/oufgl/CMakeLists.txt

ADD_LIBRARY(oufgl oufgl_sources)

SOURCE_FILES(oufgl_sources
  asyncio.cpp
  frame_grabber_pxc200.cc
)
