// This is core/vil/algo/tests/test_algo_cartesian_differential_invariants.cxx
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h> // for fabs(float)
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/algo/vil_cartesian_differential_invariants.h>
#include <testlib/testlib_test.h>

static void test_algo_cartesian_differential_invariants()
{
  vcl_cout << "*****************************************************\n"
           << " Testing test_algo_cartesian_differential_invariants\n"
           << "*****************************************************\n";

  const unsigned n = 11;

  vil_image_view<float> src(n,n);
  vil_image_view<float> dest, dest2;

  src.fill(0);
  src(n/2,n/2) = 255;

  vil_cartesian_differential_invariants_3(src, dest, 1.0);
  vcl_cout << "Source\n";
  vil_print_all(vcl_cout,  src);
  vcl_cout << "Destination\n";
  vil_print_all(vcl_cout,  dest);
  TEST("dest is correct size", dest.ni() == n && dest.nj() == n && dest.nplanes() == 8, true);


  // I have visually compared impulse response function images from
  // Kevin's original code, and my new code. They look the same.
  // Kevin's code used point sampling to build the filters, whereas
  // my code uses pixel integration sampling, so the results are not
  // directly comparable.
  // See cpp_above_matlab_comparing_cdi_code.png for impulse responses
  // from VXL and matlab versions (VXL version on top).
  // Not the lack of radial symmetry (especially in 4th, 5th, and 7th planes)
  // are due to rounding error. They should be zero, and are actually small values that
  // have been scaled up while saving to file.

  // The following is a regression test. The golden values are
  // merely verified as above.

  const float golden_data[] = {
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.00261491f, 0.193499f, 2.37197f, 5.36639f, 2.37197f, 0.193499f, 0.00261491f, 0.f, 0.f,
    0.f, 0.f, 0.193499f, 12.1514f, 120.676f, 242.613f, 120.676f, 12.1514f, 0.193499f, 0.f, 0.f,
    0.f, 0.f, 2.37197f, 120.676f, 763.611f, 958.092f, 763.611f, 120.676f, 2.37197f, 0.f, 0.f,
    0.f, 0.f, 5.36639f, 242.613f, 958.092f, 0.f, 958.092f, 242.613f, 5.36639f, 0.f, 0.f,
    0.f, 0.f, 2.37197f, 120.676f, 763.611f, 958.092f, 763.611f, 120.676f, 2.37197f, 0.f, 0.f,
    0.f, 0.f, 0.193499f, 12.1514f, 120.676f, 242.613f, 120.676f, 12.1514f, 0.193499f, 0.f, 0.f,
    0.f, 0.f, 0.00261491f, 0.193499f, 2.37197f, 5.36639f, 2.37197f, 0.193499f, 0.00261491f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.000585776f, 0.2993f, 9.67222f, 27.7197f, 9.67222f, 0.2993f, 0.000585776f, 0.f, 0.f,
    0.f, 0.f, 0.2993f, 115.43f, 2395.66f, 4625.15f, 2395.66f, 115.43f, 0.2993f, 0.f, 0.f,
    0.f, 0.f, 9.67222f, 2395.66f, 18433.9f, -2214.75f, 18433.9f, 2395.66f, 9.67222f, 0.f, 0.f,
    0.f, 0.f, 27.7197f, 4625.15f, -2214.75f, 0.f, -2214.75f, 4625.15f, 27.7197f, 0.f, 0.f,
    0.f, 0.f, 9.67222f, 2395.66f, 18433.9f, -2214.75f, 18433.9f, 2395.66f, 9.67222f, 0.f, 0.f,
    0.f, 0.f, 0.2993f, 115.43f, 2395.66f, 4625.15f, 2395.66f, 115.43f, 0.2993f, 0.f, 0.f,
    0.f, 0.f, 0.000585776f, 0.2993f, 9.67222f, 27.7197f, 9.67222f, 0.2993f, 0.000585776f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, -0.000164114f, -0.0835501f, -2.02331f, -3.73664f, -2.02331f, -0.0835501f, -0.000164114f, 0.f, 0.f,
    0.f, 0.f, -0.0835501f, -42.1126f, -987.525f, -1712.7f, -987.525f, -42.1126f, -0.0835501f, 0.f, 0.f,
    0.f, 0.f, -2.02331f, -987.525f, -20662.6f, -26980.6f, -20662.6f, -987.525f, -2.02331f, 0.f, 0.f,
    0.f, 0.f, -3.73664f, -1712.7f, -26980.6f, 0.f, -26980.6f, -1712.7f, -3.73664f, 0.f, 0.f,
    0.f, 0.f, -2.02331f, -987.525f, -20662.6f, -26980.6f, -20662.6f, -987.525f, -2.02331f, 0.f, 0.f,
    0.f, 0.f, -0.0835501f, -42.1126f, -987.525f, -1712.7f, -987.525f, -42.1126f, -0.0835501f, 0.f, 0.f,
    0.f, 0.f, -0.000164114f, -0.0835501f, -2.02331f, -3.73664f, -2.02331f, -0.0835501f, -0.000164114f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 2.05217e-011f, 0.0262445f, 1.32065f, 0.f, -1.32065f, -0.0262445f, -2.05217e-011f, 0.f, 0.f,
    0.f, 0.f, -0.0262445f, 6.72063e-006f, 339.152f, 0.f, -339.152f, -6.72063e-006f, 0.0262445f, 0.f, 0.f,
    0.f, 0.f, -1.32065f, -339.152f, -0.00219401f, 0.f, 0.00219401f, 339.152f, 1.32065f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 1.32065f, 339.152f, 0.00219401f, 0.f, -0.00219401f, -339.152f, -1.32065f, 0.f, 0.f,
    0.f, 0.f, 0.0262445f, -6.72063e-006f, -339.152f, 0.f, 339.152f, 6.72063e-006f, -0.0262445f, 0.f, 0.f,
    0.f, 0.f, -2.05217e-011f, -0.0262445f, -1.32065f, 0.f, 1.32065f, 0.0262445f, 2.05217e-011f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.0123648f, 2.47208f, 0.f, -2.47208f, -0.0123648f, 0.f, 0.f, 0.f,
    0.f, 0.f, -0.0123648f, -1.2593e-005f, 6425.73f, 0.f, -6425.73f, 1.2593e-005f, 0.0123648f, 0.f, 0.f,
    0.f, 0.f, -2.47208f, -6425.73f, 0.0353556f, 0.f, -0.0353556f, 6425.73f, 2.47208f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 2.47208f, 6425.73f, -0.0353556f, 0.f, 0.0353556f, -6425.73f, -2.47208f, 0.f, 0.f,
    0.f, 0.f, 0.0123648f, 1.2593e-005f, -6425.73f, 0.f, 6425.73f, -1.2593e-005f, -0.0123648f, 0.f, 0.f,
    0.f, 0.f, 0.f, -0.0123648f, -2.47208f, 0.f, 2.47208f, 0.0123648f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, -2.39812e-005f, -0.0894585f, -8.45908f, -34.3258f, -8.45908f, -0.0894585f, -2.39812e-005f, 0.f, 0.f,
    0.f, 0.f, -0.0894584f, -250.537f, -17086.f, -70159.f, -17086.f, -250.537f, -0.0894584f, 0.f, 0.f,
    0.f, 0.f, -8.45908f, -17086.f, -366049.f, -1.09413e+006f, -366049.f, -17086.f, -8.45908f, 0.f, 0.f,
    0.f, 0.f, -34.3257f, -70159.f, -1.09413e+006f, 0.f, -1.09413e+006f, -70159.f, -34.3257f, 0.f, 0.f,
    0.f, 0.f, -8.45908f, -17086.f, -366049.f, -1.09413e+006f, -366049.f, -17086.f, -8.45908f, 0.f, 0.f,
    0.f, 0.f, -0.0894584f, -250.537f, -17086.f, -70159.f, -17086.f, -250.537f, -0.0894584f, 0.f, 0.f,
    0.f, 0.f, -2.39812e-005f, -0.0894585f, -8.45908f, -34.3258f, -8.45908f, -0.0894585f, -2.39812e-005f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 3.38813e-021f, 0.0500808f, 7.17904f, 0.f, -7.17904f, -0.0500808f, -3.38813e-021f, 0.f, 0.f,
    0.f, 0.f, -0.0500808f, 4.8993e-005f, 6000.63f, 0.f, -6000.63f, -4.8993e-005f, 0.0500808f, 0.f, 0.f,
    0.f, 0.f, -7.17904f, -6000.63f, -0.0113272f, 0.f, 0.0113272f, 6000.63f, 7.17904f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 7.17904f, 6000.63f, 0.0113272f, 0.f, -0.0113272f, -6000.63f, -7.17904f, 0.f, 0.f,
    0.f, 0.f, 0.0500808f, -4.8993e-005f, -6000.63f, 0.f, 6000.63f, 4.8993e-005f, -0.0500808f, 0.f, 0.f,
    0.f, 0.f, -3.38813e-021f, -0.0500808f, -7.17904f, 0.f, 7.17904f, 0.0500808f, 3.38813e-021f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,

    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.000187632f, 0.894689f, 60.6675f, 52.6394f, 86.8583f, 2.0444f, 0.000731781f, 0.f, 0.f,
    0.f, 0.f, 0.60726f, 2125.88f, 84466.9f, -10280.9f, 127906.f, 4977.58f, 2.33183f, 0.f, 0.f,
    0.f, 0.f, 34.4766f, 62747.2f, -736888.f, -1.24231e+006f, -908045.f, 149626.f, 113.049f, 0.f, 0.f,
    0.f, 0.f, 52.6394f, -10280.9f, -1.24231e+006f, 0.f, -1.24231e+006f, -10280.9f, 52.6394f, 0.f, 0.f,
    0.f, 0.f, 34.4766f, 62747.2f, -736888.f, -1.24231e+006f, -908045.f, 149626.f, 113.049f, 0.f, 0.f,
    0.f, 0.f, 0.60726f, 2125.88f, 84466.9f, -10280.9f, 127906.f, 4977.58f, 2.33183f, 0.f, 0.f,
    0.f, 0.f, 0.000187632f, 0.894689f, 60.6675f, 52.6394f, 86.8583f, 2.0444f, 0.000731781f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f
  };

  assert(dest.is_contiguous());
  assert(dest.istep() == 1);
  const float *p_dest = dest.begin(), *p_golden=golden_data;
  unsigned i;
  for (i=0; i<dest.size(); ++i)
  {
    if ( vcl_fabs(*p_dest - *p_golden) > vcl_fabs(*p_golden) * 1e-6)
    {
      vcl_cout << "Found excess value at pixel " << i << vcl_endl;
      break;
    }
  }
  TEST("impulse response matches golden data", i, dest.size());


  // Test invariant properties

  src.fill(0.f);
  src(n/2,n/2) =   256.f;
  src(n/2-1,n/2) = 128.f;
  src(n/2+1,n/2) = 512.f;
  vil_cartesian_differential_invariants_3(src, dest, 1.0);
  src.fill(0.f);
  src(n/2,n/2) =   256.f;
  src(n/2,n/2-1) = 128.f;
  src(n/2,n/2+1) = 512.f;
  vil_cartesian_differential_invariants_3(src, dest2, 1.0);
  for (unsigned i=0; i<8; ++i)
    TEST_NEAR("cartesian invariance", dest2(n/2,n/2,i), dest2(n/2,n/2,i),
              vcl_fabs(dest2(n/2,n/2,i)*1.e-4f));
}

TESTMAIN(test_algo_cartesian_differential_invariants);
