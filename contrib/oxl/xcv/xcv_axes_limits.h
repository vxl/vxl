// This is oxl/xcv/xcv_axes_limits.h
#ifndef xcv_axes_limits_h_
#define xcv_axes_limits_h_
//:
// \file
// \author   Andrew Fitzgibbon <andrewfg@ed.ac.uk>
// \date     23 Apr 96
// \brief Compute marks for the axes of a graph.
//
//  Given a range (from the number low to high) this class computes
//  a suitable spacing, start value and end value for tick marks
//  for the axes of a graph.
//
// \verbatim
//   Modifications:
//     Andrew Fitzgibbon 23-APR-1996   Initial version.
//     K.Y.McGaul        26-APR-2001   Converted to vxl.
// \endverbatim

class xcv_axes_limits
{
 public:
  //: Lowest value to be plotted.
  double  low;
  //: Highest value to be plotted.
  double  high;
  //: Interval between tick marks on the axes.
  double  tick_spacing;
  //: Start of axes numbering.
  double  tick_start;
  //: End of axes numbering.
  double  tick_end;
  //: Recommended number of tick marks.
  int     tick_n;
  double  exp;
  int     exp_n;

  //: Constructor.
  xcv_axes_limits();
  //: Set the lowest and highest plotted values.
  void set_range(double low, double high);
  //: Compute the start, end and spacing for the tick marks.
  void calc_ticks();
  //: Compute the spacing between tick marks.
  double calc_tick_spacing();
};

#endif // xcv_axes_limits_h_
