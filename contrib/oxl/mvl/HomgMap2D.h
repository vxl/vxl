// This is oxl/mvl/HomgMap2D.h
#ifndef HomgMap2D_h_
#define HomgMap2D_h_
//:
//  \file
// \brief General 2D projective map
//
//    HomgMap2D represents a mapping between two 2D spaces.
//    It is used to represent a variety of such mappings, including
//    camera calibration and numerical conditioning.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 06 Nov 96
//-----------------------------------------------------------------------------

#include "HomgPoint2D.h"

class HomgMap2D
{
 public:
  virtual ~HomgMap2D() = 0;
  //: Transform a point from space 1 to space 2.
  virtual HomgPoint2D transform(const HomgPoint2D& p) = 0;
  //: Transform a point from space 2 to space 1.
  virtual HomgPoint2D inverse_transform(const HomgPoint2D& p) = 0;
};

#endif // HomgMap2D_h_
