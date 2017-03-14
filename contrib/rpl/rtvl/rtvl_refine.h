/* Copyright 2007-2010 Brad King
   Copyright 2007-2008 Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rtvl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rtvl_refine_h
#define rtvl_refine_h

template <unsigned int N> class rtvl_refine_internal;
template <unsigned int N> class rtvl_tokens;

template <unsigned int N>
class rtvl_refine
{
public:
  rtvl_refine(unsigned int num_points, double* points);
  ~rtvl_refine();

  /** Set the fraction of the scale to use as the masking sphere size
      during downsampling.  */
  void set_mask_size(double f);

  /** Get refined tokens for the current scale.  */
  void get_tokens(rtvl_tokens<N>& tokens) const;

  /** Get number of votes cast during token refinement.  */
  unsigned int get_vote_count() const;

  /** Move to the next-larger scale.  */
  bool next_scale();
private:
  friend class rtvl_refine_internal<N>;
  typedef rtvl_refine_internal<N> internal_type;
  internal_type* internal_;
};

#endif
