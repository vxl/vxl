//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/vpro/vpro_corr_tracker_process.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_face_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <sdet/sdet_tracker.h>
#include <vpro/vpro_corr_tracker_process.h>

vpro_corr_tracker_process::vpro_corr_tracker_process(sdet_tracker_params & tp)
 : tracker_(tp) 
{
  failure_ = false;
  first_frame_ = true;
}

vpro_corr_tracker_process::~vpro_corr_tracker_process()
{
  tracker_.clear();
}

bool vpro_corr_tracker_process::execute()
{
  if(failure_)
    {
      vcl_cout << "In vpro_corr_tracker_process::execute() - process failed\n";
      return false;
    }
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In vpro_corr_tracker_process::execute() - not exactly one"
               << " input image \n";
      failure_ = true;
      return false;
    }
  output_topo_objs_.clear();
  //assume the input images are grey scale (should really check)
  vil1_memory_image_of<unsigned char> img(vpro_video_process::get_input_image(0));
  input_images_.clear();
  if(first_frame_)
    {
      tracker_.set_image_0(img);
      if(!get_N_input_topo_objs())
        {
          vcl_cout << "In vpro_corr_tracker_process::execute() - no input"
                   << "correlation face  \n";
          failure_ = true;
          return false;
        }
      vtol_topology_object_sptr to = input_topo_objs_[0];
      vtol_face_sptr f = to->cast_to_face();
      vtol_face_2d_sptr f2d = f->cast_to_face_2d();
      if(!f2d)
        {
          vcl_cout << "In vpro_corr_tracker_process::execute() - input"
                   << "is not a vtol_face_2d  \n";
          failure_ = true;
          return false;
        }
      tracker_.set_initial_model(f2d);
      tracker_.init();
      output_topo_objs_.push_back((vtol_topology_object*)f2d->cast_to_face());
      first_frame_ = false;
      return true;
    }
  
  tracker_.set_image_i(img);
  tracker_.track();
  //  vcl_vector<vtol_face_2d_sptr> samples;
  //  tracker_.get_samples(samples);
  //   for(vcl_vector<vtol_face_2d_sptr>::iterator fit = samples.begin();
  //       fit != samples.end(); fit++)
  //     {
  //       vtol_topology_object_sptr to = 
  //         (vtol_topology_object*)((*fit)->cast_to_face());
  //       output_topo_objs_.push_back(to);
  //     }
  vtol_face_2d_sptr f = tracker_.get_best_sample();
  vtol_topology_object_sptr to = 
    (vtol_topology_object*)(f->cast_to_face());
  output_topo_objs_.push_back(to);
  return true;
}

