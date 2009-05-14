#include <testlib/testlib_test.h>
#include <boxm/boxm_utils.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <boxm/boxm_quad_scan_iterator.h>


#include <vpl/vpl.h>


MAIN( test_quad_interpolate )
{
  START ("QUAD INTERPOLATE");
  vcl_vector<vgl_point_2d<double> > points;
  points.push_back(vgl_point_2d<double>(10,10));
  points.push_back(vgl_point_2d<double>(10,12));
  points.push_back(vgl_point_2d<double>(12,12));
  points.push_back(vgl_point_2d<double>(12,10));

  double xvals[]={10,10,12,12};
  double yvals[]={10,12,12,10};
  double vals[]={10,10,12,12};
  //vgl_polygon<double> poly(points);
  boxm_quad_scan_iterator poly_it(xvals,yvals);
  vil_image_view<float> img_min(40,40);
  vil_image_view<float> img_max(40,40);
  vil_image_view<float> g_img_max(40,40);

  img_max.fill(0.0);
  g_img_max.fill(0.0);
  // creating ground truth
  g_img_max(10,10)=10;g_img_max(10,11)=10;
  g_img_max(11,10)=11;g_img_max(11,11)=11;

  boxm_utils::quad_interpolate(poly_it, xvals, yvals, vals,img_max,0);
  for (unsigned i=10;i<=12;i++)
  {
    for (unsigned j=10;j<=12;j++)
    {
      vcl_cout << img_max(j,i) << ' ';
    }
    vcl_cout << '\n';
  }
  bool flag=true;
  for (unsigned i=0;i<g_img_max.ni();i++)
  for (unsigned j=0;j<g_img_max.ni();j++)
    if (g_img_max(i,j)!=img_max(i,j))
      flag=false;

  TEST("Interpolated image", true, flag);
  SUMMARY();
}
