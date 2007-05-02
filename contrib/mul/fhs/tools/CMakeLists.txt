# This is mul/fhs/tools/CMakeLists.txt

LINK_LIBRARIES( fhs mbl vimt_algo vimt vil_algo vil vul )

ADD_EXECUTABLE( find_matches find_matches.cxx )
ADD_EXECUTABLE( mr_find_matches mr_find_matches.cxx )
