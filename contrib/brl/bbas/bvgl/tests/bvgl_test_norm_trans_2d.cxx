// This is brl/bbas/bvgl/tests/bvgl_test_norm_trans_2d.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <bvgl/bvgl_h_matrix_2d.h>
#include <bvgl/bvgl_norm_trans_2d.h>
bool near_eq(double a, double b){return vcl_fabs(a-b)<1e-4;}

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main()
{
  int success=0, failures=0;
  int Npts = 100;
  bvgl_norm_trans_2d<double> nt;
  vcl_cout << "testing normalizing transform for points\n";
  vcl_vector<vgl_homg_point_2d<double> > points;
  for (int i = 0; i<Npts; i++)
    {
      double x = i*1.01, y = i*1.2;
      vgl_homg_point_2d<double> p(x, y);
      points.push_back(p);
    }
  nt.compute_from_points(points);
  vcl_cout << "Normalizing Transform \n" << nt << "\n";
  double cx=0, cy=0;
  for (int i=0, j=0; i<Npts; i++,j++)
    {
      if (j>9) j = 0;
      vgl_point_2d<double> p(nt(points[i]));
      if (!j)
        {
        vcl_cout << "p[" << i << "] =  " << vgl_point_2d<double>(points[i])
                 << '\n' << "pnorm[" << i << "] =  " << p << '\n';
        }
      cx += p.x();
      cy += p.y();
    }
  vcl_cout << "Residual Center(" << cx/Npts << " " << cy/Npts << ")\n";
  Assert(near_eq(0.0, cx/Npts)&&near_eq(0.0, cy/Npts));

  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
