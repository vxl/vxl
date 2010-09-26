# Copyright 2007-2010 Brad King
# Copyright 2007-2008 Chuck Stewart
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file rtvl_license_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

SET(rtvl_sources
  rtvl_intpow.hxx
  rtvl_terms.hxx
  rtvl_tokens.hxx
  rtvl_vote.hxx                         rtvl_vote.txx
  rtvl_votee.hxx                        rtvl_votee.txx
  rtvl_votee_d.hxx                      rtvl_votee_d.txx
  rtvl_voter.hxx                        rtvl_voter.txx
  rtvl_refine.hxx                       rtvl_refine.txx
  rtvl_tensor.hxx                       rtvl_tensor.txx
  rtvl_tensor_d.hxx                     rtvl_tensor_d.txx
  rtvl_weight.hxx
  rtvl_weight_original.hxx              rtvl_weight_original.txx
  rtvl_weight_smooth.hxx                rtvl_weight_smooth.txx

  Templates/rtvl_vote+2-.cxx
  Templates/rtvl_votee+2-.cxx
  Templates/rtvl_votee_d+2-.cxx
  Templates/rtvl_voter+2-.cxx
  Templates/rtvl_refine+2-.cxx
  Templates/rtvl_tensor+2-.cxx
  Templates/rtvl_tensor_d+2-.cxx
  Templates/rtvl_weight_original+2-.cxx
  Templates/rtvl_weight_smooth+2-.cxx

  Templates/rtvl_vote+3-.cxx
  Templates/rtvl_votee+3-.cxx
  Templates/rtvl_votee_d+3-.cxx
  Templates/rtvl_voter+3-.cxx
  Templates/rtvl_refine+3-.cxx
  Templates/rtvl_tensor+3-.cxx
  Templates/rtvl_tensor_d+3-.cxx
  Templates/rtvl_weight_original+3-.cxx
  Templates/rtvl_weight_smooth+3-.cxx
  )

ADD_LIBRARY(rtvl ${rtvl_sources})
TARGET_LINK_LIBRARIES(rtvl rgtl vnl_algo vnl)

INSTALL_TARGETS(/lib rtvl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/rpl/rtvl ${rtvl_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
