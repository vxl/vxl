#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsph/vsph_unit_sphere.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vsph/vsph_utils.h>
#include <vsph/vsph_segment_sphere.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vnl/vnl_math.h>
//
// USED ONLY FOR EXPERIMENTS - NOT A ROUTINE TEST
//
static std::string MyDIR = "./";

#if 0 // only used in commented-out code below
static void random_rgb(float& r, float&g, float& b)
{
  float rmax = static_cast<float>(RAND_MAX);
  r = static_cast<float>(std::rand())/rmax;
  g = static_cast<float>(std::rand())/rmax;
  b = static_cast<float>(std::rand())/rmax;
}
#endif

#if 0 // This static function is not used in here at all
static std::vector<std::vector<float> > convert_orient_code(std::vector<unsigned char> const& cdata)
{
  unsigned n = cdata.size();
  std::vector<std::vector<float> > cres(n);
  for (unsigned i = 0; i<n; ++i) {
    unsigned c = static_cast<unsigned>(cdata[i]);
    std::vector<float> cd(3, 0.0f);
    if (c == 255) {cres[i] = cd; continue;}
    if (c == 100) {cres[i] = cd; continue;}
    if (c == 1) {cd[0]=1.0f; cd[1]=0.5f; cd[2]=0.5f;cres[i]=cd; continue;}
    if (c == 2) {cd[0]=0.0f; cd[1]=1.0f; cd[2]=1.0f;cres[i]=cd; continue;}
    if (c == 3) {cd[0]=0.0f; cd[1]=1.0f; cd[2]=0.6f;cres[i]=cd; continue;}
    if (c == 4) {cd[0]=1.0f; cd[1]=1.0f; cd[2]=0.0f;cres[i]=cd; continue;}
    if (c == 5) {cd[0]=0.6f; cd[1]=1.0f; cd[2]=0.0f;cres[i]=cd; continue;}
    if (c == 6) {cd[0]=0.6f; cd[1]=1.0f; cd[2]=0.6f;cres[i]=cd; continue;}
    if (c == 7) {cd[0]=0.6f; cd[1]=0.6f; cd[2]=0.6f;cres[i]=cd; continue;}
    if (c == 8) {cd[0]=0.6f; cd[1]=0.0f; cd[2]=0.6f;cres[i]=cd; continue;}
    if (c == 9) {cd[0]=0.0f; cd[1]=0.6f; cd[2]=0.6f;cres[i]=cd; continue;}
    if (c == 254) {cd[0]=0.0f; cd[1]=0.0f; cd[2]=1.0f;cres[i]=cd; continue;}
    cres[i]=cd;
  }
  return cres;
}
#endif // 0

