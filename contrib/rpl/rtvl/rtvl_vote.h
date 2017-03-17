/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_vote_h
#define rtvl_vote_h

template <unsigned int N> class rtvl_voter;
template <unsigned int N> class rtvl_votee;
template <unsigned int N> class rtvl_weight;
template <unsigned int N> class rtvl_vote_internal;

template <class T, unsigned int nr, unsigned int nc> class vnl_matrix_fixed;

template <unsigned int N>
void rtvl_vote(rtvl_voter<N>& voter, rtvl_votee<N>& votee,
               rtvl_weight<N>& tvw, bool include_ball = true);

template <unsigned int N>
void rtvl_vote_component(rtvl_vote_internal<N>& vi,
                         vnl_matrix_fixed<double, N, N>& vote);

template <unsigned int N>
void
rtvl_vote_component_d(rtvl_vote_internal<N>& vi,
                      vnl_matrix_fixed<double, N, N> (&dvote)[N]);

#endif
