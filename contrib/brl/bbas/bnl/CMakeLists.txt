# brl/bbas/bnl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas ${BRL_INCLUDE_DIR}/b3p)

SET(bnl_sources
    bnl_parabolic_interpolator.cxx  bnl_parabolic_interpolator.h
    bnl_quadratic_interpolator.cxx  bnl_quadratic_interpolator.h

    # classes for numerical integral
                                    bnl_integrant_fnct.h
                                    bnl_analytic_integrant.h
    bnl_definite_integral.cxx       bnl_definite_integral.h
)


ADD_LIBRARY(bnl ${bnl_sources})

TARGET_LINK_LIBRARIES(bnl vnl vnl_algo)

SUBDIRS(algo)

# tests code
SUBDIRS(tests)

