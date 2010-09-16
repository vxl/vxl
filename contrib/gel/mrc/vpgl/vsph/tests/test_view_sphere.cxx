#include <testlib/testlib_test.h>

#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_spherical_coord.h>
#include <vsph/vsph_view_point.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_camera.h>

#include <vnl/vnl_math.h>

vsph_view_point<double> create_view_point(double radius, double theta, double phi, double* data)
{
  vpgl_perspective_camera<double>* cam = new vpgl_perspective_camera<double>();
  vsph_sph_point_3d view_point(radius, theta, phi);
  vgl_point_3d<double> camera_center = view_point.cart_coord();
  cam->set_camera_center(camera_center);
  vsph_view_point<double> vp(view_point,cam,data);
  return vp;
}

static void test_view_sphere()
{
  // define the spherical coordinate system
  vgl_point_3d<double> origin(0,0,0);
  double radius = 10.0;
  vsph_spherical_coord_sptr coord=new vsph_spherical_coord(origin, radius);

  vsph_view_sphere<vsph_view_point<double> > vs(coord);
 
  double theta=vnl_math::pi/4.0;
  double phi=vnl_math::pi/4.0;
  double* val0 = new double(0.0);
  vsph_view_point<double> vp0=create_view_point(radius,theta,phi,val0);
  vs.add_view(vp0);

  theta=0;
  phi=0;
  double* val1 = new double(1.0);
  vsph_view_point<double> vp1=create_view_point(radius,theta,phi,val1);
  vs.add_view(vp1);

  theta=vnl_math::pi/2.0;
  phi=0;
  double* val2 = new double(1.0);
  vsph_view_point<double> vp2=create_view_point(radius,theta,phi,val2);
  vs.add_view(vp2);
  
  // test the iterators
}

TESTMAIN(test_view_sphere);