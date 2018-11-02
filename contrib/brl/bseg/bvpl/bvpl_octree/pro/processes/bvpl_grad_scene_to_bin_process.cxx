//:
// \file
// \brief A process to save a gradient scene to a flat binary file
// \author Isabel Restrepo
// \date 3-Aug-2011

#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/boxm_scene.h>
#include <vnl/vnl_vector_fixed.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:global variables
namespace bvpl_grad_scene_to_bin_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_grad_scene_to_bin_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_grad_scene_to_bin_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i =0;
  input_types_[i++] = "boxm_scene_base_sptr";  //alpha scene
  input_types_[i++] = "boxm_scene_base_sptr";  //gradient scene
  input_types_[i++] = "vcl_string";            //output binary file

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_grad_scene_to_bin_process(bprb_func_process& pro)
{
  using namespace bvpl_grad_scene_to_bin_process_globals;

  //get inputs
  unsigned i = 0;
  boxm_scene_base_sptr alpha_scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  boxm_scene_base_sptr grad_scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  std::string output_file = pro.get_input<std::string>(i++);

  //get scene
  typedef boct_tree<short, float> float_tree_type;
  typedef boct_tree<short, vnl_vector_fixed< float,3 > > grad_tree_type;

  std::ofstream os(output_file.c_str(), std::ios::out);

  if (auto *alpha_scene = dynamic_cast<boxm_scene< float_tree_type >* >(alpha_scene_base.as_pointer()))
  {
    if (auto *grad_scene = dynamic_cast<boxm_scene< grad_tree_type >* >(grad_scene_base.as_pointer()))
    {
      double cell_length = alpha_scene->finest_cell_length();
      short finest_level = alpha_scene->finest_level();

      bool read_only = true;

      boxm_cell_iterator<float_tree_type > alpha_iter = alpha_scene->cell_iterator(&boxm_scene<float_tree_type >::load_block_and_neighbors, read_only);
      alpha_iter.begin();

      boxm_cell_iterator<grad_tree_type > grad_iter = grad_scene->cell_iterator(&boxm_scene<grad_tree_type >::load_block, read_only);
      grad_iter.begin();

      long unsigned n_grads = 0;

      while ( !(alpha_iter.end() || grad_iter.end()) )
      {
        boct_tree_cell<short, float> *alpha_cell = *alpha_iter;
        boct_tree_cell<short, vnl_vector_fixed< float,3 > > *grad_cell = *grad_iter;

        boct_loc_code<short> grad_code = grad_cell->get_code();
        boct_loc_code<short> alpha_code = alpha_cell->get_code();

        //if level and location code of cells isn't the same then continue
        if ((alpha_cell->level() != grad_cell->level()) || !(alpha_code.isequal(&grad_code))) {
          std::cerr << " Input and output cells don't have the same structure\n";
          ++alpha_iter;
          ++grad_iter;
          continue;
        }

        //we are only interested in finest resolution
        if ((!(alpha_cell->level() == finest_level)) || !alpha_cell->is_leaf()) {
          ++alpha_iter;
          ++grad_iter;
          continue;
        }

        vgl_point_3d<double> centroid = alpha_iter.global_centroid();
        vnl_vector_fixed< float,3 > grad = grad_cell->data();
        float p_x = 1.0f - (float)std::exp(- (alpha_cell->data() * cell_length ));
#if 0
        vsl_b_write(os, (float)centroid.x());
        vsl_b_write(os, (float)centroid.y());
        vsl_b_write(os, (float)centroid.z());
        vsl_b_write(os, grad[0]);
        vsl_b_write(os, grad[1]);
        vsl_b_write(os, grad[2]);
        vsl_b_write(os, p_x);
#endif

        os << (float)centroid.x() << ' '
           << (float)centroid.y() << ' '
           << (float)centroid.z() << ' '
           << grad[0] << ' '
           << grad[1] << ' '
           << grad[2] << ' '
           << p_x << '\n';

        ++alpha_iter;
        ++grad_iter;
        n_grads++;
      }

      grad_scene->unload_active_blocks();
      alpha_scene->unload_active_blocks();
      os.close();
      std::cout << "Wrote " << n_grads << " gradients\n";
    }
  }

  return true;
}
