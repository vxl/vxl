// This is brl/bbas/bvgl/tests/bvgl_test_h_matrix_2d.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <bvgl/bvgl_h_matrix_2d.h>

bool near_eq(double a, double b){return vcl_fabs(a-b)<1e-4;}

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;
  vcl_cout << "Testing identity transform on point\n";
  vnl_matrix_fixed<double, 3, 3> M;
  M.set_identity();
  bvgl_h_matrix_2d<double> Id(M);
  vgl_homg_point_2d<double> p(3,2,1), pp, ppp;
  pp = Id(p);
  vcl_cout << "Id \n" << Id << '\n'
           << 'p' << p << " , Id(p) = pp " << pp << '\n';
  Assert(near_eq(pp.x(),p.x())&&near_eq(pp.y(), p.y())&&near_eq(pp.w(),p.w()));

  vcl_cout << "Testing perspective transform on point\n";
  M.put(0,0,1);    M.put(0,1,2);   M.put(0,2,1);
  M.put(1,0,0.5);  M.put(1,1,-2);  M.put(1,2,1.5);
  M.put(2,0,0.25); M.put(2,1,3);   M.put(2,2,1.75);
  bvgl_h_matrix_2d<double> Tproj(M);
  pp = Tproj(p);
  ppp = Tproj.preimage(pp);
  vcl_cout << "Tproj \n" << Tproj << '\n'
           << 'p' << p << " , Tproj(p) = pp " << pp << '\n'
           << " , Tproj.preimage(pp) = ppp " << ppp << '\n';
  Assert(near_eq(ppp.x(),p.x())&&
         near_eq(ppp.y(),p.y())&&
         near_eq(ppp.w(),p.w()));
  vcl_cout << "Testing canonical basis for points\n"
           << "Test points on a unit square\n";
  vgl_homg_point_2d<double> p0(0.0,0.0,1.0);
  vgl_homg_point_2d<double> p1(1.0,0.0,1.0);
  vgl_homg_point_2d<double> p2(0.0,1.0,1.0);
  vgl_homg_point_2d<double> p3(1.0,1.0,1.0);
  vcl_vector<vgl_homg_point_2d<double> > basis_points;
  basis_points.push_back(p0);   basis_points.push_back(p1);
  basis_points.push_back(p2);   basis_points.push_back(p3);
  bvgl_h_matrix_2d<double> Basis;
  Basis.projective_basis(basis_points);
  vcl_cout <<"Transform to Canonical Frame \n" << Basis << '\n'
           <<"canonical p0 " << Basis(p0) << '\n'
           <<"canonical p1 " << Basis(p1) << '\n'
           <<"canonical p2 " << Basis(p2) << '\n'
           <<"canonical p3 " << Basis(p3) << '\n';
  Assert(near_eq(p3.x(),1.0)&&near_eq(p3.y(),1.0)&&near_eq(p3.w(),1.0));
  vcl_cout << "Test collinear points\n";
  vgl_homg_point_2d<double> pcl0(0.0,0.0,1.0);
  vgl_homg_point_2d<double> pcl1(1.0,1.0,1.0);
  vgl_homg_point_2d<double> pcl2(2.0,2.0,1.0);
  vgl_homg_point_2d<double> pcl3(3.0,3.0,1.0);
  vcl_vector<vgl_homg_point_2d<double> > collinear_points;
  collinear_points.push_back(pcl0);   collinear_points.push_back(pcl1);
  collinear_points.push_back(pcl2);   collinear_points.push_back(pcl3);
  bvgl_h_matrix_2d<double> collinear_basis;
  Assert(!collinear_basis.projective_basis(collinear_points));
  vcl_cout <<"Collinear Transform \n" << collinear_basis << '\n'
           << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
