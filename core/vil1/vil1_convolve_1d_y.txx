// This is core/vil1/vil1_convolve_1d_y.txx
#ifndef vil1_convolve_1d_y_txx_
#define vil1_convolve_1d_y_txx_

#ifndef fsm_dont_croak // this file should only be #included from vil1_convolve_1d.txx
croak
#endif

#include "vil1_convolve.h"
#include <vcl_cstdlib.h> // for vcl_abort()

template <class I1, class I2, class AC, class O>
void vil1_convolve_1d_y(vil1_convolve_signal_1d<I1 const> const &kernel,
                        vil1_convolve_signal_2d<I2 const> const &input,
                        AC *,
                        vil1_convolve_signal_2d<O> const &output,
                        vil1_convolve_boundary_option b,
                        vil1_convolve_boundary_option e)
{
  // compute ranges of i, x, y here.
  int i0 = kernel.begin_-kernel.origin_;
  int i1 = kernel.end_  -kernel.origin_;

  int x0 = output.beginx_-output.originx_;
  int x1 = output.endx_  -output.originx_;

  int y0 = output.beginy_-output.originy_;
  int y1 = output.endy_  -output.originy_;

  // compute total weight of the kernel.
  // FIXME assumes non-negative kernel.
  AC total_weight = 0;
  for (int i=i0; i<i1; ++i)
    total_weight += AC(value1d(kernel, i));


  // this is not very efficient at the moment, but my main
  // concern for now is that it works correctly.
  for (int y=y0; y<y1; ++y) {
    for (int x=x0; x<x1; ++x) {
      AC ac = 0; // accumulated "kernel * input" terms.
      AC wt = 0; // accumulated "kernel" terms.
      bool zero = false;

      for (int i=i0; i<i1 && !zero; ++i) {
        // value of kernel at i :
        AC kval = AC(value1d(kernel, i));

        int yy = y-i;
        if (yy < y0) switch (b) {
        case vil1_convolve_no_extend:
          zero = true; /*FIXME*/
          break;
        case vil1_convolve_zero_extend:
          wt += kval;
          break;
        case vil1_convolve_constant_extend:
          ac += kval * AC(value2d(input, x, y0));
          wt += kval;
          break;
        case vil1_convolve_periodic_extend:
          ac += kval * AC(value2d(input, x, yy+(y1-y0)));
          wt += kval;
          break;
        case vil1_convolve_reflect_extend:
          ac += kval * AC(value2d(input, x, 2*y0-yy));
          wt += kval;
          break;
        case vil1_convolve_trim:
          break;
        default:
          vcl_abort();
          break;
        }

        else if (yy >= y1) switch (e) {
        case vil1_convolve_no_extend:
          zero = true; /*FIXME*/
          break;
        case vil1_convolve_zero_extend:
          wt += kval;
          break;
        case vil1_convolve_constant_extend:
          ac += kval * AC(value2d(input, x, y1-1));
          wt += kval;
          break;
        case vil1_convolve_periodic_extend:
          ac += kval * AC(value2d(input, x, yy-(y1-y0)));
          wt += kval;
          break;
        case vil1_convolve_reflect_extend:
          ac += kval * AC(value2d(input, x, 2*(y1-1)-yy));
          wt += kval;
          break;
        case vil1_convolve_trim:
          break;
        default:
          vcl_abort();
          break;
        }

        else {
          ac += kval * AC(value2d(input, x, yy));
          wt += kval;
        }
      }

      // compute and store final value.
      if (zero)
        value2d(output, x, y) = AC(0);
      else if (wt)
        value2d(output, x, y) = O(ac * total_weight / wt);
    }
  }
}

#endif // vil1_convolve_1d_y_txx_
