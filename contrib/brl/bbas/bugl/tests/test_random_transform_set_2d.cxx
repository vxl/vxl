#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <bugl/bugl_random_transform_set_2d.h>
#include <vgl/vgl_homg_point_2d.h>

void test_random_transform_set_2d()
{
  vcl_cout << "=====Testing random transform set =======\n";

  float dx = 1.0, dy = 2.0;
  unsigned n = 1000;
  vcl_vector<vgl_h_matrix_2d<float> > trans_set;
  bugl_random_transform_set_2d<float>::equiform_uniform(n, trans_set, dx, dy);
  float sumx = 0, sumy = 0;
  float min_x = 0, max_x = 0;
  float min_y = 0, max_y = 0;
  for (unsigned i = 0; i<n; ++i)
  {
    // vcl_cout << "\n T[" << i << "]\n"
    //          << trans_set[i];
    vgl_homg_point_2d<float> p(0.0f, 0.0f), pt;
    pt = trans_set[i](p);
    sumx += pt.x();  sumy += pt.y();
    if (pt.x()<min_x) min_x = pt.x();
    if (pt.y()<min_y) min_y = pt.y();
    if (pt.x()>max_x) max_x = pt.x();
    if (pt.y()>max_y) max_y = pt.y();
  }
  sumx /= n; sumy /= n;
  vcl_cout << "tavg (" << sumx << ' ' << sumy << ")\n"
           << "X bounds (" << min_x << ' ' << max_x << ")\n"
           << "Y bounds (" << min_y << ' ' << max_y << ")\n";
  TEST_NEAR("test average transform ", vcl_fabs(sumx)+vcl_fabs(sumy), 0, 0.2);
  TEST_NEAR("test X bounds " , max_x - min_x , 2.0, 0.2);
  TEST_NEAR("test Y bounds " , max_y - min_y , 4.0, 0.2);
  //==== test rotation
  vcl_cout << "\n\n====================Testing random set of rotations ============================\n";
  float dtheta = float(vnl_math::pi/2);
  n = 1000;
  vcl_vector<vgl_h_matrix_2d<float> > rtrans_set;
  bugl_random_transform_set_2d<float>::equiform_uniform(n, rtrans_set, 0, 0,
                                                        dtheta);
  sumx = 0; sumy = 0;
  min_x = 0; max_x = 0;
  min_y = 0; max_y = 0;
  for (unsigned i = 0; i<n; ++i)
  {
    vgl_homg_point_2d<float> p(1.0, 0.0f), pt;
    pt = rtrans_set[i](p);
    sumx += pt.x();  sumy += pt.y();
    if (pt.x()<min_x) min_x = pt.x();
    if (pt.y()<min_y) min_y = pt.y();
    if (pt.x()>max_x) max_x = pt.x();
    if (pt.y()>max_y) max_y = pt.y();
  }
  sumx /= n; sumy /= n;
  vcl_cout << "tavg (" << sumx << ' ' << sumy << ")\n"
           << "X bounds (" << min_x << ' ' << max_x << ")\n"
           << "Y bounds (" << min_y << ' ' << max_y << ")\n";
  TEST_NEAR("test average transform ", vcl_fabs(sumx)+vcl_fabs(sumy), 0.5, 0.2);
  TEST_NEAR("test X bounds " , max_x - min_x , 1.0, 0.2);
  TEST_NEAR("test Y bounds " , max_y - min_y , 2.0, 0.2);
   //==== scale
  vcl_cout << "\n\n====================Testing random set of scales ============================\n";
   n = 1000;
  vcl_vector<vgl_h_matrix_2d<float> > strans_set;
  bugl_random_transform_set_2d<float>::equiform_uniform(n, strans_set, 0, 0,
                                                        0, 0.5);
  sumx = 0; sumy = 0;
  min_x = 1.0; max_x = 0;
  min_y = 2.0; max_y = 0;
  for (unsigned i = 0; i<n; ++i)
  {
    vgl_homg_point_2d<float> p(1.0, 2.0f), pt;
    pt = strans_set[i](p);
    sumx += pt.x();  sumy += pt.y();
    if (pt.x()<min_x) min_x = pt.x();
    if (pt.y()<min_y) min_y = pt.y();
    if (pt.x()>max_x) max_x = pt.x();
    if (pt.y()>max_y) max_y = pt.y();
  }
  sumx /= n; sumy /= n;
  vcl_cout << "tavg (" << sumx << ' ' << sumy << ")\n"
           << "X bounds (" << min_x << ' ' << max_x << ")\n"
           << "Y bounds (" << min_y << ' ' << max_y << ")\n";
  TEST_NEAR("test average transform ", vcl_fabs(sumx)+vcl_fabs(sumy), 3.0, 0.2);
  TEST_NEAR("test X bounds " , max_x/min_x , 3.0, 0.2);
  TEST_NEAR("test Y bounds " , max_y/min_y , 3.0, 0.2);
}

TESTMAIN(test_random_transform_set_2d);
