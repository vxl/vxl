// This is oxl/mvl/TriTensorCompute.h
#ifndef BaseTriTensorCompute_h_
#define BaseTriTensorCompute_h_
//:
// \file
//
// A class to generate a Trifocal Tensor from point/line segment matched triplets,
// with separate functions for the linear/non-linear computations.
// Linear computation uses RANSAC and is robust to the presence of
// incorrect matches.
//
// \author
//             Paul Beardsley, 29.03.96
//             Oxford University, UK
//---------------------------------------------------------------------------

#include <iostream>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HomgMatchLineSeg2D2D2D;
class HomgMatchPoint2D2D2D;
class TriTensor;

class BaseTriTensorCompute
{
 public:

  // Constructors/Initializers/Destructors---------------------------------

  BaseTriTensorCompute();
  ~BaseTriTensorCompute();

  // Data Access-----------------------------------------------------------

  void add_matches (std::list<HomgMatchLineSeg2D2D2D*> *match_list);

  void add_matches (std::list<HomgMatchPoint2D2D2D*> *match_list);

  void clear_matches_line (void);
  void clear_matches_point (void);

  // INTERNALS-------------------------------------------------------------

 protected:
  // Data Members----------------------------------------------------------

  // the matched points.
  std::list<HomgMatchPoint2D2D2D*> *matchpoint_list_ptr_;

  // the matched line segments.
  std::list<HomgMatchLineSeg2D2D2D*> *matchlineseg_list_ptr_;
};

#endif // BaseTriTensorCompute_h_
