#ifndef BaseTriTensorCompute_h_
#define BaseTriTensorCompute_h_
#ifdef __GNUC__
#pragma interface
#endif
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

#include <vcl_list.h>

class HomgMatchLineSeg2D2D2D;
class HomgMatchPoint2D2D2D;
class TriTensor;

class BaseTriTensorCompute {

  // PUBLIC INTERFACE------------------------------------------------------
public:

  // Constructors/Initializers/Destructors---------------------------------

  BaseTriTensorCompute();
  ~BaseTriTensorCompute();

  // Data Access-----------------------------------------------------------

  void add_matches (vcl_list<HomgMatchLineSeg2D2D2D*> *match_list);

  void add_matches (vcl_list<HomgMatchPoint2D2D2D*> *match_list);

  void clear_matches_line (void);
  void clear_matches_point (void);

  // INTERNALS-------------------------------------------------------------

protected:
  // Data Members----------------------------------------------------------

  // the matched points.
  vcl_list<HomgMatchPoint2D2D2D*> *_matchpoint_list_ptr;

  // the matched line segments.
  vcl_list<HomgMatchLineSeg2D2D2D*> *_matchlineseg_list_ptr;
};

#endif // BaseTriTensorCompute_h_
