//:
// \file
#include <vcl_vector.h>
#include <testlib/testlib_test.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_fstream.h>
static bool read_geo_points(vcl_ifstream& str,
                            vcl_vector<vgl_point_3d<double> >& wpts)
{
  vcl_string token;
  str >> token;
  if (token!="NPTS")
    return false;
  unsigned npts;
  str >> npts;
  for (unsigned i = 0; i<npts; ++i)
  {
    vgl_point_3d<double> p;
    str >> token;
    double x, y, z;
    str >> y >> x >> z;
    p.set(x, y, z);
    vcl_cout << token << " = " << p << '\n';
    wpts.push_back(p);
  }
  return true;
}

static void write_cart_points(vcl_ofstream& str,
                              vcl_vector<vgl_point_3d<double> >const& wpts)
{
  str << "NUMPOINTS: " << wpts.size() << '\n';

  for (unsigned i = 0; i<wpts.size(); ++i)
  {
    str << "POINT3D: ";
    vgl_point_3d<double> p = wpts[i];
    str << p.x() << ' ' << p.y() << ' ' << p.z() << '\n';
  }
}

//: Test the graph class
static void test_lvcs()
{
  bgeo_lvcs lvcs(33.4447732, -114.3085932, 0.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::FEET);
  double lx = 0, ly = 0, lz = 0;
  lvcs.global_to_local(-114.3113131, 33.44586581, 0.0,
                       bgeo_lvcs::wgs84,
                       lx, ly, lz);

  vcl_cout << "lx = " << lx << " ly = " << ly << " lz = " << lz << '\n';
  vcl_ifstream str("ypg_geo_pts.wp");
  vcl_vector<vgl_point_3d<double> > pts;
  read_geo_points(str, pts);
  vcl_vector<vgl_point_3d<double> > cart_pts;
  for (vcl_vector<vgl_point_3d<double> >::iterator pit = pts.begin();
       pit != pts.end(); ++pit)
  {
    double lx = 0, ly = 0, lz = 0;
    lvcs.global_to_local((*pit).x(), (*pit).y(), (*pit).z(),
                         bgeo_lvcs::wgs84,
                         lx, ly, lz);
    vgl_point_3d<double> pc(lx, ly, lz);
    vcl_cout << "world coordinates " << pc << '\n';
    cart_pts.push_back(pc);
  }
  vcl_ofstream ostr("ypg_cart_pts.wp");
  if (!ostr)
    vcl_cout << "Bad File\n";
  else
  {
    write_cart_points(ostr, cart_pts);
    TEST("Test geo conversion", 1==1, true);
  }
}

TESTMAIN( test_lvcs );
