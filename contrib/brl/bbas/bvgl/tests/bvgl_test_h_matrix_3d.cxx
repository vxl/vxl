// This is brl/bbas/bvgl/tests/bvgl_test_h_matrix_3d.cxx
#include <vcl_iostream.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_3d.h>
#include <bvgl/bvgl_h_matrix_3d.h>
#include <testlib/testlib_test.h>

static void bvgl_test_h_matrix_3d()
{
  vcl_cout << "Testing identity transform on point\n";
  vnl_matrix_fixed<double, 4, 4> M; M.set_identity();
  bvgl_h_matrix_3d<double> Id(M);
  vgl_homg_point_3d<double> p(3,2,1,1);
  vgl_homg_point_3d<double> pp = Id(p);
  vcl_cout << "Id\n" << Id << '\n'
           << 'p' << p << " , Id(p) = pp " << pp << '\n';
  TEST_NEAR("pp.x()", pp.x(), p.x(), 1e-4);
  TEST_NEAR("pp.y()", pp.y(), p.y(), 1e-4);
  TEST_NEAR("pp.z()", pp.z(), p.z(), 1e-4);
  TEST_NEAR("pp.w()", pp.w(), p.w(), 1e-4);

  vcl_cout << "Testing perspective transform on point\n";
  M.put(0,0,1);    M.put(0,1,2);   M.put(0,2,3);   M.put(0,3,1);
  M.put(1,0,0.5);  M.put(1,1,-2);  M.put(1,2,-3);  M.put(1,3,1.5);
  M.put(2,0,0.75); M.put(2,1,-3);  M.put(2,2,-1);  M.put(2,3,1.5);
  M.put(3,0,0.25); M.put(3,1,3);   M.put(3,2,4);   M.put(3,3,1.75);
  bvgl_h_matrix_3d<double> Tproj(M);
  pp = Tproj(p);
  vgl_homg_point_3d<double> ppp = Tproj.preimage(pp);
  vcl_cout << "Tproj\n" << Tproj << '\n'
           << 'p' << p << " , Tproj(p) = pp " << pp << '\n'
           << " , Tproj.preimage(pp) = ppp " << ppp << '\n';
  TEST_NEAR("ppp.x()", ppp.x(), p.x(), 1e-4);
  TEST_NEAR("ppp.y()", ppp.y(), p.y(), 1e-4);
  TEST_NEAR("ppp.z()", ppp.z(), p.z(), 1e-4);
  TEST_NEAR("ppp.w()", ppp.w(), p.w(), 1e-4);
}

TESTMAIN(bvgl_test_h_matrix_3d);
