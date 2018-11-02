//:
// \file
// \brief A process to compute reconstruction error over all samples in a scene (test + train)
// \author Isabel Restrepo
// \date 13-Jan-2011

#include <iostream>
#include <cstdlib>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>
#include <bvpl/bvpl_octree/bvpl_pca_error_scenes.h>

#include <vul/vul_file.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// global variables
namespace bvpl_compute_pca_error_scene_process_globals
{
  constexpr unsigned n_inputs_ = 3;        //directory path, where pca_info.xml is
  constexpr unsigned n_outputs_ = 0;       //error file
}


// sets input and output types
bool bvpl_compute_pca_error_scene_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_compute_pca_error_scene_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string" ;  //directory path
  input_types_[i++] = "bvpl_pca_error_scenes_sptr";
  input_types_[i++] = "unsigned";  //num_components

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


#if BVPL_OCTREE_HAS_PTHREADS
// helper code to launch and manage pthreads
namespace {
  struct pthread_data
  {
    bvpl_discover_pca_kernels* pca_extractor;
    boxm_scene<boct_tree<short,float> > * error_scene;
    vgl_point_3d<int> block_idx;
    unsigned num_components;
  };

  void* pthread_launcher(void *arg)
  {
    pthread_data *pd = reinterpret_cast<pthread_data*>(arg);
    vgl_point_3d<int> block_idx = pd->block_idx;
    pd->pca_extractor->compute_testing_error_thread_safe(pd->error_scene, pd->num_components, block_idx.x(), block_idx.y(), block_idx.z());
    //delete pd;
    return arg;
  }
}
#endif

// the process
bool bvpl_compute_pca_error_scene_process(bprb_func_process& pro)
{
  using namespace bvpl_compute_pca_error_scene_process_globals;

  //get inputs
  unsigned i = 0;
  std::string pca_dir = pro.get_input<std::string>(i++);
  bvpl_pca_error_scenes_sptr pca_error_scenes = pro.get_input<bvpl_pca_error_scenes_sptr>(i++);
  auto num_components = pro.get_input<unsigned>(i++);

  if (!vul_file::is_directory(pca_dir))
    return false;

  if (!pca_error_scenes)
    return false;

  auto *pca_extractor = new bvpl_discover_pca_kernels(pca_dir);
  vgl_vector_3d<unsigned int> num_blocks = pca_extractor->data_scene_dim();
  boxm_scene<boct_tree<short, float> >* error_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (pca_error_scenes->get_scene(num_components).as_pointer());

#if BVPL_OCTREE_HAS_PTHREADS
  pca_extractor->load_all_scene_blocks();
  error_scene->read_all_blocks();
  unsigned num_threads = 0;
  std::vector<vgl_point_3d<int> > block_indices;
  for (unsigned block_i = 0; block_i < num_blocks.x(); ++block_i) {
    for (unsigned block_j = 0; block_j < num_blocks.y(); ++block_j) {
      for (unsigned block_k = 0; block_k < num_blocks.z(); ++block_k) {
        block_indices.push_back(vgl_point_3d<int>(block_i,block_j,block_k));
        ++num_threads;
      }
    }
  }

  pthread_t* thread_id = new pthread_t[num_threads]; // beware of memory leaks ...

  for (unsigned i =0; i < num_threads; ++i)
  {
    pthread_data* pd = new pthread_data();
    pd->pca_extractor = pca_extractor;
    pd->error_scene = error_scene;
    pd->num_components = num_components;
    pd->block_idx= block_indices[i];
    int rc = pthread_create(&thread_id[i], NULL, pthread_launcher, (void*) pd );
    //pthread_detach(thread_id[i]);
    if (rc)
    {
      std::cerr << "Error creating pthread, return code: "<<rc<<'\n';
      std::exit(-1);
    }
  }
#if 0
  for (unsigned i =0; i < num_threads; ++i)
  {
    pca_extractor.compute_testing_error_thread_safe(error_scene, num_components, block_indices[i].x(), block_indices[i].y(), block_indices[i].z());
  }
  pca_extractor.unload_all_scene_blocks();
#endif

#endif // PTHREADS
  return true;
}
