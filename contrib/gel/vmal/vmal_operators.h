// This is gel/vmal/vmal_operators.h
#ifndef vmal_operators_h_
#define vmal_operators_h_
//--------------------------------------------------------------------------------
//:
//  \file
//
// \author
//   L. Guichard
// \verbatim
// Modifications:
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim
//--------------------------------------------------------------------------------
#include <vnl/vnl_double_3.h>

class vmal_operators
{
 public:

  vmal_operators();

  ~vmal_operators();
//-----------------------------------------------------------------------------
//: Project the point (x0,y0) on the segment [(ax,ay),(bx,by)].
// (x,y) is the projected point.  It returns the orthogonal distance.
// (x,y)=(-1,-1) if the projected point does not belong to the segment.
//-----------------------------------------------------------------------------
  static double project_point(double x0,double y0,
                              double ax,double ay,
                              double bx,double by,
                              double *x,double *y);

//-----------------------------------------------------------------------------
//: Project the point x on the segment [a,b].
// px is the projected point. It returns the orthogonal distance.
// px=(-1,-1) if the projected point does not belong to the segment.
//-----------------------------------------------------------------------------
  static bool project_point(vnl_double_3 &x,
                            vnl_double_3 &a,
                            vnl_double_3 &b,
                            vnl_double_3 &px);

//-----------------------------------------------------------------------------
//: Look if the two segments cross.
//-----------------------------------------------------------------------------
  static bool cross_seg(double f1x,double f1y,double f2x,double f2y,//first segment
            double s1x,double s1y,double s2x,double s2y);//second segment

//-----------------------------------------------------------------------------
//: Compute the crossing point "inter" of a segment and a line.
// alpha is the angle between the two.
// It returns true iff the line crosses the segment inside [start_seg,end_seg].
//-----------------------------------------------------------------------------
  static bool line_cross_seg(vnl_double_3 start_seg, vnl_double_3 end_seg,
                             vnl_double_3 & line_equ, vnl_double_3 & inter,
                             double &alpha);
};

#endif //vmal_operators_h_
