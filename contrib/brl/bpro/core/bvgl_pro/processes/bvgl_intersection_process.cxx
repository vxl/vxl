// This is brl/bpro/core/bvgl_pro/processes/bvgl_intersection_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// processes to operate intersection operation available in vgl core

#include <vgl/vgl_intersection.h>
#include <bsol/bsol_algs.h>
#include <vul/vul_file.h>
#include <bkml/bkml_parser.h>
#include <bkml/bkml_write.h>


//: a process to return the intersection of multiple 2d rectangular boxes loaded from kml
namespace bvgl_2d_box_intersection_process_globals
{
  unsigned n_inputs_   = 2;
  unsigned n_outputs_ = 4;

  vgl_box_2d<double> intersection(std::vector<vgl_box_2d<double> > const& boxes);
}

bool bvgl_2d_box_intersection_process_cons(bprb_func_process& pro)
{
  using namespace bvgl_2d_box_intersection_process_globals;
  // this process takes 1 input
  std::vector<std::string> input_types(n_inputs_);
  input_types[0] = "vcl_string";  // the input kml file that contains 2d rectangular boxes
  input_types[1] = "vcl_string";  // the output kml file that contains the intersection region, if a filename is given
  // this process has 5 outputs
  std::vector<std::string> output_types(n_outputs_);
  output_types[0] = "double";  // the lower left lon
  output_types[1] = "double";  // the lower left lat
  output_types[2] = "double";  // the upper right lon
  output_types[3] = "double";  // the upper right lat
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool bvgl_2d_box_intersection_process(bprb_func_process& pro)
{
  using namespace bvgl_2d_box_intersection_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
  }

  // get the inputs
  unsigned in_i = 0;
  std::string in_kml  = pro.get_input<std::string>(in_i++);
  std::string out_kml = pro.get_input<std::string>(in_i++);

  if (!vul_file::exists(in_kml)) {
    std::cerr << pro.name() << ": can not find input kml file: " << in_kml << "!\n";
    return false;
  }

  // parse the boxes from input kml
  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_kml);
  std::vector<vgl_box_2d<double> > boxes;
  unsigned n_sheet = poly.num_sheets();
  for (unsigned i = 0; i < n_sheet; i++) {
    vgl_box_2d<double> bbox;
    for (auto v_idx : poly[i])
      bbox.add(v_idx);
    boxes.push_back(bbox);
  }

  // find the intersection by iteration
  vgl_box_2d<double> inc_box;
  if (boxes.size() == 1)
    inc_box = boxes[0];
  else
    inc_box = intersection(boxes);

  if (inc_box.is_empty()) {
    std::cerr << "there is no intersection among the " << boxes.size() << " boxes!\n";
    return false;
  }
  double lower_left_lon  = inc_box.min_x();
  double lower_left_lat  = inc_box.min_y();
  double upper_right_lon = inc_box.max_x();
  double upper_right_lat = inc_box.max_y();

  // generate kml if necessary
  if (out_kml.compare("") != 0) {
    std::ofstream ofs(out_kml.c_str());
    if (!ofs) {
      std::cerr << pro.name() << ": failed to open output stream: " << out_kml << std::endl;
      return false;
    }
    else {
      bkml_write::open_document(ofs);
      vgl_polygon<double> inc_poly;
      inc_poly.new_sheet();
      inc_poly.push_back(inc_box.min_x(), inc_box.max_y());
      inc_poly.push_back(inc_box.max_x(), inc_box.max_y());
      inc_poly.push_back(inc_box.max_x(), inc_box.min_y());
      inc_poly.push_back(inc_box.min_x(), inc_box.min_y());
      bkml_write::write_polygon(ofs, inc_poly, "box_intersection", "box_intersection");
      bkml_write::close_document(ofs);
      ofs.close();
    }
  }
  // generate outputs
  unsigned out_i = 0;
  pro.set_output_val<double>(out_i++, lower_left_lon);
  pro.set_output_val<double>(out_i++, lower_left_lat);
  pro.set_output_val<double>(out_i++, upper_right_lon);
  pro.set_output_val<double>(out_i++, upper_right_lat);

  return true;
}

// iteration to obtain the intersections of multiple boxes
vgl_box_2d<double> bvgl_2d_box_intersection_process_globals::intersection(std::vector<vgl_box_2d<double> > const& boxes)
{
  if (boxes.size() == 2) {
    return vgl_intersection(boxes[0], boxes[1]);
  }
  std::vector<vgl_box_2d<double> > new_boxes;
  vgl_box_2d<double> box = vgl_intersection(boxes[0], boxes[1]);
  if (box.is_empty())
    return box;
  new_boxes.push_back(box);
  for (unsigned i = 2; i < boxes.size(); i++)
    new_boxes.push_back(boxes[i]);
  return bvgl_2d_box_intersection_process_globals::intersection(new_boxes);
}
