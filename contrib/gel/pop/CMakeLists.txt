# gel/pop/CMakeLists.txt

IF(WIN32)
 ADD_DEFINITIONS(-D_AFXDLL)
 SET(CMAKE_MFC_FLAG 6)
ENDIF(WIN32)


SET(pop_sources
    pop_edge.cxx                    pop_edge.h
    pop_geometric_cost_function.cxx pop_geometric_cost_function.h
    pop_geometric_object.cxx        pop_geometric_object.h
    pop_graph_cost_function.cxx     pop_graph_cost_function.h
    pop_homography_2d.cxx           pop_homography_2d.h
    pop_manager.cxx                 pop_manager.h
    pop_object.cxx                  pop_object.h
    pop_parameter.cxx               pop_parameter.h
    pop_point_2d.cxx                pop_point_2d.h
    pop_point_3d.cxx                pop_point_3d.h
    pop_projective.cxx              pop_projective.h
    pop_rigid_3d.cxx                pop_rigid_3d.h
    pop_transform.cxx               pop_transform.h
    pop_vertex.cxx                  pop_vertex.h
)

ADD_LIBRARY(pop ${pop_sources})
TARGET_LINK_LIBRARIES( pop vcl vnl vgl vnl_algo vgl_algo)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
