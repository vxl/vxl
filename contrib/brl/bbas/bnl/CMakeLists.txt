# segmentation detection library
# brl/bbas/bnl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bnl_sources
    bnl_parabolic_interpolator.cxx  bnl_parabolic_interpolator.h
    bnl_quadratic_interpolator.cxx  bnl_quadratic_interpolator.h
)

ADD_LIBRARY(bnl ${bnl_sources})

TARGET_LINK_LIBRARIES(bnl vnl vnl_algo)

SUBDIRS(algo)
