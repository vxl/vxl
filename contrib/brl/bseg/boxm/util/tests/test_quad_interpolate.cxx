#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <boxm/util/boxm_utils.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <boxm/util/boxm_quad_scan_iterator.h>
#include <vgl/vgl_box_3d.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <vil/vil_save.h>
#include <vil/vil_plane.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>


static void test_quad_interpolate(int argc, char* argv[])
{
  std::vector<vgl_point_2d<double> > points;
  points.emplace_back(10,10);
  points.emplace_back(10,12);
  points.emplace_back(12,12);
  points.emplace_back(12,10);

  double xvals[]={10,10,12,12};
  double yvals[]={10,12,12,10};
  double vals[]={10,10,12,12};
  //vgl_polygon<double> poly(points);
  boxm_quad_scan_iterator poly_it(xvals,yvals);
  vil_image_view<float> img_min(40,40);
  vil_image_view<float> img_max(40,40);
  vil_image_view<float> g_img_max(40,40);

  img_max.fill(0.0f);
  g_img_max.fill(0.0f);
  // creating ground truth
  g_img_max(10,10)=10.5f; g_img_max(10,11)=10.5f;
  g_img_max(11,10)=11.5f; g_img_max(11,11)=11.5f;

  boxm_utils::quad_interpolate(poly_it, xvals, yvals, vals,img_max,0);
  for (unsigned i=10;i<=12;i++)
  {
    for (unsigned j=10;j<=12;j++)
    {
      std::cout << img_max(j,i) << ' ';
    }
    std::cout << '\n';
  }
  bool flag=true;
  for (unsigned i=0; i<g_img_max.ni() && flag; ++i)
  for (unsigned j=0; j<g_img_max.nj() && flag; ++j)
    if (g_img_max(i,j)!=img_max(i,j))
      flag=false;

  TEST("Interpolated image", true, flag);

  std::vector<vgl_point_2d<double> > points1;
  points.emplace_back(10.25,10.25);
  points.emplace_back(10.25,12.25);
  points.emplace_back(12.25,12.25);
  points.emplace_back(12.25,10.25);

  double xvals1[]={10.3,10.3,11.4,11.4};
  double yvals1[]={10.3,11.4,11.4,10.3};
  //double vals1[]={10,10,12,12};
  ////vgl_polygon<double> poly(points);
  boxm_quad_scan_iterator poly_it_1(xvals1,yvals1);
  //vil_image_view<float> img_min(40,40);
  //vil_image_view<float> img_max(40,40);

  g_img_max.fill(0.0f);
  // creating ground truth
  g_img_max(10,10)=10;g_img_max(10,11)=10;
  g_img_max(11,10)=10;g_img_max(11,11)=10;

  float val=0;
  float count=0;
  boxm_utils::quad_mean(poly_it_1,g_img_max,val,count);

  // code to test the projection of a cube .
  vgl_box_3d<double> cell_bb(-0.8,19.2,-18.75,0.7625, 20.7625,-17.1875);
  std::string camera_input_path = (argc < 2) ? "." : argv[1];
  std::ifstream ifs((camera_input_path + "/frame_00000.txt").c_str());
  if (!ifs.is_open()) {
    std::cerr << "Failed to open file " << camera_input_path << "/frame_00000.txt\n";
    TEST("Opening camera file", true, false);
    return;
  }
  auto* cam = new vpgl_perspective_camera<double>();
  ifs >> *cam;
  double xverts[8];
  double yverts[8];
  float vertdist[8];
  unsigned int ni=1280;
  unsigned int nj=720;

  vpgl_camera_double_sptr cam_d(cam);
  vil_image_view<float> front_xyz(ni,nj,1);front_xyz.fill(0.0f);
  vil_image_view<float> back_xyz(ni,nj,1) ;back_xyz.fill(0.0f);

  std::vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
  boxm_utils::project_corners(corners,cam_d,xverts,yverts,vertdist);
  boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam_d,xverts,yverts);
  boxm_utils::project_cube_xyz(corners,vis_face_ids,front_xyz,back_xyz,xverts,yverts,vertdist);

  std::string image_output_path = (argc < 3) ? "." : argv[2];
  std::cout << "Saving two TIFF files to directory " << image_output_path << '\n';
  vil_save(vil_plane(front_xyz,0),(image_output_path + "/front.tiff").c_str());
  vil_save(vil_plane(back_xyz,0), (image_output_path + "/back.tiff").c_str());
}

TESTMAIN_ARGS(test_quad_interpolate);
