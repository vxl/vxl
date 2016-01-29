// This is brl/bseg/sdet/tests/test_detector.cxx
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

static void test_detector(int argc, char * argv[])
{
  vcl_string root = testlib_root_dir();
  vcl_string image_path = root + "/contrib/brl/bseg/sdet/tests/jar-closeup.tif";

  vcl_cout << "Loading Image " << image_path << '\n';
  vil_image_resource_sptr image = vil_load_image_resource(image_path.c_str() );
  if( image )
    {
    sdet_detector_params dp;
    dp.noise_multiplier = 1;
    dp.aggressive_junction_closure = 1;
    sdet_detector det(dp);
    det.SetImage(image);
    det.DoContour();
    vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
    int                            n = 0;
    if( edges )
      {
      n = edges->size();
      }
    vcl_cout << "nedges = " << n << '\n';
    TEST("nedges is around 648", n >= 647 && n <= 650, true);
    if( n )
      {
      vtol_edge_2d_sptr e = (*edges)[0];
      int               x = int(e->v1()->cast_to_vertex_2d()->x() );
      int               y = int(e->v1()->cast_to_vertex_2d()->y() );
      vcl_cout << "v(" << x << ' ' << y << ")\n";
      TEST("(x,y) is (229,235)", x == 229 && y == 235, true);
      }
    }
  else
    {
    TEST("image could not be loaded so no fault", true, true);
    }
}

TESTMAIN_ARGS(test_detector);
