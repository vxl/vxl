# oxl/mvl/examples/CMakeLists.txt

LINK_LIBRARIES( mvl vul )

ADD_EXECUTABLE(mvl_multi_view_matches_example mvl_multi_view_matches_example.cxx)
ADD_EXECUTABLE(compute_FMatrix_example        compute_FMatrix_example.cxx)
#ADD_EXECUTABLE(six_point_structure            six_point_structure.cxx)
