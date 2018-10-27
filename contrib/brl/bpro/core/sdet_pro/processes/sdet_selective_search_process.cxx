// This is brl/bpro/core/sdet_pro/processes/sdet_selective_search_process.cxx
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <sdet/sdet_selective_search.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <volm/volm_io_tools.h>

using namespace std;

static double eps = 1.0e-5;

template <class T>
bool near_zero(T x) { return x < eps && x > -eps; }
template <class T>
bool near_equal(T x, T y) { return near_zero(x-y); }
template <class T>
bool near_eq_pt(vgl_point_2d<T> a, vgl_point_2d<T> b)
{
  return (near_equal(a.x(),b.x()) && near_equal(a.y(), b.y()));
}

template <class T>
bool expand_line(std::vector<vgl_point_2d<T> > line, double const& w, vgl_polygon<T>& poly)
{
  std::vector<vgl_point_2d<T> > sheet;
  std::vector<vgl_point_2d<T> > pts_u;
  std::vector<vgl_point_2d<T> > pts_d;
  unsigned n_pts = line.size();
  for (unsigned i = 0; i < n_pts-1; i++) {
    vgl_point_2d<T> s, e;
    s = line[i];  e = line[i+1];
    if (near_eq_pt(s,e))
      continue;
    vgl_line_2d<T> seg(s, e);
    vgl_vector_2d<T> n = seg.normal();
    vgl_point_2d<T> su, sd, eu, ed;
    su = s + 0.5*w*n;  sd = s - 0.5*w*n;
    pts_u.push_back(su);  pts_d.push_back(sd);
    if (i == n_pts-2) {
      eu = e + 0.5*w*n;  ed = e - 0.5*w*n;
      pts_u.push_back(eu);  pts_d.push_back(ed);
    }
  }
  // rearrange the point list
  for (unsigned i = 0; i < pts_u.size(); i++)
    sheet.push_back(pts_u[i]);
  for (int i = pts_d.size()-1; i >=0; i--)
    sheet.push_back(pts_d[i]);
  poly.push_back(sheet);
  return true;
}


bool draw_box(vil_image_view<vil_rgb<vxl_byte> >& output_img, vgl_oriented_box_2d<float>& bb, vil_rgb<vxl_byte> color) {
    vector<vgl_point_2d<float> > line = bb.corners();

    vgl_polygon<float> img_poly;
    double width = 2;
    if (!expand_line<float>(line, width, img_poly)) {
      cout << " expending osm line failed!" << endl;
      return false;
    }

    //vgl_polygon_scan_iterator<double> it(img_poly, true);
    vgl_polygon_scan_iterator<float> it(img_poly, false);
    for (it.reset(); it.next();  ) {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        if ( x >= 0 && y >= 0 && x < output_img.ni() && y < output_img.nj()) {
          output_img(x,y) = color;
        }
      }
    }
    return true;
}


//: global variables
namespace sdet_selective_search_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 2;
}

bool sdet_selective_search_process_cons(bprb_func_process& pro)
{
  using namespace sdet_selective_search_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float";  // weight threshold, depends on dissimilarity metric,
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

bool sdet_selective_search_process(bprb_func_process& pro)
{
 using namespace sdet_selective_search_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  auto weight_thres = pro.get_input<float>(1);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  vil_image_view<vxl_byte> input_image(input_image_sptr);
  float ss_sigma = 1.0f;
  float ss_vd_noise_mul = 1.25f;
  int ss_four_or_eight_conn = 8;
  unsigned ss_margin = 10;
  double ss_weight_thresh = 0;
  int ss_min_region_size = 10;
  unsigned ss_nbins = 16;
  bool ss_verbose = true;
  bool ss_debug = false;
  bool ss_use_vd_edges = false;

  sdet_selective_search_params params(ss_use_vd_edges, ss_sigma, ss_vd_noise_mul, ss_four_or_eight_conn, ss_margin,
  ss_weight_thresh, ss_min_region_size, ss_nbins, ss_verbose, ss_debug);

  sdet_selective_search sproc(params);
  sproc.set_byte_image_view(input_image);
  sproc.process();
  vil_image_view<vil_rgb<vxl_byte> >& cimg = sproc.color_region_image();
  vil_image_view<vil_rgb<vxl_byte> > bb_img(input_image.ni(), input_image.nj());
  bb_img.fill(vil_rgb<vxl_byte>(0,0,0));
  vil_rgb<vxl_byte> color = vil_rgb<vxl_byte>(255,0,0);

  double aspect_ratio_t = 4.5;
  double length_t = 70;
  const std::map<unsigned, sdet_region_sptr>& sregions=sproc.diverse_regions();
  auto rit = sregions.begin();
  for(;rit != sregions.end(); ++rit){
    //vgl_box_2d<float> bb =(*rit).second->obox();
    vgl_oriented_box_2d<float> bb = (*rit).second->obox();
    vgl_point_2d<float> lw(bb.width(), bb.height());
    //tr.add(rit->second);
    if (bb.aspect_ratio() > aspect_ratio_t && lw.x() < length_t) {
      cout << rit->first << ' ' << bb << " " << lw.y() << " " << lw.x() << endl;
      draw_box(bb_img, bb, color);
    }

  }

  // return the output color image
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vil_rgb<vxl_byte> >(cimg));
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<vil_rgb<vxl_byte> >(bb_img));

  return true;
}
