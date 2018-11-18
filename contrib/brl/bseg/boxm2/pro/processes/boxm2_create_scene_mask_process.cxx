// This is brl/bseg/boxm2/pro/processes/boxm2_create_scene_mask_process.cxx
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
//:
// \file

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>

#include <boxm2/boxm2_scene.h>

#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_convex.h>



namespace boxm2_create_scene_mask_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}

void fill_in(vil_image_view<vxl_byte>* mask, vgl_polygon<double>& poly) {
  unsigned ni = mask->ni(); unsigned nj = mask->nj();
  vgl_polygon_scan_iterator<double> psi(poly);
  for (psi.reset(); psi.next(); ) {
    int y = psi.scany();
    for (int x = psi.startx(); x <= psi.endx(); ++x) {
      if (x < 0 || x >= (int)ni || y < 0 || y >= (int)nj)
        continue;
      (*mask)(x,y) = vxl_byte(255);
    }
  }
}

//: set input and output types
bool boxm2_create_scene_mask_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_scene_mask_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned  i = 0;
  input_types_[i++] = "boxm2_scene_sptr";     // scene
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "unsigned";
  input_types_[i++] = "unsigned";
  input_types_[i++] = "bool";   // only ground plane
  bool good = pro.set_input_types(input_types_);

  unsigned j = 0;
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[j++] = "vil_image_view_base_sptr";  // mask image

  good = good && pro.set_output_types(output_types_);
  return good;
}

bool boxm2_create_scene_mask_process(bprb_func_process& pro)
{
  using namespace boxm2_create_scene_mask_process_globals;
  //static const parameters

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs:
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni = pro.get_input<unsigned>(i++);
  auto nj = pro.get_input<unsigned>(i++);
  bool only_ground_plane = pro.get_input<bool>(i++);

  vgl_box_3d<double> bbox = scene->bounding_box();
  double x = bbox.min_point().x(); double y = bbox.min_point().y(); double z = bbox.min_point().z();
  double w = bbox.width(); double h = bbox.height(); double d = bbox.depth();

  vgl_polygon<double> poly(1); double u,v;
  auto* mask = new vil_image_view<vxl_byte>(ni, nj);
  mask->fill(vxl_byte(0));

  // put the ground plane first
  std::vector<vgl_point_2d<double> > vv;
  camera->project(x, y, z, u, v); vv.emplace_back(u,v);
  camera->project(x+w, y, z, u, v); vv.emplace_back(u,v);
  camera->project(x, y+h, z, u, v); vv.emplace_back(u,v);
  camera->project(x+w, y+h, z, u, v); vv.emplace_back(u,v);
  poly = vgl_convex_hull(vv);
  fill_in(mask, poly); poly.clear(); poly.new_sheet();

  if (!only_ground_plane) {
    std::cout << "in !only ground plane\n";
    //: front plane
    camera->project(x, y, z, u, v); vv.emplace_back(u,v);
    camera->project(x, y, z+d, u, v); vv.emplace_back(u,v);
    camera->project(x+w, y, z, u, v); vv.emplace_back(u,v);
    camera->project(x+w, y, z+d, u, v); vv.emplace_back(u,v);
    poly = vgl_convex_hull(vv);
    fill_in(mask, poly); poly.clear(); poly.new_sheet();

    //: right side plane
    camera->project(x+w, y, z, u, v); vv.emplace_back(u,v);
    camera->project(x+w, y+h, z, u, v); vv.emplace_back(u,v);
    camera->project(x+w, y+h, z+d, u, v); vv.emplace_back(u,v);
    camera->project(x+w, y, z+d, u, v); vv.emplace_back(u,v);
    poly = vgl_convex_hull(vv);
    fill_in(mask, poly); poly.clear(); poly.new_sheet();

    //: back side plane
    camera->project(x+w, y+h, z, u, v); vv.emplace_back(u,v);
    camera->project(x+w, y+h, z+d, u, v); vv.emplace_back(u,v);
    camera->project(x, y+h, z+d, u, v); vv.emplace_back(u,v);
    camera->project(x, y+h, z, u, v); vv.emplace_back(u,v);
    poly = vgl_convex_hull(vv);
    fill_in(mask, poly); poly.clear(); poly.new_sheet();

    //: left side plane
    camera->project(x, y+h, z, u, v); vv.emplace_back(u,v);
    camera->project(x, y+h, z+d, u, v); vv.emplace_back(u,v);
    camera->project(x, y, z+d, u, v); vv.emplace_back(u,v);
    camera->project(x, y, z, u, v); vv.emplace_back(u,v);
    poly = vgl_convex_hull(vv);
    fill_in(mask, poly); poly.clear(); poly.new_sheet();
  }

  vil_image_view_base_sptr img_sptr = mask;
  pro.set_output_val<vil_image_view_base_sptr>(0,img_sptr);

  return true;
}
