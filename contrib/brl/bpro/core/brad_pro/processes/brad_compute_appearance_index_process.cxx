// This is brl/bpro/core/brad_pro/processes/brad_compute_appearance_index_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_appearance_neighborhood_index.h>
#include <bbas_pro/bbas_1d_array_string.h>
#include <bbas_pro/bbas_1d_array_int.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_compute_appearance_index_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("bbas_1d_array_string_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types; 
  output_types.push_back("bbas_1d_array_int_sptr"); 
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_compute_appearance_index_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() != 1) {
    vcl_cout << "brad_compute_appearance_index_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the input
  bbas_1d_array_string_sptr filenames = pro.get_input<bbas_1d_array_string_sptr>(0);
  unsigned n = static_cast<unsigned>((filenames->data_array).size());
  std::vector<vbl_smart_ptr<brad_image_metadata> > metadata;
  for(unsigned i = 0; i<n; ++i){
    vbl_smart_ptr<brad_image_metadata> meta_ptr = new brad_image_metadata();
    if(!meta_ptr->parse_from_meta_file((filenames->data_array)[i]))
      continue;
    metadata.push_back(meta_ptr);
  }
  // create the index 
  brad_appearance_neighborhood_index idx(metadata);
#if 0
  // debug jlm ====================
  unsigned mnv = idx.most_nadir_view();
  idx.force_single_index(mnv);
  //===============================
#endif
  std::map<unsigned, std::vector<unsigned> > index = idx.index();
  unsigned total_size = 0;
  for(std::map<unsigned, std::vector<unsigned> >::const_iterator iit = index.begin();
      iit != index.end(); ++iit){
    total_size++;
    total_size += static_cast<unsigned>((iit->second).size());
    total_size++; // for marker
  }
  bbas_1d_array_int_sptr index_array = new bbas_1d_array_int(total_size);
  unsigned midx = 0;
  for(std::map<unsigned, std::vector<unsigned> >::const_iterator iit = index.begin();
      iit != index.end(); ++iit){
    unsigned target_idx = iit->first;
    (index_array->data_array)[midx++] = target_idx;
    const std::vector<unsigned>& illum_neighbors =index[target_idx];
    for(std::vector<unsigned>::const_iterator iit = illum_neighbors.begin();
        iit != illum_neighbors.end(); ++iit)
      (index_array->data_array)[midx++] = *iit;
    midx++; // leave a -1 marker
  }
  pro.set_output_val<bbas_1d_array_int_sptr>(0,index_array);

  return true;
}

