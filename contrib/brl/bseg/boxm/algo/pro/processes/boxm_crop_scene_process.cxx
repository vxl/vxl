// This is brl/bseg/boxm/algo/pro/processes/boxm_crop_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for cropping boxm_scene.
// \author Isabel Restrepo
// \date January 07, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_parameters.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_crop_scene.h>
#include <boxm/boxm_sample.h>

namespace boxm_crop_scene_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}


//: set input and output types
bool boxm_crop_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_crop_scene_process_globals;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0]="boxm_scene_base_sptr"; //the input scene
  
  //The output grid
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0]="boxm_scene_base_sptr";
  
  if (!(pro.set_input_types(input_types_) && pro.set_output_types(output_types_))) 
    return false;
  
  return true;
}


//: Execute the process
bool boxm_crop_scene_process(bprb_func_process& pro)
{
  using namespace boxm_crop_scene_process_globals;
  
  
  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << "The number of inputs should be " <<n_inputs_ << vcl_endl;
    return false;
  }
  
  double corner_x = 0;
  pro.parameters()->get_value("corner_x", corner_x);
  double corner_y = 0;
  pro.parameters()->get_value("corner_y", corner_y);
  double corner_z = 0;
  pro.parameters()->get_value("corner_z", corner_z);
  
  
  double dimx = 10;
  pro.parameters()->get_value("dim_x", dimx);
  double dimy = 10;
  pro.parameters()->get_value("dim_y", dimy);
  double dimz = 10;
  pro.parameters()->get_value("dim_z", dimz);
  
  unsigned i=0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  
  if (!scene_ptr) {
    vcl_cerr << "In bvxm_crop_scene_process -- input scene is null!\n";
    return false;
  }
  
  vgl_box_3d<double> bbox(vgl_point_3d<double>(corner_x, corner_y, corner_z), 
                          vgl_point_3d<double>((corner_x + dimx), (corner_y + dimy), (corner_z + dimz)));
  
  //figure out the type
  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY)
  {
    if (!scene_ptr->multi_bin())
    {
      boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > *scene_in = 
      dynamic_cast<boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > >* > (scene_ptr.ptr());
      pro.set_output_val<boxm_scene_base_sptr>(0, boxm_crop_scene<boxm_sample<BOXM_APM_MOG_GREY> >(scene_in, bbox));
      return true;
      
    }
    
  }
  if (scene_ptr->appearence_model() == FLOAT)
  {
    if (!scene_ptr->multi_bin())
    {
      boxm_scene<boct_tree<short,float > > *scene_in = 
      dynamic_cast<boxm_scene<boct_tree<short,float > >* > (scene_ptr.ptr());
      pro.set_output_val<boxm_scene_base_sptr>(0, boxm_crop_scene<float >(scene_in, bbox));
      return true;
    }
  } 
 vcl_cerr<< "Scene type not supported in boxm_crop_scene_process" << vcl_endl;
 return false;

}
