// This is oxl/xcv/xcv_axes_limits.cxx
#include "xcv_axes_limits.h"
//:
// \file
//
// \author  Andrew Fitzgibbon <andrewfg@ed.ac.uk>
// \date    23 Apr 96
// \brief   See xcv_axes_limits.h for a description of this file.
//
// \verbatim
//   Modifications:
//     Andrew Fitzgibbon 23-APR-1996   Initial version.
//     K.Y.McGaul        26-APR-2001   Converted to vxl.
// \endverbatim
//

#include <vcl_cmath.h>
#include <vcl_iostream.h>

static bool debug = false;

//========================================================================
//: Constructor.
xcv_axes_limits::xcv_axes_limits()
{
  low  = 0.0;
  high = 1.0;
  tick_spacing = 0.1;
  tick_start = low;
  tick_end = high;
  tick_n = 10;
  exp = 1.0;
  exp_n = 0;
}

//========================================================================
//: Set the lowest and highest plotted values.
void xcv_axes_limits::set_range(double low, double high)
{
  if (low == high)
    low -= 0.1;  // to avoid low == high
    high += 0.1;
  this->low = low;
  this->high = high;
  calc_ticks();
}

//========================================================================
//: Compute the start, end and spacing for the tick marks.
void xcv_axes_limits::calc_ticks()
{
  tick_spacing = calc_tick_spacing();
  tick_start = vcl_floor(low / tick_spacing) * tick_spacing;
  tick_end = vcl_ceil(high / tick_spacing) * tick_spacing;
  tick_n = (int)((tick_end - tick_start) / tick_spacing);
  if (debug)
  {
    vcl_cout << "calc_ticks: high = " << high << vcl_endl;
    vcl_cout << "calc_ticks: low = " << low << vcl_endl;
    vcl_cout << "calc_ticks: spacing = "  << tick_spacing << vcl_endl;
    vcl_cout << "calc_ticks: start = "  << tick_start << vcl_endl;
    vcl_cout << "calc_ticks: end = "  << tick_end << vcl_endl;
    vcl_cout << "calc_ticks: n = "  << tick_n << vcl_endl;
  }
}

//========================================================================
//: Compute the spacing between tick marks.
double xcv_axes_limits::calc_tick_spacing()
{
  static double trythese[] = {0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0,
    200.0, 500.0, 1000.0, 2000.0, 5000.0};
  double l = high - low;

  exp_n = (int)vcl_floor(vcl_log10(l));
  exp = vcl_pow(10.0, exp_n);

  for (unsigned int i = 0; i < sizeof trythese / sizeof trythese[0]; ++i) {
    double tickspacing = (trythese[i] * exp);
    double num_ticks_at_this_spacing = l / tickspacing;
    if (5.0 < num_ticks_at_this_spacing && num_ticks_at_this_spacing < 15.0)
      return tickspacing;
  }
  return 1;
}
