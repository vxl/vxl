# ./gel/gtrl/CMakeLists.txt
#

ADD_LIBRARY(gtrl gtrl_sources)

SOURCE_FILES( gtrl_sources
  gtrl_triangulation
  gtrl_triangle
  gtrl_vertex
  gtrl_polygon
)

AUX_SOURCE_DIRECTORY(Templates gtrl_sources)
