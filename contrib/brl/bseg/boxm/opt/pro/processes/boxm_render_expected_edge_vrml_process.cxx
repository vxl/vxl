// This is brl/bseg/boxm/opt/pro/processes/boxm_render_expected_edge_vrml_process.cxx
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
#include <boxm/boxm_render_image.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_sample_multi_bin.h>
#include <boxm/boxm_inf_line_sample.h>
#include <bvrml/bvrml_write.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_intersection.h>
#include <vcl_fstream.h>

namespace boxm_render_expected_edge_vrml_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}

bool boxm_render_expected_edge_vrml_process_cons(bprb_func_process& pro)
{
  using namespace boxm_render_expected_edge_vrml_process_globals;
  //process takes 4 inputs
  //input[0]: scene binary file
  //input[1]: camera
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "float";
  input_types_[4] = "int";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 output:
  // output[0]: rendered image
  // output[0]: mask
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  //output_types_[0] = "vil_image_view_base_sptr";
  //output_types_[1] = "vil_image_view_base_sptr";
  //if (!pro.set_output_types(output_types_))
  //  return false;

  return true;
}

bool boxm_render_expected_edge_vrml_process(bprb_func_process& pro)
{
  using namespace boxm_render_expected_edge_vrml_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cerr << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  vcl_string path = pro.get_input<vcl_string>(i++);
  float threshold = pro.get_input<float>(i++);
  int s = pro.get_input<int>(i++);

  vcl_ofstream stream(path.c_str());
  // number of updates
  //unsigned num_updates = pro.get_input<unsigned>(i++);
  //vil_image_view_base_sptr img;
  //vil_image_view_base_sptr img_mask;

  if (scene_ptr->appearence_model() == BOXM_EDGE_LINE) {
//    vil_image_view<boxm_apm_traits<BOXM_EDGE_LINE>::obs_datatype> expected(ni,nj,3);
//    vil_image_view<float> mask(ni,nj);
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_inf_line_sample<float> > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      if (!scene) {
        vcl_cout << "boxm_render_expected_edge_process: the scene is not of expected type" << vcl_endl;
        return false;
      }
      //////////////////////////////
      bvrml_write::write_vrml_header(stream);
      vgl_point_3d<double> origin(450.0,450.0,0.0);
      vgl_vector_3d<double> dirx(1,0,0);
      vgl_vector_3d<double> diry(0,1,0);
      vgl_vector_3d<double> dirz(0,0,1);
      bvrml_write::write_vrml_line(stream,origin,dirx,3,1,0,0);
      bvrml_write::write_vrml_line(stream,origin,diry,3,0,1,0);
      bvrml_write::write_vrml_line(stream,origin,dirz,3,0,0,1);

      //vgl_vector_3d<unsigned> dim=grid->grid_size();
      //write the colors
      // for each block
      boxm_block_iterator<type> iter(scene);
      iter.begin();
      while (!iter.end())
      {
        scene->load_block(iter.index());
        boxm_block<type>* block = *iter;
        type* tree = block->get_tree();
        vcl_vector<boct_tree_cell<short,boxm_inf_line_sample<float> >*> cells = tree->leaf_cells();
        
        // iterate over cells
        for (unsigned i=0; i<cells.size(); ++i)
        {
          boct_tree_cell<short,boxm_inf_line_sample<float> >* cell = cells[i];
          boxm_inf_line_sample<float> data = cell->data();
          vgl_infinite_line_3d<float> line = data.line_;
          // TODO: revise with segment length
          vgl_vector_3d<double> dir(line.direction().x(), line.direction().y(), line.direction().z());
          if (line.x0().x()!=0 && line.x0().y()!=0) {
            vgl_point_3d<double> p0,p1;
            vgl_box_3d<double> bb = tree->cell_bounding_box(cell);
            // convert to line type (from float to double)
            vgl_vector_2d<double> x0(line.x0().x(), line.x0().y());
            vgl_infinite_line_3d<double> dline(x0,dir);
            if (vgl_intersection<double>(bb, dline, p0, p1)) {
              vgl_vector_3d<double> dir(p1-p0);
              double length=dir.length();
              dir/=length;
              bvrml_write::write_vrml_line(stream, p0,dir,length,1.f,0.f,0.f);
            }
#if 0
            vgl_point_3d<double> point = tree->global_origin(cell);
            // find the center of the cell
            double l = tree->cell_length(cell);
            point.set(point.x()+l/2.0, point.y()+l/2.0, point.z()+l/2.0);
            bvrml_write::write_vrml_line(stream, point,dir,4.0f*(1.0f-.5f),1.f,0.f,0.f);
           // bvrml_write::write_vrml_disk(stream, point,dir,2*(1.0-.5f),0.f,.5f,0.f);
#endif
          }
        }
        iter++;
      }
      //boxm_render_edge_tangent_image_rt<short, boxm_inf_line_sample<float> >(*scene, camera, expected, mask,n_normal,num_updates);
      //////////////////////////////
    }
    else
    {
      vcl_cerr << "Ray tracing version not yet implemented\n";
      return false;
    }
 /*   img_mask = new vil_image_view<float>(mask);

    vil_image_view<float> *ex = new vil_image_view<float>(ni,nj,expected.nplanes());
    for (unsigned i=0; i<ni; i++) {
      for (unsigned j=0;j<nj; j++) {
        (*ex)(i,j) = expected(i,j);//static_cast<unsigned char>(255.0*(expected(i,j)));
      }
    }
    img = ex;*/
  } else {
    vcl_cerr << "boxm_render_expected_edge_vrml_process: undefined APM type\n";
    return false;
  }

 // unsigned j = 0;
 // pro.set_output_val<vil_image_view_base_sptr>(j++, img);
 // pro.set_output_val<vil_image_view_base_sptr>(j++, img_mask);
  return true;
}
