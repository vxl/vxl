# This is mul/mipa/tools/CMakeLists.txt

link_libraries( mipa vimt_algo ${VXL_LIB_PREFIX}vul )
add_executable( mipa_orient_histo mipa_orient_histo.cxx )
