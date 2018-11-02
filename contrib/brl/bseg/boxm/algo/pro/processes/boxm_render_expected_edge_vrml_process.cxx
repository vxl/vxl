// This is brl/bseg/boxm/algo/pro/processes/boxm_render_expected_edge_vrml_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for obtaining roc curve from change detection results.
//
// \author Gamze Tunali
// \date Apr 14, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#include <bvrml/bvrml_write.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_intersection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace boxm_render_expected_edge_vrml_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm_render_expected_edge_vrml_process_cons(bprb_func_process& pro)
{
  using namespace boxm_render_expected_edge_vrml_process_globals;

  // process takes 4 inputs and no output
  //input[0]: scene binary file
  //input[1]: the path for vrml file
  //input[2]: threshold
  //input[3]: s (to write every s cell)
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "float";
  input_types_[3] = "int";

  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_render_expected_edge_vrml_process(bprb_func_process& pro)
{
  using namespace boxm_render_expected_edge_vrml_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cerr << pro.name() << ": The input number should be " << n_inputs_<< '\n';
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  //vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  std::string path = pro.get_input<std::string>(i++);
  auto threshold = pro.get_input<float>(i++);
  int s = pro.get_input<int>(i++); // FIXME - unused!

  std::ofstream stream(path.c_str());

  if (scene_ptr->appearence_model() == BOXM_EDGE_LINE)
  {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_inf_line_sample<float> > type;
      auto* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      if (!scene) {
        std::cout << "boxm_render_expected_edge_process: the scene is not of expected type" << std::endl;
        return false;
      }
      bvrml_write::write_vrml_header(stream);
#if 0
      vgl_point_3d<double> origin(0.0,0.0,0.0);
      vgl_vector_3d<double> dirx(1,0,0);
      vgl_vector_3d<double> diry(0,1,0);
      vgl_vector_3d<double> dirz(0,0,1);
      bvrml_write::write_vrml_line(stream,origin,dirx,3,1,0,0);
      bvrml_write::write_vrml_line(stream,origin,diry,3,0,1,0);
      bvrml_write::write_vrml_line(stream,origin,dirz,3,0,0,1);
#endif
      // for each block
      boxm_block_iterator<type> iter(scene);
      iter.begin();
      while (!iter.end())
      {
        scene->load_block(iter.index());
        boxm_block<type>* block = *iter;
        type* tree = block->get_tree();
        std::vector<boct_tree_cell<short,boxm_inf_line_sample<float> >*> cells = tree->leaf_cells();

        // iterate over cells
        for (auto cell : cells)
        {
          boxm_inf_line_sample<float> data = cell->data();
          vgl_infinite_line_3d<float> line = data.line_;
          vgl_vector_2d<double> x0(line.x0().x(), line.x0().y());
          // TODO: revise with segment length
          if (!(line.x0().x()==0 && line.x0().y()==0)) {
            //if (data.num_obs_ > threshold)
            {
              vgl_point_3d<double> p0,p1;
              vgl_box_3d<double> bb = tree->cell_bounding_box(cell);
              // convert to line type (from float to double)
              vgl_vector_3d<double> dir(line.direction().x(), line.direction().y(), line.direction().z());

              vgl_infinite_line_3d<double> dline(x0,dir);
              if (vgl_intersection<double>(bb, dline, p0, p1)) {
                vgl_vector_3d<double> dir(p1-p0);
                double length=dir.length();
                dir/=length;
                std::cout<<data.residual_<<' ';
                if (data.residual_<threshold)
                {
                  bvrml_write::write_vrml_line(stream, p0,dir,float(length),1.f,0.f,0.f);
                  //bvrml_write::write_vrml_disk(stream,p0,dir,data.residual_,0.f,1.0f,0.f);
                }
              }
            }
          }
        }
        iter++;
      }
    }
    else
    {
      std::cerr << "Ray tracing version not yet implemented\n";
      return false;
    }
  }
  else {
    std::cerr << "boxm_render_expected_edge_vrml_process: undefined APM type\n";
    return false;
  }

  // no output
  return true;
}
