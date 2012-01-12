# ./gel/vgel/examples/CMakeLists.txt

SET( kl_tracker_sources
  kl_tracker.cxx
)

ADD_EXECUTABLE( kl_tracker ${kl_tracker_sources} )

TARGET_LINK_LIBRARIES( kl_tracker vgel vtol vil )
