# ./gel/vgel/examples/CMakeLists.txt

ADD_EXECUTABLE( kl_tracker kl_tracker_sources )

SOURCE_FILES( kl_tracker_sources
  kl_tracker.cxx
)

INCLUDE(${allvxl_SOURCE_DIR}/gel/vgel/CMakeListsLink.txt)
