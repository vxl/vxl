#ifndef fsm_dont_croak
croak
#endif

template <class I1, class I2, class AC, class O>
void vil_convolve_1d_y(vil_convolve_signal_1d<I1 const> const &kernel,
                       vil_convolve_signal_2d<I2 const> const &input,
                       AC *,
                       vil_convolve_signal_2d<O> const &output,
                       vil_convolve_boundary_option b,
                       vil_convolve_boundary_option e)
{
  // compute ranges of i, x, y here.
  int i0 = kernel.begin-kernel.origin;
  int i1 = kernel.end  -kernel.origin;

  int x0 = output.beginx-output.originx;
  int x1 = output.endx  -output.originx;

  int y0 = output.beginy-output.originy;
  int y1 = output.endy  -output.originy;

  // compute total weight of the kernel.
  // FIXME assumes non-negative kernel.
  AC total_weight = 0;
  for (int i=i0; i<i1; ++i)
    total_weight += AC(value1d(kernel, i));


  // this is not very efficient at the moment, buy my main
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
        case vil_convolve_no_extend:
          zero = true; /*FIXME*/
          break;
        case vil_convolve_zero_extend:
          wt += kval;
          break;
        case vil_convolve_constant_extend:
          ac += kval * AC(value2d(input, x, y0));
          wt += kval;
          break;
        case vil_convolve_periodic_extend:
          ac += kval * AC(value2d(input, x, yy+(y1-y0)));
          wt += kval;
          break;
        case vil_convolve_reflect_extend:
          ac += kval * AC(value2d(input, x, 2*y0-yy));
          wt += kval;
          break;
        case vil_convolve_trim:
          break;
        default:
          vcl_abort();
          break;
        }

        else if (yy >= y1) switch (e) {
        case vil_convolve_no_extend:
          zero = true; /*FIXME*/
          break;
        case vil_convolve_zero_extend:
          wt += kval;
          break;
        case vil_convolve_constant_extend:
          ac += kval * AC(value2d(input, x, y1-1));
          wt += kval;
          break;
        case vil_convolve_periodic_extend:
          ac += kval * AC(value2d(input, x, yy-(y1-y0)));
          wt += kval;
          break;
        case vil_convolve_reflect_extend:
          ac += kval * AC(value2d(input, x, 2*(y1-1)-yy));
          wt += kval;
          break;
        case vil_convolve_trim:
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
        value2d(output, x, y) = ac * total_weight / wt;
    }
  }

}
