# vxl/vgl/algo/CMakeLists.txt

SET( vgl_algo_sources
  # need at least one file for SunPro
  vgl_algo.cxx

  vgl_rtree.txx vgl_rtree.h
  vgl_homg_operators_1d.txx vgl_homg_operators_1d.h
  vgl_homg_operators_2d.txx vgl_homg_operators_2d.h
  vgl_homg_operators_3d.txx vgl_homg_operators_3d.h
)

AUX_SOURCE_DIRECTORY(Templates vgl_algo_sources)

ADD_LIBRARY(vgl_algo ${vgl_algo_sources})
TARGET_LINK_LIBRARIES( vgl_algo vgl vnl_algo )
