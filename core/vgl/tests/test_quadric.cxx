// Some tests for vgl_quadric_3d
// J.L. Mundy, June 2017.
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <vgl/vgl_quadric_3d.h>
#include <vcl_compiler.h>
#include <vgl_tolerance.h>
// multiply square matrices
static std::vector<std::vector<double> > mul(std::vector<std::vector<double> > const& a,
                                             std::vector<std::vector<double> > const& b){
  size_t N = a.size();
  std::vector<std::vector<double> > out(N,std::vector<double>(N,0.0));
  for (unsigned i = 0; i < N; ++i)
    for (unsigned j = 0; j < N; ++j)
    {
      double accum = a[i][0] * b[0][j];
      for (unsigned k = 1; k < N; ++k)
        accum += a[i][k] * b[k][j];
      out[i][j] = accum;
    }
  return out;
}
static std::vector<std::vector<double> > transform_quadric(std::vector<std::vector<double> > const& T,
                                                           std::vector<std::vector<double> > const& Q){
  std::vector<std::vector<double> > Tt(4,std::vector<double>(4,0.0));
  for(size_t r = 0; r<4; ++r)
    for(size_t c = 0; c<4; ++c)
      Tt[r][c]=T[c][r];
  std::vector<std::vector<double> > temp(4,std::vector<double>(4,0.0));
  temp = mul(Q,T);
  temp = mul(Tt,temp);
  return temp;
}
static void test_quadric()
{
 /*
  coincident_planes              x^2=0                                  
  imaginary_ellipsoid           (x^2)/(a^2)+(y^2)/(b^2)+(z^2)/(c^2)=-1  
  real_ellipsoid                (x^2)/(a^2)+(y^2)/(b^2)+(z^2)/(c^2)=1   
  imaginary_elliptic_cone       (x^2)/(a^2)+(y^2)/(b^2)+(z^2)/(c^2)=0   
  real_elliptic_cone            (x^2)/(a^2)+(y^2)/(b^2)-(z^2)/(c^2)=0   
  imaginary_elliptic_cylinder   (x^2)/(a^2)+(y^2)/(b^2)=-1              
  real_elliptic_cylinder        (x^2)/(a^2)+(y^2)/(b^2)=1               
  elliptic_paraboloid           z=(x^2)/(a^2)+(y^2)/(b^2)               
  hyperbolic_cylinder           (x^2)/(a^2)-(y^2)/(b^2)=-1              
  hyperbolic_paraboloid         z=(y^2)/(b^2)-(x^2)/(a^2)               
  hyperboloid_of_one_sheet      (x^2)/(a^2)+(y^2)/(b^2)-(z^2)/(c^2)=1   
  hyperboloid_of_two_sheets     (x^2)/(a^2)+(y^2)/(b^2)-(z^2)/(c^2)=-1  
  imaginary_intersecting_planes (x^2)/(a^2)+(y^2)/(b^2)=0               
  real_intersecting_planes      (x^2)/(a^2)-(y^2)/(b^2)=0               
  parabolic_cylinder             x^2+2rz=0                              
  imaginary_parallel_planes      x^2=-a^2                               
  real_parallel planes           x^2=a^2                                
*/
  vgl_quadric_3d<double> coincident_planes(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  vgl_quadric_3d<double> imaginary_ellipsoid(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0); 
  vgl_quadric_3d<double> real_ellipsoid(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
  vgl_quadric_3d<double> imaginary_elliptic_cone(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  vgl_quadric_3d<double> real_elliptic_cone(1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  vgl_quadric_3d<double> imaginary_elliptic_cylinder(1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  vgl_quadric_3d<double> real_elliptic_cylinder(1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
  vgl_quadric_3d<double> elliptic_paraboloid(1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0);
  vgl_quadric_3d<double> hyperbolic_cylinder(1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  vgl_quadric_3d<double> hyperbolic_paraboloid(-1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0);
  vgl_quadric_3d<double> hyperboloid_of_one_sheet(1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0); 
  vgl_quadric_3d<double> hyperboloid_of_two_sheets(1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  vgl_quadric_3d<double> imaginary_intersecting_planes(1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  vgl_quadric_3d<double> real_intersecting_planes(1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);               
  vgl_quadric_3d<double> parabolic_cylinder(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  vgl_quadric_3d<double> imaginary_parallel_planes(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  vgl_quadric_3d<double> real_parallel_planes(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);

  bool good = coincident_planes.type()==vgl_quadric_3d<double>::coincident_planes;
  good = good && imaginary_ellipsoid.type()==vgl_quadric_3d<double>::imaginary_ellipsoid;
  good = good && real_ellipsoid.type()==vgl_quadric_3d<double>::real_ellipsoid;
  good = good && imaginary_elliptic_cone.type()==vgl_quadric_3d<double>::imaginary_elliptic_cone;
  good = good && real_elliptic_cone.type()==vgl_quadric_3d<double>::real_elliptic_cone;
  good = good && imaginary_elliptic_cylinder.type()==vgl_quadric_3d<double>::imaginary_elliptic_cylinder;
  good = good && real_elliptic_cylinder.type()==vgl_quadric_3d<double>::real_elliptic_cylinder;
  good = good && elliptic_paraboloid.type()==vgl_quadric_3d<double>::elliptic_paraboloid;
  good = good && hyperbolic_cylinder.type()==vgl_quadric_3d<double>::hyperbolic_cylinder;
  good = good && elliptic_paraboloid.type()==vgl_quadric_3d<double>::elliptic_paraboloid;
  good = good && hyperbolic_paraboloid.type()==vgl_quadric_3d<double>::hyperbolic_paraboloid;
  good = good && hyperboloid_of_one_sheet.type()==vgl_quadric_3d<double>::hyperboloid_of_one_sheet;
  good = good && hyperboloid_of_two_sheets.type()==vgl_quadric_3d<double>::hyperboloid_of_two_sheets;
  good = good && imaginary_intersecting_planes.type()==vgl_quadric_3d<double>::imaginary_intersecting_planes;
  good = good && real_intersecting_planes.type()==vgl_quadric_3d<double>::real_intersecting_planes;
  good = good && parabolic_cylinder.type()==vgl_quadric_3d<double>::parabolic_cylinder;
  good = good && imaginary_parallel_planes.type()==vgl_quadric_3d<double>::imaginary_parallel_planes;
  good = good && real_parallel_planes.type()==vgl_quadric_3d<double>::real_parallel_planes;
  TEST("Quadric classification", good, true);

  std::vector<std::vector<double> >  Q = elliptic_paraboloid.coef_matrix(), Qtrans;
  std::vector<std::vector< double> > T(4, std::vector<double>(4, 0.0));
  double p = 0.785, q = 0.866;
  double cp = cos(p), sp = sin(p);
  double cq = cos(q), sq = sin(q);
  double tx = 1.0, ty = 2.0, tz = 3.0;
  T[0][0] = cp; T[0][1] = -sp; T[0][3]=tx;
  T[1][0] = cq*sp; T[1][1] = cp*cq; T[1][2]=-sq; T[1][3]=ty;
  T[2][0] = sp*sq; T[2][1] = cp*sq; T[2][2]= cq; T[2][3]=tz;
  T[3][3] = 1.0;
  Qtrans = transform_quadric(T,Q);
  vgl_quadric_3d<double> test(Qtrans);
  good = test.type() == vgl_quadric_3d<double>::elliptic_paraboloid;
  Q = real_elliptic_cone.coef_matrix();
  Qtrans = transform_quadric(T, Q);
  vgl_quadric_3d<double> test2(Qtrans);
  good = good && test2.type() == vgl_quadric_3d<double>::real_elliptic_cone;
  TEST("Transformed quadric classification", good, true);
  std::string name = test2.type_by_number(test2.type());
  good = (name == "real_elliptic_cone"); 
  TEST("type_by_number", good, true);
  vgl_quadric_3d<double>::vgl_quadric_type typ = test2.type_by_name("real_elliptic_cone");
  good = typ == test2.type();
  TEST("type_by_name", good, true);
  good = real_elliptic_cone.contains(vgl_homg_point_3d<double>(1.0, 2.0, sqrt(5)), vgl_tolerance<double>::position);
  TEST("contains ", good, true);
  // stream ops
  std::stringstream ss;
  ss << 3.0 << ' ' << 3.0 << ' ' << -3.0 << ' '  << 0.0 << ' ' << 0.0 << ' ' << 0.0 << ' '
     << 0.0 << ' ' << 0.0 << ' ' << 0.0 << ' ' << 0.0;
  vgl_quadric_3d<double> qstr;
  ss >> qstr;
  std::cout << qstr << std::endl;
  good = qstr.type() == test2.type_by_name("real_elliptic_cone");
  TEST("stream ops", good , true);
  good = good && qstr == real_elliptic_cone;
  TEST("equal", good , true);
}

TESTMAIN(test_quadric);
