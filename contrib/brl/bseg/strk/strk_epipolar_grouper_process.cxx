// This is brl/bseg/strk/strk_epipolar_grouper_process.cxx
#include <vcl_iostream.h>
#include <vsol/vsol_polyline_2d.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_edge.h>
#include <brip/brip_vil1_float_ops.h>
#include <vpro/vpro_edge_process.h>
#include <strk/strk_epipolar_grouper.h>
#include <strk/strk_epipolar_grouper_process.h>

strk_epipolar_grouper_process::
strk_epipolar_grouper_process(sdet_detector_params& dp,
                              strk_epipolar_grouper_params & tp)
  : sdet_detector_params(dp), epipolar_grouper_(tp)
{
  failure_ = false;
  first_frame_ = true;
}

strk_epipolar_grouper_process::~strk_epipolar_grouper_process()
{
}


bool strk_epipolar_grouper_process::execute()
{
  if (failure_)
  {
    vcl_cout << "In strk_epipolar_grouper_process::execute() - process failed\n";
    return false;
  }
  this->clear_output();
  if (first_frame_)
  {
    epipolar_grouper_.init(n_frames_);
    first_frame_ = false;
  }
  //get the input edges
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In vpro_edge_line_process::execute() - not exactly one input image\n";
    return false;
  }
  vpro_edge_process ep(*((sdet_detector_params*)this));
  ep.add_input_image(this->get_input_image(0));
  if (!ep.execute())
  {
    this->clear_input();
    return false;
  }

  vcl_vector<vtol_topology_object_sptr> topo_objs=ep.get_output_topology();
  if (!topo_objs.size())
    return false;

  vcl_vector<vtol_edge_2d_sptr> edges;
  for (vcl_vector<vtol_topology_object_sptr>::iterator tob = topo_objs.begin();
       tob != topo_objs.end(); tob++)
  {
    vtol_edge_sptr e = (*tob)->cast_to_edge();
    if (!e)
      continue;
    vtol_edge_2d_sptr e2d = e->cast_to_edge_2d();
    edges.push_back(e2d);
  }
  ep.clear_output();
  //set the image and edges on the grouper and process intersections
  vil1_memory_image_of<float> flt =
    brip_vil1_float_ops::convert_to_float(this->get_input_image(0));
  epipolar_grouper_.set_image(flt);
  epipolar_grouper_.set_edges(frame_index_, edges);
  if (!epipolar_grouper_.group())
    return false;
  vcl_vector<vsol_polyline_2d_sptr> polys =
    epipolar_grouper_.display_segs(frame_index_);
  for (vcl_vector<vsol_polyline_2d_sptr>::iterator pit = polys.begin();
       pit != polys.end(); pit++)
  {
    vsol_spatial_object_2d_sptr so = (*pit)->cast_to_spatial_object();
    if (!so)
      continue;
    output_spat_objs_.push_back(so);
  }
  this->clear_input();
  return true;
}

