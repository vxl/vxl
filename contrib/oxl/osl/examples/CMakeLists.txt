LINK_LIBRARIES( osl vbl vul vil vgl vcl )

# Harris corner detector
ADD_EXECUTABLE( run_osl_harris run_osl_harris.cxx )

ADD_EXECUTABLE( run_osl_canny run_osl_canny.cxx )
ADD_EXECUTABLE( topology_edges topology_edges.cxx )
ADD_EXECUTABLE( break_lines break_lines.cxx )
ADD_EXECUTABLE( fit_lines fit_lines.cxx )
