# segmentation detection library
# brl/bbas/bvnl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
SET(bvnl_sources
  bvnl_parabolic_interpolator.cxx   bvnl_parabolic_interpolator.h
  bvnl_quadratic_interpolator.cxx   bvnl_quadratic_interpolator.h
)
AUX_SOURCE_DIRECTORY(Templates bvnl_sources)

ADD_LIBRARY(bvnl ${bvnl_sources})

TARGET_LINK_LIBRARIES(bvnl vnl vnl_algo)
