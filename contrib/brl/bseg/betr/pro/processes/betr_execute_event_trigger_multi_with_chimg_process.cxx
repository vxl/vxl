// This is brl/bseg/betr/pro/processes/betr_execute_event_trigger_multi_with_chimg_process.cxx
#include <iostream>
#include <fstream>
#include <string>
#include <bprb/bprb_func_process.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <bpro/core/bbas_pro/bbas_1d_array_byte.h>
#include <bpro/core/bbas_pro/bbas_1d_array_int.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vil/vil_image_view.h>
//:
// \file
// \brief  A process for executing an event_trigger (process change)
//

#include <betr/betr_event_trigger.h>
#include <vgl/vgl_point_3d.h>

namespace betr_execute_event_trigger_multi_with_chimg_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 4;
}

bool betr_execute_event_trigger_multi_with_chimg_process_cons(bprb_func_process& pro)
{
  using namespace betr_execute_event_trigger_multi_with_chimg_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "betr_event_trigger_sptr"; //event_trigger
  input_types_[1] = "vcl_string"; //algorithm name
  input_types_[2] = "vcl_string"; //json prarameters
  // process has 4 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_double_sptr"; // change probability
  output_types_[1] = "bbas_1d_array_string_sptr"; // event region names
  output_types_[2] = "bbas_1d_array_int_sptr";    // dimensions and offset for each change image
  output_types_[3] = "bbas_1d_array_byte_sptr";   // byte pixels for all change images in one array
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool betr_execute_event_trigger_multi_with_chimg_process(bprb_func_process& pro)
{
  using namespace betr_execute_event_trigger_multi_with_chimg_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  betr_event_trigger_sptr event_trigger = pro.get_input<betr_event_trigger_sptr>(i++);
  std::string algorithm_name = pro.get_input<std::string>(i++);
  std::string algorithm_params = pro.get_input<std::string>(i);
  if(!event_trigger)
    return false;
  std::cout << "executing " << algorithm_name << " with params " << algorithm_params << std::endl;
  const std::map<std::string, betr_geo_object_3d_sptr>& evt_objs = event_trigger->evt_objects();
  unsigned n = evt_objs.size();
  bbas_1d_array_string_sptr evt_names = new bbas_1d_array_string(n);
  bbas_1d_array_double_sptr change_probs = new bbas_1d_array_double(n);
  bbas_1d_array_int_sptr dims_offset = new bbas_1d_array_int(4*n);
  unsigned j = 0;
  for(auto oit = evt_objs.begin();
      oit != evt_objs.end(); ++oit, ++j)
    evt_names->data_array[j]=oit->first;
  std::vector<double> prob_change;
  std::vector<vil_image_resource_sptr> change_imgs;
  std::vector<vgl_point_2d<unsigned> > offsets;
  bool good = event_trigger->process(algorithm_name, prob_change, change_imgs, offsets, algorithm_params);
  if(!good)
    return good;
  // get offset and size
  unsigned area_total = 0;
  unsigned k = 0;
  for(unsigned i = 0; i<n; ++i, k+=4){
    vil_image_resource_sptr imgr = change_imgs[i];
    if(!imgr)
      continue;
    unsigned ni = imgr->ni(), nj = imgr->nj();
    unsigned area = ni*nj;
    area_total += area;
    dims_offset->data_array[k]=ni;
    dims_offset->data_array[k+1]=nj;
    dims_offset->data_array[k+2]=offsets[i].x();
    dims_offset->data_array[k+3]=offsets[i].y();
  }
  bbas_1d_array_byte_sptr pix = new bbas_1d_array_byte(area_total);
  unsigned byte_index = 0;
  for(unsigned k = 0; k<n; ++k){
    vil_image_resource_sptr imgr = change_imgs[k];
    if(!imgr)
      continue;
    vil_image_view<vxl_byte> view = imgr->get_view();
    unsigned ni = imgr->ni(), nj = imgr->nj();
    for(unsigned j = 0; j<nj; ++j)
      for(unsigned i = 0; i<ni; ++i)
        pix->data_array[byte_index++]= view(i,j);
  }
  j = 0;
  for(auto pit = prob_change.begin();
      pit != prob_change.end(); ++pit, ++j)
    change_probs->data_array[j]=*pit;
  pro.set_output_val<bbas_1d_array_double_sptr>(0, change_probs);
  pro.set_output_val<bbas_1d_array_string_sptr>(1, evt_names);
  pro.set_output_val<bbas_1d_array_int_sptr>(2, dims_offset);
  pro.set_output_val<bbas_1d_array_byte_sptr>(3, pix);
  return good;
}
