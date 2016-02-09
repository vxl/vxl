# oxl/osl/examples/CMakeLists.txt

link_libraries( osl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil1 )

# Harris corner detector
add_executable(run_osl_harris run_osl_harris.cxx)
# Canny edge detector
add_executable(run_osl_canny  run_osl_canny.cxx)
add_executable(topology_edges topology_edges.cxx)
add_executable(break_lines    break_lines.cxx)
add_executable(fit_lines      fit_lines.cxx)
