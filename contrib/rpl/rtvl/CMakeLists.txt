# Copyright 2007-2010 Brad King
# Copyright 2007-2008 Chuck Stewart
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file rtvl_license_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)

set(rtvl_sources
  rtvl_intpow.h
  rtvl_terms.h
  rtvl_tokens.h
  rtvl_vote.h                         rtvl_vote.hxx
  rtvl_votee.h                        rtvl_votee.hxx
  rtvl_votee_d.h                      rtvl_votee_d.hxx
  rtvl_voter.h                        rtvl_voter.hxx
  rtvl_refine.h                       rtvl_refine.hxx
  rtvl_tensor.h                       rtvl_tensor.hxx
  rtvl_tensor_d.h                     rtvl_tensor_d.hxx
  rtvl_weight.h
  rtvl_weight_original.h              rtvl_weight_original.hxx
  rtvl_weight_smooth.h                rtvl_weight_smooth.hxx

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
  # Templates/rtvl_refine+3-.cxx
   Templates/rtvl_tensor+3-.cxx
   Templates/rtvl_tensor_d+3-.cxx
   Templates/rtvl_weight_original+3-.cxx
   Templates/rtvl_weight_smooth+3-.cxx
  )

vxl_add_library(LIBRARY_NAME rtvl LIBRARY_SOURCES ${rtvl_sources})
target_link_libraries(rtvl rgtl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
