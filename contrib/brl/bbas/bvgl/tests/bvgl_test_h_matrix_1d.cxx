// This is brl/bbas/bvgl/tests/bvgl_test_h_matrix_1d.cxx
#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_1d.h>
#include <bvgl/bvgl_h_matrix_1d.h>
#include <testlib/testlib_test.h>

static void bvgl_test_h_matrix_1d()
{
  vcl_cout << "Testing identity transform on point\n";
  vnl_matrix_fixed<double,2,2> M; M.set_identity();
  bvgl_h_matrix_1d<double> Id(M);
  vgl_homg_point_1d<double> p(3,1);
  vgl_homg_point_1d<double> pp = Id(p);
  vcl_cout << "Id\n" << Id << '\n'
           << 'p' << p << " , Id(p) = pp " << pp << '\n';
  TEST_NEAR("pp.x()", pp.x(), p.x(), 1e-4);
  TEST_NEAR("pp.w()", pp.w(), p.w(), 1e-4);

  vcl_cout << "Testing perspective transform on point\n";
  M.put(0,0,1);    M.put(0,1,1);
  M.put(1,0,0.25); M.put(1,1,1.75);
  bvgl_h_matrix_1d<double> Tproj(M);
  pp = Tproj(p);
  vgl_homg_point_1d<double> ppp = Tproj.preimage(pp);
  vcl_cout << "Tproj\n" << Tproj << '\n'
           << 'p' << p << " , Tproj(p) = pp " << pp << '\n'
           << " , Tproj.preimage(pp) = ppp " << ppp << '\n';
  TEST_NEAR("ppp.x()", ppp.x(), p.x(), 1e-4);
  TEST_NEAR("ppp.w()", ppp.w(), p.w(), 1e-4);
}

TESTMAIN(bvgl_test_h_matrix_1d);
