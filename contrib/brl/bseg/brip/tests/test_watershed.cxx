#include <queue>
#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_region_pixel.h>
#include <brip/brip_watershed_params.h>
#include <brip/brip_watershed.h>

static float parabola(float r, float vmin, float vmax, float x0, float y0,
                      float x, float y)
{
  float arg_sq = (x-x0)*(x-x0) + (y-y0)*(y-y0);
  float dv = vmax-vmin;
  float v = (dv*arg_sq)/(r*r);
  if (v>dv)
    return vmax;
  else
    return v+vmin;
}

static void test_watershed()
{
  //Test region pixel
  vgl_point_2d<int> location(0, 0), nearest1(1, 1), nearest2(2,2);

  brip_region_pixel_sptr pixel1 =
    new brip_region_pixel(location, nearest1, 0, 0, 0);
  brip_region_pixel_sptr pixel2 =
    new brip_region_pixel(location, nearest2, 0, 0, 0);
  brip_region_pixel::compare c;
  std::cout << "compare c() " << c(pixel1, pixel2) << '\n'<< std::flush;
#if 0
  std::priority_queue<brip_region_pixel_sptr, std::vector<brip_region_pixel_sptr>, brip_region_pixel::compare> pq;

  pq.push(pixel1);
  pq.push(pixel2);
  brip_region_pixel_sptr top = pq.top();
  std::cout << "top element " << *top << '\n'
           << "pq.size() " << pq.size() << '\n'<< std::flush;
#endif
  //  TEST("",x  , y);
  //Create test image for watershed
  int w = 12, h = 12;
  vil1_memory_image_of<float> input(w,h);
  input.fill(0);
  // create a parabolic dish
  for (int r = 0; r<h; r++)
  {
    for (int c = 0; c<w; c++)
    {
      input(c,r) = parabola(2.f, 0.f, 255.f, 4.f, 4.f, float(c), float(r))
                 + parabola(2.f, 0.f, 255.f, 7.f, 7.f, float(c), float(r))
                 + parabola(2.f, 0.f, 255.f, 3.f, 8.f, float(c), float(r));
      std::cout << ' ' << input(c,r);
    }
    std::cout << std::endl;
  }
  brip_watershed_params wp;
  brip_watershed ws(wp);
  ws.set_image(input);
  ws.compute_regions();
  ws.print_region_array();
  ws.print_adjacency_map();
  std::vector<unsigned int> adj;
  ws.adjacent_regions(3, adj);
  std::cout << "# adjacent to 3 " << adj.size() << '\n';
  TEST("Number of adjacent regions", adj.size(), 3);
}

TESTMAIN(test_watershed);
