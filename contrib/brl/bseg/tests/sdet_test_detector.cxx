// This is brl/bseg/tests/sdet_test_detector.cxx
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int argc, char * argv[])
{
  int success=0, failures=0;
  vcl_string image_path = (argc < 2) ? "" : argv[1];
  if (image_path=="")
    image_path = "jar-closeup.tif";
  vcl_cout << "Loading Image " << image_path << "\n";
  vil_image image = vil_load(image_path.c_str());
  if (image)
    {
      sdet_detector_params dp;
      dp.noise_multiplier= 1;
      dp.aggressive_junction_closure=1;
      sdet_detector det(dp);
      det.SetImage(image);
      det.DoContour();
      vcl_vector<vtol_edge_2d_sptr>* edges = det.GetEdges();
      int n = 0;
      if (edges)
        n = edges->size();
      vcl_cout << "nedges = " << n << "\n";
      Assert(n==647 || n==648);
      int x = 0, y = 0;
      if (n)
        {
          vtol_edge_2d_sptr e = (*edges)[0];
          x = int(e->v1()->cast_to_vertex_2d()->x());
          y = int(e->v1()->cast_to_vertex_2d()->y());
          vcl_cout << "v(" << x << " " << y << ")\n";
          Assert(x==229&&y==235);
        }
    }
  vcl_cout << "finished testing sdet_detector\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
