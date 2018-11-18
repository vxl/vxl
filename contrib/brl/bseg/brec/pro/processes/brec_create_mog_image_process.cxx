// This is brl/bseg/brec/pro/processes/brec_create_mog_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to create a mixture of gaussian image of type bbgm_image_of from a bvxm_voxel_slab which should have been created for a voxel world from a given view using the corresponding bvxm process
//
// Summarizes the appearance model of the voxel world and constitutes a view-based background model
// Outputs the mixture in the bbgm format for binary io with that libraries methods
//
// \author Ozge Can Ozcanli
// \date December 15, 2008
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bbgm/bbgm_image_sptr.h>

template <bvxm_voxel_type APM_T>
bbgm_image_sptr create_bbgm_image(const bvxm_voxel_slab_base_sptr& mog_image_)
{
  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype mog_type;
  bbgm_image_sptr out_model_img_ = new bbgm_image_of<mog_type>();
  auto* out_model_img_ptr = dynamic_cast<bbgm_image_of<mog_type>* >(out_model_img_.ptr());
  out_model_img_ptr->set_size(mog_image_->nx(), mog_image_->ny());
  typename bbgm_image_of<mog_type>::iterator model_it = out_model_img_ptr->begin();

  auto* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>* >(mog_image_.ptr());
  typename bvxm_voxel_slab<mog_type>::const_iterator iter = mog_image_ptr->begin();

  for ( ; iter != mog_image_ptr->end(); iter++, ++model_it)
    *model_it = *iter;

  return out_model_img_;
}

//: Constructor
bool brec_create_mog_image_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bvxm_voxel_slab_base_sptr");
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("bbgm_image_sptr");  // output mog image as a bbgm distribution image
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_create_mog_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  bvxm_voxel_slab_base_sptr v_slab = pro.get_input<bvxm_voxel_slab_base_sptr>(i++);
  std::string voxel_type = pro.get_input<std::string>(i++);

  // turn the bvxm_slab into a bbgm_image_of instance for binary io (i.e. use processes in bbgm_pro)
  bbgm_image_sptr out_img;
  if (voxel_type == "apm_mog_grey")
    out_img = create_bbgm_image<APM_MOG_GREY>(v_slab);
  else if (voxel_type == "apm_mog_mc_3_3")
    out_img = create_bbgm_image<APM_MOG_MC_3_3>(v_slab);
  else if (voxel_type == "apm_mog_mc_4_3")
    out_img = create_bbgm_image<APM_MOG_MC_4_3>(v_slab);
  else {
    std::cout << "In brec_create_mog_image_process -- input appearance model: " << voxel_type << " is not supported\n";
    return false;
  }

  if (!out_img) {
    std::cout << "In brec_create_mog_image_process - problems in creating mixture of gaussian image!\n";
    return false;
  }

  // return the output img
  pro.set_output_val<bbgm_image_sptr>(0, out_img);

  return true;
}
