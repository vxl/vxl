// This is brl/bseg/boxm2/volm/pro/processes/boxm2_load_hypotheses_process.cxx
//:
// \file
// \brief  A process for loading the hypotheses for a scene (e.g. created by exe generate_loc_hypotheses)
//
// \author Ozge C. Ozcanli
// \date Oct 21, 2012

#include <bprb/bprb_func_process.h>

#include <boxm2/volm/boxm2_volm_locations.h>
#include <boxm2/volm/boxm2_volm_locations_sptr.h>

namespace boxm2_load_hypotheses_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 2;
}
bool boxm2_load_hypotheses_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_load_hypotheses_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_volm_loc_hypotheses_sptr";
  output_types_[1] = "unsigned";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_load_hypotheses_process(bprb_func_process& pro)
{
  using namespace boxm2_load_hypotheses_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  vcl_string hyp_file = pro.get_input<vcl_string>(i++);
  boxm2_volm_loc_hypotheses_sptr h = new boxm2_volm_loc_hypotheses(hyp_file);

  i=0;
  pro.set_output_val<boxm2_volm_loc_hypotheses_sptr>(i++, h);
  pro.set_output_val<unsigned>(i++, (unsigned)h->locs_.size());
  return true;
}

namespace boxm2_save_hypotheses_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}
bool boxm2_save_hypotheses_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_save_hypotheses_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_volm_loc_hypotheses_sptr";  
  input_types_[1] = "vcl_string";

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_save_hypotheses_process(bprb_func_process& pro)
{
  using namespace boxm2_save_hypotheses_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_volm_loc_hypotheses_sptr h = pro.get_input<boxm2_volm_loc_hypotheses_sptr>(i++);
  vcl_string hyp_file = pro.get_input<vcl_string>(i++);
  h->write_hypotheses(hyp_file);
  return true;
}

namespace boxm2_get_hypothesis_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 3;
}
bool boxm2_get_hypothesis_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_get_hypothesis_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_volm_loc_hypotheses_sptr";
  input_types_[1] = "unsigned";  // id of the hypothesis

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "float";
  output_types_[1] = "float";
  output_types_[2] = "float";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_get_hypothesis_process(bprb_func_process& pro)
{
  using namespace boxm2_get_hypothesis_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_volm_loc_hypotheses_sptr h = pro.get_input<boxm2_volm_loc_hypotheses_sptr>(i++);
  unsigned k = pro.get_input<unsigned>(i++);
  if (k >= h->locs_.size())
    return false;
  i=0;
  pro.set_output_val<float>(i++, h->locs_[k].x());
  pro.set_output_val<float>(i++, h->locs_[k].y());
  pro.set_output_val<float>(i++, h->locs_[k].z());
  return true;
}


namespace boxm2_create_hypotheses_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 2;
}
bool boxm2_create_hypotheses_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_hypotheses_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "float"; // lat
  input_types_[1] = "float"; // lon
  input_types_[2] = "float"; // scale i in degrees (width of tile)
  input_types_[3] = "float"; // scale j in degrees (height of tile)
  input_types_[4] = "unsigned"; // ni
  input_types_[5] = "unsigned"; // nj

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_volm_loc_hypotheses_sptr";
  output_types_[1] = "unsigned";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_hypotheses_process(bprb_func_process& pro)
{
  using namespace boxm2_create_hypotheses_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  float lat = pro.get_input<float>(i++);
  float lon = pro.get_input<float>(i++);
  float scale_i = pro.get_input<float>(i++);
  float scale_j = pro.get_input<float>(i++);
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);
  volm_tile tt(lat, lon, scale_i, scale_j, ni, nj);
  boxm2_volm_loc_hypotheses_sptr h = new boxm2_volm_loc_hypotheses(tt);
  pro.set_output_val<boxm2_volm_loc_hypotheses_sptr>(0, h);
  return true;
}

namespace boxm2_add_hypothesis_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;
}
bool boxm2_add_hypothesis_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_add_hypothesis_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_volm_loc_hypotheses_sptr";
  input_types_[1] = "double";  // lon
  input_types_[2] = "double";  // lat
  input_types_[3] = "float";  // cent_x
  input_types_[4] = "float";  // cent_y
  input_types_[5] = "float";  // cent_z

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_add_hypothesis_process(bprb_func_process& pro)
{
  using namespace boxm2_add_hypothesis_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_volm_loc_hypotheses_sptr h = pro.get_input<boxm2_volm_loc_hypotheses_sptr>(i++);
  double lon = pro.get_input<double>(i++);
  double lat = pro.get_input<double>(i++);
  float cent_x = pro.get_input<float>(i++);
  float cent_y = pro.get_input<float>(i++);
  float cent_z = pro.get_input<float>(i++);
  if (!h->add(lon, lat, cent_x, cent_y, cent_z))
    return false;
  return true;
}
