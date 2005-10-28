# brl/bbas/bnl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bnl_sources
    bnl_parabolic_interpolator.cxx  bnl_parabolic_interpolator.h
    bnl_quadratic_interpolator.cxx  bnl_quadratic_interpolator.h

    bnl_fst_grid.cxx                bnl_fst_grid.h
    bnl_fst_spectrum.cxx            bnl_fst_spectrum.h

   )

ADD_LIBRARY(bnl ${bnl_sources})
INSTALL_TARGETS(/lib bnl)
INSTALL_NOBASE_HEADER_FILES(/include/contrib/brl/bbas/bnl ${bnl_sources})

TARGET_LINK_LIBRARIES(bnl vnl vnl_algo b3p_fst)

SUBDIRS(algo)

# tests code
#SUBDIRS(tests)
