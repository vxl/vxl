# gel/gtrl/CMakeLists.txt

SET(gtrl_sources
                        gtrl_netlib_triangle.h
 gtrl_triangulation.cxx gtrl_triangulation.h
 gtrl_triangle.cxx      gtrl_triangle.h
 gtrl_vertex.cxx        gtrl_vertex.h
 gtrl_polygon.cxx       gtrl_polygon.h
)

AUX_SOURCE_DIRECTORY(Templates gtrl_sources)

ADD_LIBRARY(gtrl ${gtrl_sources})
