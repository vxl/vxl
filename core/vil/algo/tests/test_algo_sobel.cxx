// This is core/vil/algo/tests/test_algo_sobel.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Peter Vanroose
// \date   26 Aug. 2004
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/algo/vil_sobel_1x3.h>
#include <vil/algo/vil_sobel_3x3.h>

static void test_algo_sobel()
{
  vcl_cout << "*******************\n"
           << " Testing vil_sobel\n"
           << "*******************\n";

  const unsigned int WD=5, HT=6, X=2, Y=3;
  const int step=9;
  const double halfstep=0.5*step,
               qrtrstep=0.25*step,
               eigtstep=0.125*step;
  vil_image_view<vxl_byte> image0; image0.set_size(WD,HT);
  image0.fill(10); image0(X,Y  )=10+step;

  vil_image_view<double> gradi, gradj, gradij;
  vil_sobel_1x3(image0, gradi, gradj);
  TEST("vil_sobel_1x3: grad_i.ni()", gradi.ni(), WD);
  TEST("vil_sobel_1x3: grad_i.nj()", gradi.nj(), HT);
  TEST("vil_sobel_1x3: grad_i.nplanes()", gradi.nplanes(), 1);
  TEST("vil_sobel_1x3: grad_i",    gradi(X  ,Y  ), 0.0);
  TEST("vil_sobel_1x3: grad_i T",  gradi(X  ,Y-1), 0.0);
  TEST("vil_sobel_1x3: grad_i B",  gradi(X  ,Y+1), 0.0);
  TEST("vil_sobel_1x3: grad_i L",  gradi(X-1,Y  ), halfstep);
  TEST("vil_sobel_1x3: grad_i R",  gradi(X+1,Y  ),-halfstep);
  TEST("vil_sobel_1x3: grad_i TL", gradi(X-1,Y-1), 0.0);
  TEST("vil_sobel_1x3: grad_i TR", gradi(X+1,Y-1), 0.0);
  TEST("vil_sobel_1x3: grad_i BL", gradi(X-1,Y+1), 0.0);
  TEST("vil_sobel_1x3: grad_i BR", gradi(X+1,Y+1), 0.0);
  TEST("vil_sobel_1x3: grad_j.ni()", gradj.ni(), WD);
  TEST("vil_sobel_1x3: grad_j.nj()", gradj.nj(), HT);
  TEST("vil_sobel_1x3: grad_j.nplanes()", gradj.nplanes(), 1);
  TEST("vil_sobel_1x3: grad_j",    gradj(X  ,Y  ), 0.0);
  TEST("vil_sobel_1x3: grad_j T",  gradj(X  ,Y-1), halfstep);
  TEST("vil_sobel_1x3: grad_j B",  gradj(X  ,Y+1),-halfstep);
  TEST("vil_sobel_1x3: grad_j L",  gradj(X-1,Y  ), 0.0);
  TEST("vil_sobel_1x3: grad_j R",  gradj(X+1,Y  ), 0.0);
  TEST("vil_sobel_1x3: grad_j TL", gradj(X-1,Y-1), 0.0);
  TEST("vil_sobel_1x3: grad_j TR", gradj(X+1,Y-1), 0.0);
  TEST("vil_sobel_1x3: grad_j BL", gradj(X-1,Y+1), 0.0);
  TEST("vil_sobel_1x3: grad_j BR", gradj(X+1,Y+1), 0.0);
  vil_sobel_1x3(image0, gradij);
  TEST("vil_sobel_1x3: grad_ij.ni()", gradij.ni(), WD);
  TEST("vil_sobel_1x3: grad_ij.nj()", gradij.nj(), HT);
  TEST("vil_sobel_1x3: grad_ij.nplanes()", gradij.nplanes(), 2);
  TEST("vil_sobel_1x3: grad_ij i",   gradij(X  ,Y,0), 0.0);
  TEST("vil_sobel_1x3: grad_ij Ti",  gradij(X  ,Y-1,0), 0.0);
  TEST("vil_sobel_1x3: grad_ij Bi",  gradij(X  ,Y+1,0), 0.0);
  TEST("vil_sobel_1x3: grad_ij Li",  gradij(X-1,Y,0), halfstep);
  TEST("vil_sobel_1x3: grad_ij Ri",  gradij(X+1,Y,0),-halfstep);
  TEST("vil_sobel_1x3: grad_ij TLi", gradij(X-1,Y-1,0), 0.0);
  TEST("vil_sobel_1x3: grad_ij TRi", gradij(X+1,Y-1,0), 0.0);
  TEST("vil_sobel_1x3: grad_ij BLi", gradij(X-1,Y+1,0), 0.0);
  TEST("vil_sobel_1x3: grad_ij BRi", gradij(X+1,Y+1,0), 0.0);
  TEST("vil_sobel_1x3: grad_ij j",   gradij(X  ,Y,1), 0.0);
  TEST("vil_sobel_1x3: grad_ij Tj",  gradij(X  ,Y-1,1), halfstep);
  TEST("vil_sobel_1x3: grad_ij Bj",  gradij(X  ,Y+1,1),-halfstep);
  TEST("vil_sobel_1x3: grad_ij Lj",  gradij(X-1,Y,1), 0.0);
  TEST("vil_sobel_1x3: grad_ij Rj",  gradij(X+1,Y,1), 0.0);
  TEST("vil_sobel_1x3: grad_ij TLj", gradij(X-1,Y-1,1), 0.0);
  TEST("vil_sobel_1x3: grad_ij TRj", gradij(X+1,Y-1,1), 0.0);
  TEST("vil_sobel_1x3: grad_ij BLj", gradij(X-1,Y+1,1), 0.0);
  TEST("vil_sobel_1x3: grad_ij BRj", gradij(X+1,Y+1,1), 0.0);

  vil_sobel_3x3(image0, gradi, gradj);
  TEST("vil_sobel_3x3: grad_i.ni()", gradi.ni(), WD);
  TEST("vil_sobel_3x3: grad_i.nj()", gradi.nj(), HT);
  TEST("vil_sobel_3x3: grad_i.nplanes()", gradi.nplanes(), 1);
  TEST("vil_sobel_3x3: grad_i",    gradi(X  ,Y  ), 0.0);
  TEST("vil_sobel_3x3: grad_i T",  gradi(X  ,Y-1), 0.0);
  TEST("vil_sobel_3x3: grad_i B",  gradi(X  ,Y+1), 0.0);
  TEST("vil_sobel_3x3: grad_i L",  gradi(X-1,Y  ), qrtrstep);
  TEST("vil_sobel_3x3: grad_i R",  gradi(X+1,Y  ),-qrtrstep);
  TEST("vil_sobel_3x3: grad_i TL", gradi(X-1,Y-1), eigtstep);
  TEST("vil_sobel_3x3: grad_i TR", gradi(X+1,Y-1),-eigtstep);
  TEST("vil_sobel_3x3: grad_i BL", gradi(X-1,Y+1), eigtstep);
  TEST("vil_sobel_3x3: grad_i BR", gradi(X+1,Y+1),-eigtstep);
  TEST("vil_sobel_3x3: grad_j.ni()", gradj.ni(), WD);
  TEST("vil_sobel_3x3: grad_j.nj()", gradj.nj(), HT);
  TEST("vil_sobel_3x3: grad_j.nplanes()", gradj.nplanes(), 1);
  TEST("vil_sobel_3x3: grad_j",    gradj(X  ,Y  ), 0.0);
  TEST("vil_sobel_3x3: grad_j T",  gradj(X  ,Y-1), qrtrstep);
  TEST("vil_sobel_3x3: grad_j B",  gradj(X  ,Y+1),-qrtrstep);
  TEST("vil_sobel_3x3: grad_j L",  gradj(X-1,Y  ), 0.0);
  TEST("vil_sobel_3x3: grad_j R",  gradj(X+1,Y  ), 0.0);
  TEST("vil_sobel_3x3: grad_j TL", gradj(X-1,Y-1), eigtstep);
  TEST("vil_sobel_3x3: grad_j TR", gradj(X+1,Y-1), eigtstep);
  TEST("vil_sobel_3x3: grad_j BL", gradj(X-1,Y+1),-eigtstep);
  TEST("vil_sobel_3x3: grad_j BR", gradj(X+1,Y+1),-eigtstep);
  vil_sobel_3x3(image0, gradij);
  TEST("vil_sobel_3x3: grad_ij.ni()", gradij.ni(), WD);
  TEST("vil_sobel_3x3: grad_ij.nj()", gradij.nj(), HT);
  TEST("vil_sobel_3x3: grad_ij.nplanes()", gradij.nplanes(), 2);
  TEST("vil_sobel_3x3: grad_ij i",   gradij(X  ,Y,0), 0.0);
  TEST("vil_sobel_3x3: grad_ij Ti",  gradij(X  ,Y-1,0), 0.0);
  TEST("vil_sobel_3x3: grad_ij Bi",  gradij(X  ,Y+1,0), 0.0);
  TEST("vil_sobel_3x3: grad_ij Li",  gradij(X-1,Y,0), qrtrstep);
  TEST("vil_sobel_3x3: grad_ij Ri",  gradij(X+1,Y,0),-qrtrstep);
  TEST("vil_sobel_3x3: grad_ij TLi", gradij(X-1,Y-1,0), eigtstep);
  TEST("vil_sobel_3x3: grad_ij TRi", gradij(X+1,Y-1,0),-eigtstep);
  TEST("vil_sobel_3x3: grad_ij BLi", gradij(X-1,Y+1,0), eigtstep);
  TEST("vil_sobel_3x3: grad_ij BRi", gradij(X+1,Y+1,0),-eigtstep);
  TEST("vil_sobel_3x3: grad_ij j",   gradij(X  ,Y,1), 0.0);
  TEST("vil_sobel_3x3: grad_ij Tj",  gradij(X  ,Y-1,1), qrtrstep);
  TEST("vil_sobel_3x3: grad_ij Bj",  gradij(X  ,Y+1,1),-qrtrstep);
  TEST("vil_sobel_3x3: grad_ij Lj",  gradij(X-1,Y,1), 0.0);
  TEST("vil_sobel_3x3: grad_ij Rj",  gradij(X+1,Y,1), 0.0);
  TEST("vil_sobel_3x3: grad_ij TLj", gradij(X-1,Y-1,1), eigtstep);
  TEST("vil_sobel_3x3: grad_ij TRj", gradij(X+1,Y-1,1), eigtstep);
  TEST("vil_sobel_3x3: grad_ij BLj", gradij(X-1,Y+1,1),-eigtstep);
  TEST("vil_sobel_3x3: grad_ij BRj", gradij(X+1,Y+1,1),-eigtstep);
}

TESTMAIN(test_algo_sobel);
