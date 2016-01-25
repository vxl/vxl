# This is mul/fhs/tools/CMakeLists.txt

link_libraries( fhs mbl vimt_algo )

add_executable( find_matches find_matches.cxx )
add_executable( mr_find_matches mr_find_matches.cxx )
add_executable( fhs_match_tree_model fhs_match_tree_model.cxx )
