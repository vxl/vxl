//:
// \brief A process to compute pca projection coeffiecients
// \file
// \author Isabel Restrepo
// \date 28-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>

#include <vul/vul_file.h>

//:global variables
namespace bvpl_pca_project_process_globals 
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_pca_project_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_pca_project_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string" ;  //pca dir
  input_types_[i++] = "boxm_scene_base_sptr"; //projection scene
  input_types_[i++] = "int";   //block Indeces
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "unsigned";  //num_components
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_pca_project_process(bprb_func_process& pro)
{
  using namespace bvpl_pca_project_process_globals;
  
  //get inputs
  unsigned i = 0;
  vcl_string pca_dir = pro.get_input<vcl_string>(i++);
  boxm_scene_base_sptr proj_scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  unsigned num_components = pro.get_input<unsigned>(i++);
  
  if(!vul_file::is_directory(pca_dir))
    return false;
  
  if(!proj_scene_base)
    return false;
 
  boxm_scene<boct_tree<short, float> >* proj_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (proj_scene_base.as_pointer());
  
  bvpl_discover_pca_kernels pca_extractor(pca_dir);
  pca_extractor.project(proj_scene, num_components, block_i, block_j, block_k);
  
  return true;
}