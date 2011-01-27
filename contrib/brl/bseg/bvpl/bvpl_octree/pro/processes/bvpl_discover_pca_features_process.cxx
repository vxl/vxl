//:
// \brief
// \file
// \author Isabel Restrepo
// \date 17-Nov-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/boxm_scene.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>

//:global variables
namespace bvpl_discover_pca_features_process_globals 
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_discover_pca_features_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_discover_pca_features_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string"; //directory to save PCA matrices
  input_types_[2] = "double"; //fraction of the total number of leaves
  
  
  return pro.set_input_types(input_types_);
}


//:the process
bool bvpl_discover_pca_features_process(bprb_func_process& pro)
{
  using namespace bvpl_discover_pca_features_process_globals;
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << ": the input number should be " << n_inputs_
    << " but instead it is " << pro.n_inputs() << vcl_endl;
    return false;
  }
  
  //get inputs
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  vcl_string pca_dir = pro.get_input<vcl_string>(1);
  double frac = pro.get_input<double>(2);
  
  //check input's validity
  if (!scene_base.ptr()) {
    vcl_cout <<  " :-- Base Scene is not valid!\n";
    return false;
  }
  
  //neighborhood box for volume "patches" 5x5x5 for now - this could be an input to the process
  vgl_box_3d<int> neighborhood(vgl_point_3d<int>(-2,-2,-2), vgl_point_3d<int>(2,2,2));
  
  //cast scene
  boxm_scene<boct_tree<short, float > > *scene= dynamic_cast<boxm_scene<boct_tree<short, float > >* > (scene_base.as_pointer());
  if (!scene) {
    vcl_cout <<  " :-- Input Scene is not of supported type\n";
    return false;
  }
  
  //number of samples - 10% of total number of leaf-cells
  unsigned long nsamples = (unsigned long)((double)scene->size() * frac);
  vcl_cout << "Number of samples: " << nsamples << vcl_endl;
  
  //bvpl_discover_pca_kernels pca_extractor(neighborhood, nsamples, scene);
  bvpl_discover_pca_kernels pca_extractor(neighborhood, nsamples, scene, pca_dir);
  
  //write matrices to disk
  pca_extractor.xml_write();
  
  return true;
}