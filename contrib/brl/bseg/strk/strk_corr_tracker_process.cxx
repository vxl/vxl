// This is brl/bseg/strk/strk_corr_tracker_process.cxx
#include <vcl_iostream.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_face_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <strk/strk_tracker.h>
#include <strk/strk_corr_tracker_process.h>

strk_corr_tracker_process::strk_corr_tracker_process(strk_tracker_params & tp)
 : tracker_(tp)
{
  failure_ = false;
  first_frame_ = true;
}

strk_corr_tracker_process::~strk_corr_tracker_process()
{
  tracker_.clear();
}


bool strk_corr_tracker_process::execute()
{
  if (failure_)
  {
    vcl_cout << "In strk_corr_tracker_process::execute() - process failed\n";
    return false;
  }
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In strk_corr_tracker_process::execute() -"
             << " not exactly one input image\n";
    failure_ = true;
    return false;
  }
  output_topo_objs_.clear();
  //assume the input images are grey scale (should really check)
  vil1_memory_image_of<unsigned char> img(vpro_video_process::get_input_image(0));
  input_images_.clear();

  if (first_frame_)
  {
    tracker_.set_image_0(img);
    if (!get_N_input_topo_objs())
    {
      vcl_cout << "In strk_corr_tracker_process::execute() -"
               << " no input correlation face\n";
      failure_ = true;
      return false;
    }
    vtol_topology_object_sptr to = input_topo_objs_[0];
    vtol_face_sptr f = to->cast_to_face();
    vtol_face_2d_sptr f2d = f->cast_to_face_2d();
    if (!f2d)
    {
      vcl_cout << "In strk_corr_tracker_process::execute() -"
               << " input is not a vtol_face_2d\n";
      failure_ = true;
      return false;
    }
    tracker_.set_initial_model(f2d);
    tracker_.init();
    vcl_vector<vtol_edge_sptr> edges_2d;
    f2d->edges(edges_2d);
    for (vcl_vector<vtol_edge_sptr>::iterator eit = edges_2d.begin();
         eit != edges_2d.end(); eit++)
    {
      vtol_topology_object_sptr to = (*eit)->cast_to_edge();
      output_topo_objs_.push_back(to);
    }
    first_frame_ = false;
    return true;
  }

  tracker_.set_image_i(img);
  tracker_.track();
  vtol_face_2d_sptr f = tracker_.get_best_sample();
  vcl_vector<vtol_edge_sptr> edges;
  f->edges(edges);
  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vtol_topology_object_sptr to = (*eit)->cast_to_edge();
    output_topo_objs_.push_back(to);
  }
  return true;
}
