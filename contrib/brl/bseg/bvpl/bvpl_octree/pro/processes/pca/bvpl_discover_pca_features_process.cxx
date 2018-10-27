//:
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
  constexpr unsigned n_inputs_ = 9;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_discover_pca_features_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_discover_pca_features_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i =0;
  input_types_[i++] = "boxm_scene_base_sptr";
  input_types_[i++] = "vcl_string";   //directory to save PCA matrices
  input_types_[i++] = "double";   //fraction of the total number of leaves
  input_types_[i++] = "int";    // min and max of kernel
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "int";


  return pro.set_input_types(input_types_);
}


//:the process
bool bvpl_discover_pca_features_process(bprb_func_process& pro)
{
  using namespace bvpl_discover_pca_features_process_globals;
  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs
  unsigned i =0;
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  std::string pca_dir = pro.get_input<std::string>(i++);
  auto frac = pro.get_input<double>(i++);
  int min_x = pro.get_input<int>(i++);
  int min_y = pro.get_input<int>(i++);
  int min_z = pro.get_input<int>(i++);
  int max_x = pro.get_input<int>(i++);
  int max_y = pro.get_input<int>(i++);
  int max_z = pro.get_input<int>(i++);


  //check input's validity
  if (!scene_base.ptr()) {
    std::cout <<  " :-- Base Scene is not valid!\n";
    return false;
  }

  //neighborhood box for volume "patches" 5x5x5 for now - this could be an input to the process
  vgl_box_3d<int> neighborhood(vgl_point_3d<int>(min_x,min_y, min_z), vgl_point_3d<int>(max_x, max_y, max_z));

  //cast scene
  auto *scene= dynamic_cast<boxm_scene<boct_tree<short, float > >* > (scene_base.as_pointer());
  if (!scene) {
    std::cout <<  " :-- Input Scene is not of supported type\n";
    return false;
  }
  std::cout << "Scene path: " << scene->filename()<< std::endl;;


  //number of samples - 10% of total number of leaf-cells
  auto nsamples = (unsigned long)((double)scene->size() * frac);
  std::cout << "Number of samples: " << nsamples << std::endl;

  //bvpl_discover_pca_kernels pca_extractor(neighborhood, nsamples, scene);
  bvpl_discover_pca_kernels pca_extractor(neighborhood, nsamples, scene, pca_dir);

  //write matrices to disk
  pca_extractor.xml_write();

  return true;
}
