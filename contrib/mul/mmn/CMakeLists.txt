# mul/mmn/CMakeLists.txt

SET(mmn_sources
  mmn_arc.h
  mmn_dependancy.h
  mmn_graph_rep1.h    mmn_graph_rep1.cxx
  mmn_dp_solver.h     mmn_dp_solver.cxx
)
AUX_SOURCE_DIRECTORY(Templates mmn_sources)

ADD_LIBRARY(mmn ${mmn_sources})
TARGET_LINK_LIBRARIES(mmn vimt vnl vil_algo vil vsl)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

IF(BUILD_MUL_TOOLS)
#  SUBDIRS(tools)
ENDIF(BUILD_MUL_TOOLS)

