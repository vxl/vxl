#include <iostream>
#include <testlib/testlib_test.h>
#include <imesh/algo/imesh_imls_surface.h>
#include "test_share.h"
#include <imesh/algo/imesh_transform.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <imesh/imesh_fileio.h>
#include <imesh/imesh_operations.h>
#include <vnl/vnl_matlab_filewrite.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>

double approx_deriv(const imesh_imls_surface& f,
                    const vgl_point_3d<double>& p,
                    vgl_vector_3d<double>& dp, double eps = 1e-6)
{
  double fp = f(p);
  double fpx = f(p+vgl_vector_3d<double>(eps,0,0));
  double fpy = f(p+vgl_vector_3d<double>(0,eps,0));
  double fpz = f(p+vgl_vector_3d<double>(0,0,eps));
  dp.set((fpx-fp)/eps, (fpy-fp)/eps, (fpz-fp)/eps);
  return fp;
}


static void test_imls_surface()
{
  double I1,Ix;
  double k1 = 1.5, k2 = 4.0;
  imesh_imls_surface::line_integrals(k1,k2,I1,Ix);
  double I1_,Ix_,dI1,dIx,dIx2;
  imesh_imls_surface::line_integrals(k1,k2,I1_,Ix_,dI1,dIx,dIx2);

  // numerical integration by trapezoid rule
  double quad_I1 = 0.0, quad_Ix = 0.0;
  double quad_dI1 = 0.0, quad_dIx = 0.0, quad_dIx2 = 0.0;
  double step = 0.00001;
  for (double i=step/2.0; i<1.0; i+=step) {
    double x_pk1 = i+k1;
    double x_pk1_2 = x_pk1*x_pk1;
    double x_pk1_2_pk2 = x_pk1_2 + k2;
    double x_pk1_2_pk2_2 = x_pk1_2_pk2 * x_pk1_2_pk2;
    double x_pk1_2_pk2_3 = x_pk1_2_pk2_2 * x_pk1_2_pk2;
    quad_I1 += 1.0 / x_pk1_2_pk2_2;
    quad_Ix += i / x_pk1_2_pk2_2;
    quad_dI1 += 1.0 / x_pk1_2_pk2_3;
    quad_dIx += i / x_pk1_2_pk2_3;
    quad_dIx2 += i*i / x_pk1_2_pk2_3;
  }
  quad_I1 *= step;
  quad_Ix *= step;
  quad_dI1 *= step;
  quad_dIx *= step;
  quad_dIx2 *= step;

  TEST_NEAR("Line Integral 1",I1, quad_I1, 1e-12);
  TEST_NEAR("Line Integral X",Ix, quad_Ix, 1e-12);
  TEST_NEAR("Line Integral d1",dI1, quad_dI1, 1e-12);
  TEST_NEAR("Line Integral dX",dIx, quad_dIx, 1e-12);
  TEST_NEAR("Line Integral dX^2",dIx2, quad_dIx2, 1e-12);
  TEST("Derivative version same", I1 == I1_ && Ix == Ix_, true);

  vgl_point_3d<double> p0(-1,3,.5), p1(3,4,2), x(1,2,0);
  double v0=1, v1=3, eps2=.01*.01;
  double li = imesh_imls_surface::line_integral(x,p0,p1,v0,v1,eps2);

  vgl_vector_3d<double> diff(p1-p0);
  double quad_li = 0.0;
  for (double i=step/2.0; i<1.0; i+=step) {
    vgl_point_3d<double> p = p0 + i*diff;
    double val = v0 + i*(v1-v0);

    double s = ((p-x).sqr_length() + eps2);
    quad_li += val / (s*s);
  }
  quad_li *= step * diff.length();

  TEST_NEAR("Line Integral 3D",li, quad_li, 1e-10);

  {
    vgl_vector_3d<double> deriv_x
        = imesh_imls_surface::line_integral_deriv(x,p0,p1,v0,v1,eps2);
    double eps = 1e-7;
    double dx = imesh_imls_surface::line_integral(x+vgl_vector_3d<double>(eps,0,0),
                                                  p0,p1,v0,v1,eps2) - li;
    double dy = imesh_imls_surface::line_integral(x+vgl_vector_3d<double>(0,eps,0),
                                                  p0,p1,v0,v1,eps2) - li;
    double dz = imesh_imls_surface::line_integral(x+vgl_vector_3d<double>(0,0,eps),
                                                  p0,p1,v0,v1,eps2) - li;
    vgl_vector_3d<double> step_deriv_x(dx/eps,dy/eps,dz/eps);
    TEST_NEAR("Line Integral of deriv 3D",(step_deriv_x - deriv_x).length(), 0.0, eps);
  }

  vgl_point_3d<double> p2(2,0,-4);
  double v2 = 2;

  // project x onto the triangle plane
  vgl_vector_3d<double> n = normalized(cross_product(p1-p0,p2-p0));
  x -= .90*dot_product(n,x-p0)*n;

  double alpha = 2.0/3.0;
  double u=alpha;
  double last_li = 0.0;
  double sum = 0.0;
  vgl_vector_3d<double> sum_dx(0,0,0), last_dx(0,0,0);
  for (; u>0.01; u*=alpha) {
    vgl_point_3d<double> p0i((1-u)*p0.x() + u*p2.x(),(1-u)*p0.y() + u*p2.y(),(1-u)*p0.z() + u*p2.z());
    vgl_point_3d<double> p1i((1-u)*p1.x() + u*p2.x(),(1-u)*p1.y() + u*p2.y(),(1-u)*p1.z() + u*p2.z());
    double v0i = (1-u)*v0 + u*v2;
    double v1i = (1-u)*v1 + u*v2;
    li = imesh_imls_surface::line_integral(x,p0i,p1i,v0i,v1i,eps2);
    vgl_vector_3d<double> dx = imesh_imls_surface::line_integral_deriv(x,p0i,p1i,v0i,v1i,eps2);
    double i_size = u/alpha - u;
    sum += i_size*(li + last_li)/2.0;
    sum_dx += i_size*(dx + last_dx)/2.0;
    last_li = li;
    last_dx = dx;
  }

  li = imesh_imls_surface::line_integral(x,p0,p1,v0,v1,eps2);
  vgl_vector_3d<double> dx = imesh_imls_surface::line_integral_deriv(x,p0,p1,v0,v1,eps2);
  sum += u/alpha*(li + last_li)/2.0;
  sum_dx += u/alpha*(dx + last_dx)/2.0;

  sum *= cross_product(p2-p0,p1-p0).length() / (p1-p0).length();
  sum_dx *= cross_product(p2-p0,p1-p0).length() / (p1-p0).length();

  std::cout << "integral 1 = "<<sum << std::endl
           << "integral 1 dx = "<<sum_dx << std::endl;

  vgl_vector_2d<double> ii = imesh_imls_surface::split_triangle_quadrature(x,p0,p1,p2,v0,v1,v2,eps2);
  std::cout << "integral 2 = "<<ii.x()<<std::endl;

  imesh_imls_surface::integral_data id =
      imesh_imls_surface::split_triangle_quadrature_with_deriv(x,p0,p1,p2,v0,v1,v2,eps2);
  std::cout << "integral 2 dx = "<<id.dI_phi<<std::endl;

  TEST_NEAR("Same with and without deriv (phi)", ii.x(), id.I_phi, 2e-10);
  TEST_NEAR("Same with and without deriv ", ii.y(), id.I, 1e-10);

  n = cross_product(p1-p0,p2-p0)/2.0;
  {
    typedef vgl_vector_2d<double> T;
    typedef vgl_vector_2d<double> (*F) (const vgl_point_3d<double>&, const vgl_point_3d<double>&,
                                        const vgl_point_3d<double>&, const vgl_point_3d<double>&,
                                        double, double, double, double);
    T data = imesh_imls_surface::triangle_quadrature<T,F>(imesh_imls_surface::split_triangle_quadrature,
                                                          x,p0,p1,p2,n,v0,v1,v2,eps2);
    std::cout << "integral 3 = "<<data<<std::endl;
  }

  {
    typedef imesh_imls_surface::integral_data T;
    typedef T (*F) (const vgl_point_3d<double>&, const vgl_point_3d<double>&,
                    const vgl_point_3d<double>&, const vgl_point_3d<double>&,
                    double, double, double, double);
    T data = imesh_imls_surface::triangle_quadrature<T,F>
                   (imesh_imls_surface::split_triangle_quadrature_with_deriv,
                    x,p0,p1,p2,n,v0,v1,v2,eps2);
    std::cout << "integral 3a = "<< data.I << ' ' << data.I_phi
             << ' ' <<data.dI << ' ' << data.dI_phi << std::endl;
  }

  {
    unsigned int num_samps = 1000;
    double sum = 0.0;
    vgl_vector_3d<double> sum_dx(0,0,0);
    for (unsigned i=0; i<num_samps; ++i) {
      double u = (i+0.5)/num_samps;
      for (unsigned j=0; j<num_samps-i; ++j) {
        double v = (j+0.5)/num_samps;
        double t = 1.0-u-v;
        vgl_point_3d<double> p(t*p0.x() + u*p1.x() + v*p2.x(),
                               t*p0.y() + u*p1.y() + v*p2.y(),
                               t*p0.z() + u*p1.z() + v*p2.z());
        double val = t*v0 + u*v1 + v*v2;
        double w = 1.0/((p-x).sqr_length()+eps2);
        if (i+j+1 == num_samps) {
          sum += val*w*w/2.0;
          sum_dx += 4*(p-x)*val*w*w*w/2.0;
        }
        else {
          sum += val*w*w;
          sum_dx += 4*(p-x)*val*w*w*w;
        }
      }
    }
    sum /= num_samps * num_samps;
    sum *= cross_product(p1-p0, p2-p0).length();
    sum_dx /= num_samps * num_samps;
    sum_dx *= cross_product(p1-p0, p2-p0).length();
    std::cout << "true integral = " << sum << std::endl
             << "true integral of dx = " << sum_dx << std::endl
             << "area = " << cross_product(p1-p0, p2-p0).length()/2.0 << std::endl;
  }

  {
    imesh_mesh cube;
    make_cube(cube);
    imesh_transform_inplace(cube, vgl_rotation_3d<double>(0,0,vnl_math::pi_over_4));
    imesh_quad_subdivide(cube);
    imesh_quad_subdivide(cube);
    std::set<unsigned int> no_normals;
    for (unsigned int i=32; i<64; i+=2) {
      no_normals.insert(i);
    }
    imesh_imls_surface f(cube,.1,.1,true,no_normals);

    //imesh_write_obj("cube.obj",cube);

    vgl_point_3d<double> p(3.0,.5,.1);
    vgl_vector_3d<double> dp, dp2;
    double fval = approx_deriv(f,p,dp,1e-8);
    double fval2 = f.deriv(p,dp2);
    TEST_NEAR("Evaluation same with deriv",fval, fval2, 1e-10);
    // FIXME: can this derivative be made more accurate?
    TEST_NEAR("Function derivative",(dp-dp2).length(),0.0,1e-3);

#if 0
    vnl_matrix<double> M(200,200);
    for (int i=0; i<200; ++i) {
      std::cout << "row "<< i<<std::endl;
      for (int j=0; j<200; ++j) {
        M(i,j) = f(double(i-100)/50.0, double(j-100)/50.0, 0.25);
      }
    }
    vnl_matlab_filewrite mfw("slice.mat");
    mfw.write(M,"M");
#endif
  }
}

TESTMAIN(test_imls_surface);