static void test_sph_segment()
{
#if 0
  vsl_b_ifstream is(MyDIR + "unit_sphere_1_75_105.vsl");
  vsph_unit_sphere_sptr usph;
  vsl_b_read(is, usph);
  int nv = usph->size();
  double dpr = vnl_math::deg_per_rad;
  double reg_min_theta = 85.0/dpr, reg_max_theta = 95.0/dpr;
  double reg_min_phi = -90.0/dpr, reg_max_phi = 90.0/dpr;
  std::vector<double> data(nv, 0.0);
  int cnt = 0;
  vsph_unit_sphere::iterator sit = usph->begin();
  for (; sit!=usph->end(); ++sit, ++cnt)
  {
    vsph_sph_point_2d& sp = *sit;
    if (sp.theta_ >=reg_min_theta && sp.theta_<=reg_max_theta &&
        sp.phi_>=reg_min_phi && sp.phi_<=reg_max_phi)
      data[cnt] = 50.0;
  }


  std::cout << "Start segment" << std::endl;
  double point_angle = usph->point_angle();//for coast (2 really)
  double sigma = (0.1*point_angle)/dpr,  c =300.0;
  int min_size = 10;
  vsph_segment_sphere seg(*usph, sigma, int(c), min_size, true);
  seg.set_data(data);
  /* seg.smooth_data(); -- is private !! */
  seg.segment();
  seg.extract_region_bounding_boxes();
  const std::map<int, vsph_sph_box_2d>& boxes = seg.region_boxes();
  std::vector<vsph_sph_box_2d> dboxes;
  std::map<int, vsph_sph_box_2d>::const_iterator bit = boxes.begin();
  std::vector<std::vector<float> > colors;
  for (; bit != boxes.end(); ++bit) {
    dboxes.push_back(bit->second);
    std::vector<float> c(3);
    random_rgb(c[0],c[1],c[2]);
    colors.push_back(c);
  }
  std::string test_path = MyDIR + "rseg_box_color.wrl";
  double tol = 0.001;
  vsph_sph_box_2d::display_boxes(test_path, dboxes, colors, tol);

  std::string path1 = MyDIR + "vol_indices/geoindex_vsph_zone_17_high_res_only_gt_pa_2/geo_index_tile_3_node_-80.000000_32.500000_-79.500000_33.000000_index_hyp_32.649_-79.9519_1.02686.txt";
  std::string path2 = MyDIR + "vol_indices/geoindex_vsph_zone_17_high_res_only_gt_pa_2/geo_index_tile_3_node_-80.000000_32.500000_-79.500000_33.000000_index_label_orientation_hyp_32.6507_-79.949_3.07477.txt";
  std::string path3 = MyDIR + "vol_indices/geoindex_vsph_zone_17_high_res_only_gt_pa_1/geo_index_tile_3_node_-80.000000_32.500000_-79.500000_33.000000_index_label_orientation_hyp_32.649_-79.9519_1.02686.txt";
  std::vector<unsigned char> cdata;
  vsph_utils::read_ray_index_data(path3, cdata);
  data.clear();
  int n = cdata.size();
  for (int i = 0; i<n; ++i)
    data.push_back(static_cast<double>(cdata[i]));
  std::string data_path2 = MyDIR + "vol_indices/geoindex_vsph_zone_17_high_res_only_gt_pa_1/joe_geo_index_tile_3_node_-80.000000_32.500000_-79.500000_33.000000_index_label_orientation_hyp_32.6507_-79.949_3.07477.wrl";
  // usph->display_data(data_path2, data);
  vsph_segment_sphere seg(*usph, sigma, c, min_size);
  seg.set_data(data);
  seg.smooth_data();
  seg.segment();
  //std::vector<std::vector<float> > cldata = seg.region_color();
  std::string seg_path2 = MyDIR + "vol_indices/geoindex_vsph_zone_17_high_res_only_gt_pa_1/joe_seg_geo_index_tile_3_node_-80.000000_32.500000_-79.500000_33.000000_index_label_orientation_hyp_32.6507_-79.949_3.07477.wrl";
  //usph->display_color(seg_path2, cldata);
  seg.extract_region_bounding_boxes();
  const std::map<int, vsph_sph_box_2d>& boxes = seg.region_boxes();
  std::vector<vsph_sph_box_2d> dboxes;
  std::map<int, vsph_sph_box_2d>::const_iterator bit = boxes.begin();
  std::vector<std::vector<float> > colors;
  for (; bit != boxes.end(); ++bit) {
    dboxes.push_back(bit->second);
    std::vector<float> c(3);
    random_rgb(c[0],c[1],c[2]);
    colors.push_back(c);
  }
  std::string seg_path_boxes = MyDIR + "vol_indices/geoindex_vsph_zone_17_high_res_only_gt_pa_1/joe_seg_boxes_geo_index_tile_3_node_32.649_-79.9519_1.02686.wrl";
  double tol = 0.001;
  vsph_sph_box_2d::display_boxes(seg_path_boxes, dboxes, colors, tol);
#endif // 0
  TEST("Not a routine test - so ok by definition", true , true);
}

TESTMAIN(test_sph_segment);
