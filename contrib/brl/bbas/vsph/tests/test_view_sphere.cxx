#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>

#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_spherical_coord.h>
#include <vsph/vsph_view_point.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <vnl/vnl_math.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vsph_view_point<double> create_view_point(const vsph_spherical_coord_sptr& coord, double theta, double phi, double* data)
{
  auto* cam = new vpgl_perspective_camera<double>();
  vsph_sph_point_3d view_point(coord->radius(), theta, phi);
  vgl_point_3d<double> camera_center = coord->cart_coord(view_point);
  cam->set_camera_center(camera_center);
  cam->look_at(vgl_homg_point_3d<double>(coord->origin()));
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
  vsph_view_sphere<vsph_view_point<double> >::iterator it;

  double theta=vnl_math::pi/4.0;
  double phi=vnl_math::pi/4.0;
  auto* val0 = new double(0.0);
  vsph_view_point<double> vp0=create_view_point(coord,theta,phi,val0);
  vs.add_view(vp0, 0,0);

  theta=0;
  phi=0;
  auto* val1 = new double(1.0);
  vsph_view_point<double> vp1=create_view_point(coord,theta,phi,val1);
  vs.add_view(vp1, 0,0);

  theta=vnl_math::pi/2.0;
  phi=0;
  auto* val2 = new double(2.0);
  vsph_view_point<double> vp2=create_view_point(coord,theta,phi,val2);
  vs.add_view(vp2, 0,0);

  // test the iterators
  it = vs.begin();
  int count=0;
  bool good=true;
  while (it != vs.end()) {
    good = good && (int(it->first) == count);
    std::cout << it->first << std::endl;
    it->second.print(std::cout);
    ++count;
    ++it;
  }

  TEST_EQUAL("Number of views by the iterator",  count, vs.size());
  TEST_EQUAL("The data at the view points are correct",  good, true);

  // test the closest view point
  int uid;
  double dist;
  vs.find_closest(1, uid, dist);
  TEST_EQUAL("Test find_closest(i)",  uid, 0);
  vs.find_closest(vgl_point_3d<double>(0,0,10), uid, dist);
  TEST_EQUAL("Test find_closest(p)",  uid, 1);

  // test remove view
  vs.remove_view(1);
  TEST_EQUAL("Test remove view",  vs.size(), 2);

  delete val0;
  delete val1;
  delete val2;
}

TESTMAIN(test_view_sphere);
